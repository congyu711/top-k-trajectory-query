#include <cryptopp/integer.h>
#include <cryptopp/eccrypto.h>
#include <cryptopp/elgamal.h>
#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>
#include <cryptopp/oids.h>
#include <iostream>
#include <iomanip>
#include "function_declaration.h"
using namespace CryptoPP;
using namespace std;

Integer e;
Integer v;

ElGamal_keys Proxy_ReEncryption::ElGamal_key_generation(){
    //private_key
    Integer x(prng,Integer::One(),g.GetMaxExponent());

    //public_key
    Integer y = g.ExponentiateBase(x);
    
    Integer order = g.GetGroupOrder();
    Integer generator = g.GetGenerator();

    return ElGamal_keys(x,y,order,generator);
}

void Proxy_ReEncryption::ElGamal_Encryption(string &plain_txt,string &cipher_txt,Integer publickey)
{
    ElGamal::Encryptor e0;
    e0.AccessKey().AccessGroupParameters().Initialize(g);
    e0.AccessKey().SetPublicElement(publickey);
    StringSource ss(plain_txt,true,new PK_EncryptorFilter(prng,e0,new StringSink(cipher_txt)));
}

void Proxy_ReEncryption::ElGamal_Decryption(string &cipher_txt,string &decryption_txt,Integer privatekey)
{
    ElGamal::Decryptor d0;
    d0.AccessKey().AccessGroupParameters().Initialize(g);
    d0.AccessKey().SetPrivateExponent(privatekey);
    StringSource ss(cipher_txt,true,new PK_DecryptorFilter(prng,d0,new StringSink(decryption_txt)));
}

//generate (public_key,private_Key)
pair<Integer,Element> Proxy_ReEncryption::ECC_key_generation(Integer x){
    GroupParameters group;
    group.Initialize(ASN1::secp256r1());

    x = x % group.GetGroupOrder();
    // cout<<group.GetGroupOrder()<<"\n";
        
    // public key
    Element y = group.ExponentiateBase(x);
     
    return make_pair(x,y);
}

void Proxy_ReEncryption::ECIES_Encryption(string plain_txt,Element PublicKey,string& cipher_txt){
    ECIES<ECP>::Encryptor e0;
    e0.AccessKey().AccessGroupParameters().Initialize(ASN1::secp256r1());
    e0.AccessKey().SetPublicElement(PublicKey);
    StringSource ss(plain_txt,true,new PK_EncryptorFilter(prng,e0,new StringSink(cipher_txt)));
}

void Proxy_ReEncryption::ECIES_Decryption(string cipher_txt,Integer PrivateKey,string& plain_txt){
    ECIES<ECP>::Decryptor d0;
    d0.AccessKey().AccessGroupParameters().Initialize(ASN1::secp256r1());
    // cout<<"K1: "<<PrivateKey<<"\n";
    d0.AccessKey().SetPrivateExponent(PrivateKey);
    StringSource ss(cipher_txt,true,new PK_DecryptorFilter(prng,d0,new StringSink(plain_txt)));
    
}

pair<vector<string>,Capsule> Proxy_ReEncryption::Pre_Enc(vector<string> plain_txt,Integer A_privatekey,Integer B_publickey){ 
    Integer min = Integer("100000");
    Integer max = Integer("10000000000");
    SHA256 hash;
    string digest;
    // random_num e,v
    e = Integer(prng,min,max);
    v = Integer(prng,min,max);
    Integer E = a_exp_b_mod_c(gen,e,m);
    Integer V = a_exp_b_mod_c(gen,v,m);
    string hash_str = Integer_to_string(E) + Integer_to_string(V);
    StringSource ss(hash_str,true ,new HashFilter(hash,new HexEncoder(new StringSink(digest))));
    Integer s = v + e*to_Integer(digest);
    Integer k = (a_exp_b_mod_c(B_publickey,A_privatekey,m)*v)%m;
    // cout <<"k1: "<<k <<endl;
    pair<Integer,Element> K = ECC_key_generation(k);
    // cout<<"K: "<<K.first<<"\n";
    vector<string> m_Enc;
    for(string a:plain_txt)
    {
        string tmp;
        ECIES_Encryption(a,K.second,tmp);
        m_Enc.push_back(tmp);
    }
    return make_pair(m_Enc,Capsule(E,V,s));
}

pair<Integer,Integer> Proxy_ReEncryption::Pre_ReKeyGen(Integer A_privatekey,Integer B_publickey){
    Integer x = Integer(prng,Integer("1000000000"),Integer("100000000000000"));
    SHA256 hash;
    Integer X = a_exp_b_mod_c(gen,x,m);
    string hash_str = Integer_to_string(X)+Integer_to_string(B_publickey)+Integer_to_string(a_exp_b_mod_c(B_publickey,x,m));
    string digest;
    StringSource ss(hash_str,true,new HashFilter(hash,new HexEncoder(new StringSink(digest))));
    // cout << "d1: " << digest<<endl;
    Integer d_inverse = to_Integer(digest).InverseMod(m);
    Integer rk = e*d_inverse;
    return make_pair(rk,X);
}

Capsule Proxy_ReEncryption::Pre_ReEncryption(Integer rk,Capsule cap){
    Integer a = a_exp_b_mod_c(gen,cap.s,m);
    SHA256 hash;
    string digest;
    Integer E1;
    Integer V1;
    Integer r = Integer(prng,Integer("1000000"),Integer("100000000000000"));
    StringSource ss((Integer_to_string(cap.E)+Integer_to_string(cap.V)),true,new HashFilter(hash,new HexEncoder(new StringSink(digest))));
    Integer b = (cap.V*a_exp_b_mod_c(cap.E,to_Integer(digest),m))%m;
    if(a == b){
        E1 = r*rk;
        V1 = r.InverseMod(m)*to_Integer(digest);
    }
    else{
        cout<<"error"<<endl;
    }
    // Integer E1 = a_exp_b_mod_c(cap.E,rk,m);
    // Integer V1 = a_exp_b_mod_c(cap.V,rk,m);
    return Capsule(E1,V1,cap.s);
}

Integer Proxy_ReEncryption::Pre_ReCreateKey(Integer B_privatekey,Integer B_publickey,Integer A_publickey,Capsule cap,Integer X){
    SHA256 hash;
    string digest;
    string hash_str = Integer_to_string(X)+Integer_to_string(B_publickey)+Integer_to_string(a_exp_b_mod_c(X,B_privatekey,m));
    StringSource ss(hash_str,true,new HashFilter(hash,new HexEncoder(new StringSink(digest))));
    cout << "d2: " <<digest<<endl;
    Integer ex = (cap.s%m-cap.E*cap.V*to_Integer(digest)%m);
    // cout<<"v: "<<ex<<"\n";
    cout<<"x: "<<a_exp_b_mod_c(A_publickey,B_privatekey,m)<<"\n";
    Integer k = (a_exp_b_mod_c(A_publickey,B_privatekey,m)*ex)%m;
    // cout<<"k2: "<< k <<endl;
    pair<Integer,Element> K = ECC_key_generation(k);
    return K.first;
}

vector<string> Proxy_ReEncryption::Pre_Decryption(Integer privatekey,vector<string> cipher_txt){
    vector<string> m_dec;
    for(string a:cipher_txt)
    {
        string tmp;
        ECIES_Decryption(a,privatekey,tmp);
        m_dec.push_back(tmp);
    }
    return m_dec;
}

// #ifdef __Proxy_ReEncryption_test__
// int main(){
// }
// #endif