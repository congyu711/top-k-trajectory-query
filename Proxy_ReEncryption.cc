#include <cryptopp/integer.h>
#include <cryptopp/eccrypto.h>
#include <cryptopp/elgamal.h>
#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>
// #include <cryptopp/hashfwd.h>
#include <cryptopp/oids.h>
// #include <cryptopp/base64.h>
#include <iostream>
#include <iomanip>
using namespace CryptoPP;
using namespace std;
typedef DL_GroupParameters_EC<ECP> GroupParameters;
typedef DL_GroupParameters_EC<ECP>::Element Element;

AutoSeededRandomPool prng;
DL_GroupParameters_ElGamal g;
Integer m,gen;

class ElGamal_keys
{
public:
    Integer privatekey,publickey,order,generator;
    ElGamal_keys(Integer _privatekey,Integer _publickey,Integer _order,Integer _gen):
        privatekey(_privatekey),publickey(_publickey),order(_order),generator(_gen){}
};

class Capsule
{
public:
    Integer E,V,s;
    Capsule(Integer _E,Integer _V,Integer _s):
        E(_E),V(_V),s(_s){}
};

ElGamal_keys ElGamal_key_generation(){
    //private_key
    Integer x(prng,Integer::One(),g.GetMaxExponent());

    //public_key
    Integer y = g.ExponentiateBase(x);
    
    Integer order = g.GetGroupOrder();
    Integer generator = g.GetGenerator();

    return ElGamal_keys(x,y,order,generator);
}

//generate (public_key,private_Key)
pair<Integer,Element> ECC_key_generation(Integer x){
    GroupParameters group;
    group.Initialize(ASN1::secp256r1());

    x = x % group.GetGroupOrder();
        
    // public key
    Element y = group.ExponentiateBase(x);
     
    return make_pair(x,y);
}

//outpt the cipher_txt in hex
void printstring(const string& str, ostream& out=cout)
{
    string o;
    for(int i=0;i<str.length();i++)
    {
        stringstream ss;
        string a;
        int num=0;
        uint tmp=str[i];
        for(int j=0;j<8;j++)    {num+=((tmp&0x80)>>7)<<(8-j-1);tmp<<=1;}
        ss << hex << num;
        ss >> a;
        o += a;
    }
    out<<"Cipher_txt: "<<o<<endl;
}

void ECIES_Encryption(string plain_txt,Element PublicKey,string& cipher_txt){
    // ECIES<ECP>::Decryptor d0;
    // d0.AccessKey().AccessGroupParameters().Initialize(ASN1::secp256r1());
    // d0.AccessKey().SetPrivateExponent(Alice_privatekey);
    ECIES<ECP>::Encryptor e0;
    e0.AccessKey().AccessGroupParameters().Initialize(ASN1::secp256r1());
    e0.AccessKey().SetPublicElement(PublicKey);
    StringSource ss(plain_txt,true,new PK_EncryptorFilter(prng,e0,new StringSink(cipher_txt)));
}

void ECIES_Decryption(string cipher_txt,Integer PrivateKey,string& plain_txt){
    ECIES<ECP>::Decryptor d0;
    d0.AccessKey().AccessGroupParameters().Initialize(ASN1::secp256r1());
    d0.AccessKey().SetPrivateExponent(PrivateKey);
    StringSource ss(cipher_txt,true,new PK_DecryptorFilter(prng,d0,new StringSink(plain_txt)));
    
}

Integer to_Integer(string a){
    Integer b;
    for (auto i = 0;i < a.length()-1;i++){
        b = (Integer(a[i]) - Integer("48")) + b*Integer("10");
    }
    return b;
}

string Integer_to_string(Integer x){
    stringstream ss;
    ss<<x;
    string y(ss.str());
    return y;
}

pair<string,Capsule> Pre_Enc(string plain_txt,Integer A_publickey){ 
    Integer min = Integer("1000000000");
    Integer max = Integer("100000000000000");
    SHA256 hash;
    string digest;
    // random_num e,v
    Integer e = Integer(prng,min,max);
    Integer v = Integer(prng,min,max);

    Integer E = a_exp_b_mod_c(gen,e,m);
    Integer V = a_exp_b_mod_c(gen,v,m);
    string hash_str = Integer_to_string(E) + Integer_to_string(V);
    StringSource ss(hash_str,true ,new HashFilter(hash,new HexEncoder(new StringSink(digest))));
    Integer s = v + e*to_Integer(digest);
    Integer k = a_exp_b_mod_c(A_publickey,e+v,m);
    // cout <<"k1: "<<k <<endl;
    pair<Integer,Element> K = ECC_key_generation(k);
    string m_Enc;
    ECIES_Encryption(plain_txt,K.second,m_Enc);
    return make_pair(m_Enc,Capsule(E,V,s));
}

pair<Integer,Integer> Pre_ReKeyGen(Integer A_privatekey,Integer B_publickey){
    Integer x = Integer(prng,Integer("1000000000"),Integer("100000000000000"));
    SHA256 hash;
    Integer X = a_exp_b_mod_c(gen,x,m);
    string hash_str = Integer_to_string(X)+Integer_to_string(B_publickey)+Integer_to_string(a_exp_b_mod_c(B_publickey,x,m));
    string digest;
    StringSource ss(hash_str,true,new HashFilter(hash,new HexEncoder(new StringSink(digest))));
    // cout << "d1: " << digest<<endl;
    Integer d_inverse = to_Integer(digest).InverseMod(m-Integer::One());
    Integer rk = A_privatekey*d_inverse;
    return make_pair(rk,X);
}

Capsule Pre_ReEncryption(Integer rk,Capsule cap){
    Integer a = a_exp_b_mod_c(gen,cap.s,m);
    SHA256 hash;
    string digest;
    Integer E1;
    Integer V1;
    StringSource ss((Integer_to_string(cap.E)+Integer_to_string(cap.V)),true,new HashFilter(hash,new HexEncoder(new StringSink(digest))));
    Integer b = (cap.V*a_exp_b_mod_c(cap.E,to_Integer(digest),m))%m;
    if(a == b){
        E1 = a_exp_b_mod_c(cap.E,rk,m);
        V1 = a_exp_b_mod_c(cap.V,rk,m);
    }
    else{
        cout<<"error"<<endl;
    }
    // Integer E1 = a_exp_b_mod_c(cap.E,rk,m);
    // Integer V1 = a_exp_b_mod_c(cap.V,rk,m);
    return Capsule(E1,V1,cap.s);
}

Integer Pre_ReCreateKey(Integer B_privatekey,Integer B_publickey,Capsule cap,Integer X){
    SHA256 hash;
    string digest;
    string hash_str = Integer_to_string(X)+Integer_to_string(B_publickey)+Integer_to_string(a_exp_b_mod_c(X,B_privatekey,m));
    StringSource ss(hash_str,true,new HashFilter(hash,new HexEncoder(new StringSink(digest))));
    // cout << "d2: " <<digest<<endl;
    Integer k = a_exp_b_mod_c(cap.E*cap.V,to_Integer(digest),m);
    // cout<<"k2: "<< k <<endl;
    pair<Integer,Element> K = ECC_key_generation(k);
    return K.first;
}

string Pre_Decryption(Integer privatekey,string cipher_txt){
    string m_dec;
    ECIES_Decryption(cipher_txt,privatekey,m_dec);
    cout << m_dec <<endl;
    return m_dec;
}

#ifdef __Proxy_ReEncryption_test__
int main(){
    string plain_txt = "Proxy Re-Encryption";
    g.Initialize(prng,1024);
    ElGamal_keys A_key = ElGamal_key_generation();
    ElGamal_keys B_key = ElGamal_key_generation();
    gen = A_key.generator;
    m = A_key.order + Integer::One();
    pair<string,Capsule> a = Pre_Enc(plain_txt,A_key.publickey);
    pair<Integer,Integer> b = Pre_ReKeyGen(A_key.privatekey,B_key.publickey);
    Capsule cap = Pre_ReEncryption(b.first,a.second);
    Integer k = Pre_ReCreateKey(B_key.privatekey,B_key.publickey,cap,b.second);
    string m_dec = Pre_Decryption(k,a.first);
    cout << m_dec;
    return 0;
}
#endif