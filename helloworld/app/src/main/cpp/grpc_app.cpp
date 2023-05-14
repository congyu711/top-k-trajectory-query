#include <jni.h>
#include "hilbertcurve.cpp"
#include <cryptopp/integer.h>
#include <cryptopp/eccrypto.h>
#include <cryptopp/elgamal.h>
#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>
#include <cryptopp/oids.h>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;
using namespace CryptoPP;

typedef DL_GroupParameters_EC<ECP> GroupParameters;
typedef DL_GroupParameters_EC<ECP>::Element Element;
// typedef DL_GroupParameters_EC<ECP>::EllipticCurve Ec;
// typedef DL_GroupParameters<CryptoPP::ECP::Point> Point;
AutoSeededRandomPool prng;

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

Integer to_Integer(const string& a){
    Integer b;
    for (auto i = 0;i < a.length()-1;i++){
        b = int(a[i]-'0') + b*10;
    }
    return b;
}

string Integer_to_string(const Integer& x){
    stringstream ss;
    ss<<x;
    string y(ss.str());
    return y;
}

double Integer_to_double(const Integer& x)
{
    stringstream ss;
    ss<<x;
    double b;
    ss>>b;
    return b;
}

struct Point
{
    double time;
    double x;
    double y;
    Point(double _time = 0.0, double _x = 0.0, double _y = 0.0) : time(_time), x(_x), y(_y) {}
    bool operator==(Point a)
    {
        if (a.x == this->x && a.y == this->y)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
};

struct Phi
{
    uint order;
    pair<int, int> origin;
    char direcion;
    double gamma;
    // Phi():order(0),origin(make_pair(0,0)),direcion('u'),gamma(0.0){}
    Phi() {}
    Phi(uint _order, int _x, int _y, char _direcion, double _gamma) : order(_order), origin(make_pair(_x, _y)), direcion(_direcion), gamma(_gamma) {}
};

class ElGamal_keys
{
public:
    Integer privatekey, publickey, order, generator;
    ElGamal_keys() : privatekey(Integer::Zero()), publickey(Integer::Zero()), order(Integer::Zero()), generator(Integer::Zero()) {}
    ElGamal_keys(Integer _privatekey, Integer _publickey, Integer _order, Integer _gen) : privatekey(_privatekey), publickey(_publickey), order(_order), generator(_gen) {}
};

class Capsule
{
public:
    Integer E, V, s;
    Capsule(Integer _E, Integer _V, Integer _s) : E(_E), V(_V), s(_s) {}
    Capsule() : E(Integer::Zero()), V(Integer::Zero()), s(Integer::Zero()) {}
};

class Proxy_ReEncryption
{
private:
    pair<Integer, Element> ECC_key_generation(Integer x);
    DL_GroupParameters_ElGamal g;
    Integer m, gen;
    // Integer n,k;
    // Ec ec;
    // Point p;

public:
    void ECIES_Encryption(string plain_txt, Element PublicKey, string &cipher_txt);
    void ECIES_Decryption(string cipher_txt, Integer PrivateKey, string &plain_txt);
    pair<vector<string>, Capsule> Pre_Enc(vector<string> plain_txt, Integer A_privatekey, Integer B_publickey);
    pair<Integer, Integer> Pre_ReKeyGen(Integer A_privatekey, Integer B_publickey);
    Capsule Pre_ReEncryption(Integer rk, Capsule cap);
    Integer Pre_ReCreateKey(Integer B_privatekey, Integer B_publickey, Integer A_publickey, Capsule cap, Integer X);
    vector<string> Pre_Decryption(Integer privatekey, vector<string> cipher_txt);
    ElGamal_keys ElGamal_key_generation();
    void ElGamal_Encryption(string &plain_txt, string &cipher_txt, Integer publickey);
    void ElGamal_Decryption(string &cipher_txt, string &decryption_txt, Integer privatekey);
    string ElGamal_Decryption(string &cipher_txt, Integer privatekey);
    Proxy_ReEncryption()
    {
        m = Integer("842876432176263314988599011300607670790928782165361660406383");
        gen = Integer("2");
        g.Initialize(m, gen);
    }
};

Integer e;
Integer v;

ElGamal_keys Proxy_ReEncryption::ElGamal_key_generation()
{
    // private_key
    Integer x(prng, Integer::One(), g.GetMaxExponent());

    // public_key
    Integer y = g.ExponentiateBase(x);

    Integer order = g.GetGroupOrder();
    Integer generator = g.GetGenerator();

    return ElGamal_keys(x, y, order, generator);
}

void Proxy_ReEncryption::ElGamal_Encryption(string &plain_txt, string &cipher_txt, Integer publickey)
{
    ElGamal::Encryptor e0;
    e0.AccessKey().AccessGroupParameters().Initialize(g);
    e0.AccessKey().SetPublicElement(publickey);
    StringSource ss(plain_txt, true, new PK_EncryptorFilter(prng, e0, new StringSink(cipher_txt)));
}

void Proxy_ReEncryption::ElGamal_Decryption(string &cipher_txt, string &decryption_txt, Integer privatekey)
{
    ElGamal::Decryptor d0;
    d0.AccessKey().AccessGroupParameters().Initialize(g);
    d0.AccessKey().SetPrivateExponent(privatekey);
    printstring(cipher_txt);
    StringSource ss(cipher_txt, true, new PK_DecryptorFilter(prng, d0, new StringSink(decryption_txt)));
}
string Proxy_ReEncryption::ElGamal_Decryption(string &cipher_txt, Integer privatekey)
{
    ElGamal::Decryptor d0;
    d0.AccessKey().AccessGroupParameters().Initialize(g);
    d0.AccessKey().SetPrivateExponent(privatekey);
    printstring(cipher_txt);
    string res;
    StringSource ss(cipher_txt, true, new PK_DecryptorFilter(prng, d0, new StringSink(res)));
    return res;
}

// generate (public_key,private_Key)
pair<Integer, Element> Proxy_ReEncryption::ECC_key_generation(Integer x)
{
    GroupParameters group;
    group.Initialize(ASN1::secp256r1());

    x = x % group.GetGroupOrder();

    // public key
    Element y = group.ExponentiateBase(x);

    return make_pair(x, y);
}

void Proxy_ReEncryption::ECIES_Encryption(string plain_txt, Element PublicKey, string &cipher_txt)
{
    ECIES<ECP>::Encryptor e0;
    e0.AccessKey().AccessGroupParameters().Initialize(ASN1::secp256r1());
    e0.AccessKey().SetPublicElement(PublicKey);
    StringSource ss(plain_txt, true, new PK_EncryptorFilter(prng, e0, new StringSink(cipher_txt)));
}

void Proxy_ReEncryption::ECIES_Decryption(string cipher_txt, Integer PrivateKey, string &plain_txt)
{
    ECIES<ECP>::Decryptor d0;
    d0.AccessKey().AccessGroupParameters().Initialize(ASN1::secp256r1());
    d0.AccessKey().SetPrivateExponent(PrivateKey);
    StringSource ss(cipher_txt, true, new PK_DecryptorFilter(prng, d0, new StringSink(plain_txt)));
}

pair<vector<string>, Capsule> Proxy_ReEncryption::Pre_Enc(vector<string> plain_txt, Integer A_privatekey, Integer B_publickey)
{
    Integer min = Integer("100000");
    Integer max = Integer("10000000000");
    SHA256 hash;
    string digest;
    // random_num e,v
    e = Integer(prng, min, max);
    v = Integer(prng, min, max);
    Integer E = a_exp_b_mod_c(gen, e, m);
    Integer V = a_exp_b_mod_c(gen, v, m);
    string hash_str = Integer_to_string(E) + Integer_to_string(V);
    StringSource ss(hash_str, true, new HashFilter(hash, new HexEncoder(new StringSink(digest))));
    Integer s = v + e * to_Integer(digest);
    Integer k = (a_exp_b_mod_c(B_publickey, A_privatekey, m) * v) % m;
    pair<Integer, Element> K = ECC_key_generation(k);
    vector<string> m_Enc;
    for (string a : plain_txt)
    {
        string tmp;
        ECIES_Encryption(a, K.second, tmp);
        m_Enc.push_back(tmp);
    }
    return make_pair(m_Enc, Capsule(E, V, s));
}

pair<Integer, Integer> Proxy_ReEncryption::Pre_ReKeyGen(Integer A_privatekey, Integer B_publickey)
{
    Integer x = Integer(prng, Integer("1000000000"), Integer("100000000000000"));
    SHA256 hash;
    Integer X = a_exp_b_mod_c(gen, x, m);
    string hash_str = Integer_to_string(X) + Integer_to_string(B_publickey) + Integer_to_string(a_exp_b_mod_c(B_publickey, x, m));
    string digest;
    StringSource ss(hash_str, true, new HashFilter(hash, new HexEncoder(new StringSink(digest))));
    Integer d_inverse = to_Integer(digest).InverseMod(m);
    Integer rk = e * d_inverse;
    return make_pair(rk, X);
}

Capsule Proxy_ReEncryption::Pre_ReEncryption(Integer rk, Capsule cap)
{
    Integer a = a_exp_b_mod_c(gen, cap.s, m);
    SHA256 hash;
    string digest;
    Integer E1;
    Integer V1;
    Integer r = Integer(prng, Integer("1000000"), Integer("100000000000000"));
    StringSource ss((Integer_to_string(cap.E) + Integer_to_string(cap.V)), true, new HashFilter(hash, new HexEncoder(new StringSink(digest))));
    Integer b = (cap.V * a_exp_b_mod_c(cap.E, to_Integer(digest), m)) % m;
    if (a == b)
    {
        E1 = r * rk;
        V1 = r.InverseMod(m) * to_Integer(digest);
    }
    else
    {
        cerr << "Capsule Proxy_ReEncryption::Pre_ReEncryption(Integer, Capsule) error\n";
    }
    return Capsule(E1, V1, cap.s);
}

Integer Proxy_ReEncryption::Pre_ReCreateKey(Integer B_privatekey, Integer B_publickey, Integer A_publickey, Capsule cap, Integer X)
{
    SHA256 hash;
    string digest;
    string hash_str = Integer_to_string(X) + Integer_to_string(B_publickey) + Integer_to_string(a_exp_b_mod_c(X, B_privatekey, m));
    StringSource ss(hash_str, true, new HashFilter(hash, new HexEncoder(new StringSink(digest))));
    // cout << "d2: " << digest << endl;
    Integer ex = (cap.s % m - cap.E * cap.V * to_Integer(digest) % m);
    // cout << "v: " << ex << "\n";
    // cout << "x: " << a_exp_b_mod_c(A_publickey, B_privatekey, m) << "\n";
    // cout << "A_publickey = " << A_publickey << "\n";
    // cout << "B_privatekey" << B_privatekey << "\n";
    Integer k = (a_exp_b_mod_c(A_publickey, B_privatekey, m) * ex) % m;
    // cout << "k2: " << k << endl;
    pair<Integer, Element> K = ECC_key_generation(k);
    return K.first;
}

vector<string> Proxy_ReEncryption::Pre_Decryption(Integer privatekey, vector<string> cipher_txt)
{
    vector<string> m_dec;
    for (string a : cipher_txt)
    {
        string tmp;
        // cout << "privatekey = " << privatekey << "\n";
        printstring(a);
        ECIES_Decryption(a, privatekey, tmp);
        m_dec.push_back(tmp);
    }
    return m_dec;
}

void Phi_Dec(vector<string> &cipher_txt, Proxy_ReEncryption &Pre_ReEnc, vector<string> &phi_dec, Integer privatekey)
{
    phi_dec.clear();
    // string order = cipher_txt[0];
    // string origin1 = cipher_txt[1];
    // string origin2 = cipher_txt[2];
    // string direction = cipher_txt[3];
    // string gamma = cipher_txt[4];
    // phi_dec.push_back(Pre_ReEnc.ElGamal_Decryption(order, privatekey));
    // phi_dec.push_back(Pre_ReEnc.ElGamal_Decryption(origin1, privatekey));
    // phi_dec.push_back(Pre_ReEnc.ElGamal_Decryption(origin2, privatekey));
    // phi_dec.push_back(Pre_ReEnc.ElGamal_Decryption(direction, privatekey));
    // phi_dec.push_back(Pre_ReEnc.ElGamal_Decryption(gamma, privatekey));
    for(auto &txts:cipher_txt)
    {
        phi_dec.push_back(Pre_ReEnc.ElGamal_Decryption(txts, privatekey));
    }
}

void query_encode(vector<Point> &trajectionList, Phi phi, vector<pair<double, string>> &encodingList)
{
    hilbertcurve hc({phi.order, phi.origin, phi.direcion, phi.gamma});
    for (auto a : trajectionList)
    {
        pair<double, string> tmp;
        tmp = make_pair(a.time, hc.xy2H[a.x][a.y]);
        encodingList.push_back(tmp);
    }
}

void Elgamal_Enc(const vector<pair<double, string>> &qurey, Proxy_ReEncryption &Pre_ReEnc, vector<pair<double, string>> &cipher_txt, const Integer &publickey)
{
    for (auto a : qurey)
    {
        string tmp;
        Pre_ReEnc.ElGamal_Encryption(a.second, tmp, publickey);
        cipher_txt.push_back(make_pair(a.first, tmp));
    }
}

void Elgamal_Dec(const vector<pair<double, string>> &cipher_txt, Proxy_ReEncryption &Pre_ReEnc, vector<pair<double, string>> &dec_txt, Integer privatekey)
{
    vector<pair<double, string>> tmp1;
    // cout << 1 << "\n";
    //for (auto x : cipher_txt)
    //{
    //    printstring(x.second);
    //}
    // cout << 2 << "\n";
    for (auto a : cipher_txt)
    {
        string tmp;
        // cout << "privatekey = " << privatekey << endl;
        //printstring(a.second);
        Pre_ReEnc.ElGamal_Decryption(a.second, tmp, privatekey);
        tmp1.push_back(make_pair(a.first, tmp));
        // cout << a.first << " " << tmp << "\n";
    }
    dec_txt = tmp1;
}

void decode(vector<vector<Point>> &trajectionList, Phi phi, vector<vector<pair<double, string>>> &encodingList)
{
    hilbertcurve hc({phi.order, phi.origin, phi.direcion, phi.gamma});
    vector<vector<Point>> tmp1;
    for (auto a : encodingList)
    {
        vector<Point> tmp;
        for (auto b : a)
        {
            pair<uint, uint> x = hc.H2xy[b.second];
            tmp.push_back({b.first, (double)x.first, (double)x.second});
        }
        tmp1.push_back(tmp);
    }
    trajectionList = tmp1;
}

class QueryUser: public Proxy_ReEncryption
{
public:
    ElGamal_keys QU_key;
    vector<Point> search_qu;
    Proxy_ReEncryption Pre;
    vector<string> phi_dec;//the decryption of cipherTxt(Phi)
    Phi phi;//parameters of hilbertCurve
    vector<pair<double,string>> encoding_qu;
    vector<pair<double,string>> qu_Enc;
    vector<string> txt_Dec; //Proxy_Encryption: decrypt by reCreateKey
    Integer reCreateKey; //Proxy_Encryption generate the reCreateKey
    vector<vector<Point>> dec_result;

    QueryUser() {
        //search_qu = {Point(0.0,1,1),Point(1.0,2,2),Point(2.0,3,3)};
        //generate the ElGamal_key(PubKey and PrivateKey) for QU
        QU_key = Pre.ElGamal_key_generation();
    }

    ElGamal_keys generator_key(){
        QU_key = Pre.ElGamal_key_generation();
    }

    void decrypt_Phi(vector<string> &ciphertxt_Phi,vector<string> &phi_dec,Integer &privatekey) {
        Phi_Dec(ciphertxt_Phi,Pre,phi_dec,privatekey);
        uint order = strtoul(phi_dec[0].c_str(),NULL,10);
        int x = atoi(phi_dec[1].c_str());
        int y = atoi(phi_dec[2].c_str());
        char direction = phi_dec[3][0];
        double gamma = strtod(phi_dec[4].c_str(),NULL);
        phi = Phi(order,x,y,direction,gamma);
    }

    void Encode_Query(vector<Point> &search_qu,Phi &phi,vector<pair<double,string>> &encoding_qu) {
        query_encode(search_qu,phi,encoding_qu);
    }

    void Encrypt_Query(vector<Point> &search_qu,Phi &phi,vector<pair<double,string>> &encoding_qu,Proxy_ReEncryption &Pre_ReEnc,vector<pair<double,string>> &qu_Enc,Integer &CS1_key_publickey) {
        Encode_Query(search_qu,phi,encoding_qu);
        Elgamal_Enc(encoding_qu,Pre_ReEnc,qu_Enc,CS1_key_publickey);
    }

    vector<string> Decrypt_ID(Proxy_ReEncryption &Pre_ReEnc,Integer &privatekey,Integer &publickey,Integer &DO_key_publickey,Capsule &cap,Integer &conversion_key,vector<string> &kid) {
        reCreateKey = Pre_ReEnc.Pre_ReCreateKey(privatekey,publickey,DO_key_publickey,cap,conversion_key);
        txt_Dec = Pre_ReEnc.Pre_Decryption(reCreateKey,kid);
        return txt_Dec;
    }

    vector<vector<Point>> Decrypt_Result(vector<vector<pair<double,string>>> &Enc_result,Proxy_ReEncryption Pre_ReEnc,Phi &phi,Integer &privatekey) {
        vector<vector<pair<double,string>>> tmp1;
        for(auto a:Enc_result) {
            vector<pair<double,string>> tmp;
            Elgamal_Dec(a,Pre_ReEnc,tmp,privatekey);
            tmp1.push_back(tmp);
        }
        decode(dec_result,phi,tmp1);
        return dec_result;
    }
};

QueryUser QU;

//generate the Elgamal key for QU
extern "C"
JNIEXPORT jobjectArray JNICALL
Java_io_grpc_helloworldexample_Cryptopp_stringFromJNI(JNIEnv *env, jclass clazz) {
    Integer publicKey = QU.QU_key.publickey;
    string a = Integer_to_string(publicKey);
    string b = Integer_to_string(QU.QU_key.privatekey);
    vector<string> tmp({a,b});
    jobjectArray QU_key;
    jclass stringArrCls = env->FindClass("java/lang/String");
    QU_key = env->NewObjectArray(2,stringArrCls,NULL);
    for (int i = 0; i < 2; ++i) {
        jstring tmp1 = env->NewStringUTF(tmp[i].c_str());
        env->SetObjectArrayElement(QU_key,i,tmp1);
    }
    return QU_key;
}

string jstringToChar(JNIEnv *env,jstring str) {
    char* rtn = nullptr;
    jclass strCls = env->FindClass("java/lang/String");
    jstring strEncode = env->NewStringUTF("UFT-16");
    jmethodID mid = env->GetMethodID(strCls,"getBytes","(Ljava/lang/String;)[B");
    auto barr = (jbyteArray) env->CallObjectMethod(str,mid,strEncode);
    jsize len = env->GetArrayLength(barr);
    jbyte* ba = env->GetByteArrayElements(barr,JNI_FALSE);
    if(len > 0) {
        rtn = (char*) malloc(len+1);
        memcpy(rtn,ba,len);
        rtn[len] = 0;
    }
    env->ReleaseByteArrayElements(barr,ba,0);
    return {rtn};
}

//
extern "C" JNIEXPORT jstring JNICALL
Java_io_grpc_helloworldexample_Cryptopp_DecryptPhi(JNIEnv *env,jclass clazz, jbyteArray decryption,jstring key) {
    int length = env->GetArrayLength(decryption);
    Integer privateKey = Integer(env->GetStringUTFChars(key,nullptr));
    Proxy_ReEncryption pre;
    vector<string> a;
    string tmp1;
    tmp1.assign((char*)env->GetByteArrayElements(decryption,JNI_FALSE),length);
    a.push_back(tmp1);
//    for (int i = 0; i < length; ++i) {
//        auto tmp =(jstring) env->GetObjectArrayElement(decryption,i);
//        string tmp1 = jstringToChar(env,tmp);
//        a.push_back(tmp1);
//    }
    vector<string> phi_dec;
    Phi_Dec(a,pre,phi_dec,privateKey);
    //QU.decrypt_Phi(a,phi_dec,privateKey);
//    jclass uInt = env->FindClass("com/google/common/primitives/UnsignedInteger");
//    jobject tmp = env->NewObject(uInt,env->GetMethodID(uInt,"<init>", "(I)V"),(int) QU.phi.order);
//    jclass phiCls = env->FindClass("io/grpc/helloworldexample/Phi");
//    jmethodID init = env->GetMethodID(phiCls,"<init>", "(IIICD)V");
//    if(init == NULL) return NULL;
//    //jobject Phi = env->NewObject(phiCls,init,(int)QU.phi.order,QU.phi.origin.first,QU.phi.origin.second,QU.phi.direcion,QU.phi.gamma);
//    jobject Phi = env->NewObject(phiCls,init,8,7,0,'l',1.000);
    return env->NewStringUTF(phi_dec[0].c_str());
}

extern "C" JNIEXPORT jobjectArray JNICALL
Java_io_grpc_helloworldexample_Cryptopp_hilbert(JNIEnv *env,jclass clazz, jobject phi,jstring CS1PublicKey,jobjectArray query) {
    auto length = env->GetArrayLength(query);
//    vector<string> a(length);
    Proxy_ReEncryption pre;
    vector<Point> b;
    jclass pointArrCls = env->FindClass("io/grpc/helloworldexample/EncodePoint");
    jclass stringArrCls = env->GetObjectClass(env->GetObjectArrayElement(query,0));
    jfieldID timeField = env->GetFieldID(stringArrCls,"time","D");
    jfieldID xField = env->GetFieldID(stringArrCls,"x","I");
    jfieldID yField = env->GetFieldID(stringArrCls,"y","I");
    jobjectArray result = env->NewObjectArray(length,pointArrCls,NULL);
    for (int i = 0; i < length; ++i) {
        auto tmp = env->GetObjectArrayElement(query,i);
        jdouble time = env->GetDoubleField(tmp,timeField);
        jint x = env->GetIntField(tmp,xField);
        jint y = env->GetIntField(tmp,yField);
        Point p = Point(time,x,y);
        b.push_back(p);
    }
    jclass phiCls = env->FindClass("io/grpc/helloworldexample/Phi");
    jfieldID orderField = env->GetFieldID(phiCls,"order",
                                          "I");
    jfieldID originXField = env->GetFieldID(phiCls,"origin_x", "I");
    jfieldID originYField = env->GetFieldID(phiCls,"origin_y","I");
    jfieldID dietField = env->GetFieldID(phiCls,"direction", "C");
    jfieldID gammaField = env->GetFieldID(phiCls,"gamma", "D");
//    uint order = env->CallIntMethod(
//            env->GetObjectField(phi,orderField),
//            env->GetMethodID(
//                    env->FindClass(
//            "com/google/common/primitives/UnsignedInteger"),
//            "intValue",
//            "()I"));
    uint order = env->GetIntField(phi,orderField);
    int x = env->GetIntField(phi,originXField);
    int y = env->GetIntField(phi,originYField);
    char direction = env->GetCharField(phi,dietField);
    double gamma = env->GetDoubleField(phi,gammaField);
    Integer CS1PublicKey1 = Integer(env->GetStringUTFChars(CS1PublicKey,NULL));
    Phi phi1 = Phi(order,x,y,direction,gamma);
    QU.Encrypt_Query(b,phi1,QU.encoding_qu,QU.Pre,QU.qu_Enc,CS1PublicKey1);
    jmethodID init = env->GetMethodID(pointArrCls,"<init>", "(D[B)V");
    for(int i=0;i<length;i++) {
        auto *by = (jbyte*) QU.qu_Enc.at(i).second.c_str();
        jsize len = QU.qu_Enc.at(i).second.length();
        jbyteArray array = env->NewByteArray(len);
        env->SetByteArrayRegion(array,0,len,by);
        auto tmp = env->NewObject(pointArrCls,init,QU.qu_Enc.at(i).first,array);
        env->SetObjectArrayElement(result,i,tmp);
    }
    return result;
}

extern "C" JNIEXPORT jstring JNICALL
Java_io_grpc_helloworldexample_Cryptopp_DecryptID(JNIEnv *env,jclass clazz,jstring DOPublicKey,jobjectArray QUKey,jobject cap,jstring conversionKey,jbyteArray kid) {
    Integer DO_PublicKey = Integer(env->GetStringUTFChars(DOPublicKey,NULL));
    Integer conversionkey = Integer(env->GetStringUTFChars(conversionKey,NULL));
    jclass ArrCls = env->FindClass("java/lang/String");
    jsize length = env->GetArrayLength(kid);
    vector<string> cKid;
    vector<Integer> QU_key;
//    for (int i = 0; i < length; ++i) {
//        auto tmp = (jstring) env->GetObjectArrayElement(kid,i);
//        cKid.emplace_back(env->GetStringUTFChars(tmp,nullptr));
//    }
    string byte;
    byte.assign((char *)env->GetByteArrayElements(kid,JNI_FALSE),length);
    cKid.emplace_back(byte);
    for (int i = 0; i < 2; ++i) {
        auto tmp = (jstring) env->GetObjectArrayElement(QUKey,i);
        QU_key.emplace_back(Integer(env->GetStringUTFChars(tmp,nullptr)));
    }
    jclass capCls = env->FindClass("io/grpc/helloworldexample/Cap");
    jfieldID e1 = env->GetFieldID(capCls,"E", "Ljava/lang/String;");
    jfieldID v1 = env->GetFieldID(capCls,"V", "Ljava/lang/String;");
    jfieldID s = env->GetFieldID(capCls,"S", "Ljava/lang/String;");
    string E = string(env->GetStringUTFChars(jstring (env->GetObjectField(cap, e1)), NULL));
    string V = string(env->GetStringUTFChars(jstring (env->GetObjectField(cap, v1)), NULL));
    string S = string(env->GetStringUTFChars(jstring (env->GetObjectField(cap, s)), NULL));
    Capsule cap1 = Capsule(Integer(E.c_str()),Integer(V.c_str()),Integer(S.c_str()));
    QU.Decrypt_ID(QU.Pre,QU_key.at(1),QU_key.at(0),DO_PublicKey,cap1,conversionkey,cKid);
//    jobjectArray result;
//    result = env->NewObjectArray(length,ArrCls,nullptr);
//    for (int i = 0; i < length; ++i) {
//        jstring tmp = env->NewStringUTF(QU.txt_Dec.at(i).c_str());
//        env->SetObjectArrayElement(result,i,tmp);
//    }
    return env->NewStringUTF(QU.txt_Dec.at(0).c_str());
}

extern "C" JNIEXPORT jobjectArray JNICALL
Java_io_grpc_helloworldexample_Cryptopp_DecryptRes(JNIEnv *env,jclass clazz, jobjectArray Enc_res,jobject phi,jstring privateKey) {
    jsize row = env->GetArrayLength(Enc_res);
    vector<vector<pair<double,string>>> encResult;
    vector<pair<double,string>> tmp;
    jclass encodePointCls = env->FindClass("io/grpc/helloworldexample/EncodePoint");
    jfieldID time = env->GetFieldID(encodePointCls,"time","D");
    jfieldID hPoint = env->GetFieldID(encodePointCls,"hPoint", "[B");
    for (int i = 0; i < row; ++i) {
        jobject point = env->GetObjectArrayElement(Enc_res,i);
        double t = env->GetDoubleField(point,time);
        auto array = (jbyteArray) env->GetObjectField(point, hPoint);
        string hpoint;
        hpoint.assign((char *)env->GetByteArrayElements(array,JNI_FALSE),env->GetArrayLength(array));
        tmp.emplace_back(t,hpoint);
    }
    encResult.push_back(tmp);
    Integer quPrivateKey(env->GetStringUTFChars(privateKey,nullptr));
    jclass phiCls = env->FindClass("io/grpc/helloworldexample/Phi");
    jfieldID xField = env->GetFieldID(phiCls,"origin_x","I");
    jfieldID yField = env->GetFieldID(phiCls,"origin_y","I");
    jfieldID orderField = env->GetFieldID(phiCls,"order",
                                          "I");
    jfieldID dietField = env->GetFieldID(phiCls,"direction", "C");
    jfieldID gammaField = env->GetFieldID(phiCls,"gamma", "D");
//    uint order = env->CallIntMethod(env->GetObjectField(phi,orderField),env->GetMethodID(env->FindClass(
//            "com/google/common/primitives/UnsignedInteger"),"intValue", "()I"));
    uint order = env->GetIntField(phi,orderField);
    int x = env->GetIntField(phi,xField);
    int y = env->GetIntField(phi,yField);
    char direction = env->GetCharField(phi,dietField);
    double gamma = env->GetDoubleField(phi,gammaField);
    Phi phi1 = Phi(order,x,y,direction,gamma);
    QU.Decrypt_Result(encResult,QU.Pre,phi1,quPrivateKey);
    int length = QU.dec_result[0].size();
    jclass pointCls = env->FindClass("io/grpc/helloworldexample/MyPoint");
    jmethodID construct = env->GetMethodID(pointCls,"<init>", "(DII)V");
    jobjectArray result = env->NewObjectArray(length,pointCls,NULLPTR);
    for (int i = 0; i < length; ++i) {
        jobject tmp1 = env->NewObject(pointCls,construct,QU.dec_result[0][i].time,int(QU.dec_result[0][i].x),int(QU.dec_result[0][i].y));
        env->SetObjectArrayElement(result,i,tmp1);
        env->DeleteLocalRef(tmp1);
    }
    return result;
}