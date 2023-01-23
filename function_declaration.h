#include <bits/stdc++.h>
#include <crypto++/integer.h>
#include <cryptopp/eccrypto.h>
#include <cryptopp/elgamal.h>
#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>
#include <cryptopp/oids.h>
using namespace std;
using namespace CryptoPP;

typedef DL_GroupParameters_EC<ECP> GroupParameters;
typedef DL_GroupParameters_EC<ECP>::Element Element;
AutoSeededRandomPool prng;

struct Point
{
    double time;
    double x;
    double y;
    Point(double _time=0.0,double _x=0.0,double _y=0.0): time(_time),x(_x),y(_y){}
    bool operator == (Point a)
    {
        if(a.x==this->x&&a.y==this->y)
        {
            return true;
        }
        else{
            return false;
        }
    }
};

struct Phi
{
    uint order;
    pair<int,int> origin;
    char direcion;
    double gamma;
};

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

class Douglas_Peucker
{
private:
    float distanceofPointtoLine(const Point &p,const vector<Point> &line);
    void condense(const vector<Point> &pointList,vector<int> &keyPointIndexList,int firstIndex,int endIndex,const float &thrDistance);
public:
    Douglas_Peucker();
    bool execute(const vector<Point> &pointList,vector<Point> &resultPointList,const float &thrdistance);
};

class Proxy_ReEncryption
{
private:
    pair<Integer,Element> ECC_key_generation(Integer x);
    DL_GroupParameters_ElGamal g;
    Integer m,gen;
public:
    void ECIES_Encryption(string plain_txt,Element PublicKey,string& cipher_txt);
    void ECIES_Decryption(string cipher_txt,Integer PrivateKey,string& plain_txt);
    pair<vector<string>,Capsule> Pre_Enc(vector<string> plain_txt,Integer A_privatekey,Integer B_publickey);
    pair<Integer,Integer> Pre_ReKeyGen(Integer A_privatekey,Integer B_publickey);
    Capsule Pre_ReEncryption(Integer rk,Capsule cap);
    Integer Pre_ReCreateKey(Integer B_privatekey,Integer B_publickey,Integer A_publickey,Capsule cap,Integer X);
    vector<string> Pre_Decryption(Integer privatekey,vector<string> cipher_txt);
    ElGamal_keys ElGamal_key_generation();
    void ElGamal_Encryption(string &plain_txt,string &cipher_txt,Integer publickey);
    void ElGamal_Decryption(string &cipher_txt,string &decryption_txt,Integer privatekey);
    Proxy_ReEncryption(){
        g.Initialize(prng,200);
        m = g.GetGroupOrder() + Integer::One();
        gen = g.GetGenerator();
    }
};

class SHE
{
    private:
        Integer P;
        Integer L;
    public:
        void setup(int a=512,int b=250,int c=50);
        Integer N;
        int k0,k1,k2;
        pair<Integer,Integer> Key_Generation();
        Integer Encryption(Integer plain_txt,pair<Integer,Integer> sk,Integer N);
        Integer Decryption(pair<Integer,Integer> sk,Integer cipher_txt);
};

void Phi_Enc(Phi a,vector<string> &cipher_txt,Proxy_ReEncryption &Pre_ReEnc,Integer publickey);

void Elgamal_Enc(vector<pair<double,string>> &qurey,Proxy_ReEncryption &Pre_ReEnc,vector<pair<double,string>> &cipher_txt,Integer publickey);

void Phi_Dec(vector<string> cipher_txt,Proxy_ReEncryption &Pre_ReEnc,vector<string> &phi_dec,Integer privatekey);

void Elgamal_Dec(vector<pair<double,string>> &cipher_txt,Proxy_ReEncryption &Pre_ReEnc,vector<pair<double,string>> &dec_txt,Integer privatekey);

void database_encode(vector<vector<Point>> &trajectionList,Phi phi,vector<vector<pair<double,string>>> &encodingList);

void decode(vector<vector<Point>> &trajectionList,Phi phi,vector<vector<pair<double,string>>> &encodingList);

void query_encode(vector<Point> &trajectionList,Phi phi,vector<pair<double,string>> &encodingList);

Integer to_Integer(string a);

bool mycmp1(pair<double,string> a,pair<double,string> b);

int D_H(string &a,string &b,map<string,vector<string>> &dict);

double dist(pair<double,string> &a,vector<pair<double,string>> b,map<string,vector<string>> &dict);

Integer dist1(pair<double,string> &a,vector<pair<double,string>> b,map<string,vector<string>> &dict);

vector<pair<double,string>> merge(vector<pair<double,string>> &a,vector<pair<double,string>> &b);

double DSED(vector<pair<double,string>> &a,vector<pair<double,string>> &b,map<string,vector<string>> &dict);

pair<Integer,double> DSED1(vector<pair<double,string>> &a,vector<pair<double,string>> &b,map<string,vector<string>> &dict);

void printstring(const string& str, ostream& out=cout);

string Integer_to_string(Integer x);

double Integer_to_double(Integer x);

void rot(uint n, uint &x, uint &y, uint rx, uint ry);

uint xy2d(uint n, uint x, uint y);

void read_data(string file_name,vector<vector<Point>> &trajectionList);

void vacuate(vector<vector<Point>> &trajectionList);

vector<Phi>  hilbertcurve_generator(uint order,int num);

void mapping_table(vector<vector<Point>> &trajectionList,vector<Phi> phi_list,map<string,vector<string>> &dict,SHE &she,pair<Integer,Integer> sk);

void top_K(const int k,vector<double> &distlist,vector<pair<double,int>> &K);

void compute_dist(vector<double> &distlist,vector<vector<pair<double,string>>> &encodingList,map<string,vector<string>> &dict,vector<pair<double,string>> &encoding_qu);

void compute_dist1(vector<pair<Integer,double>> distlist1,vector<pair<double,int>> &K,SHE &she,pair<Integer,Integer> sk,vector<vector<pair<double,string>>> &encondinglist_K,vector<vector<pair<double,string>>> &encodingList,map<string,vector<string>> &dict,vector<pair<double,string>> &encoding_qu);

vector<Integer> compute_ESD(vector<pair<Integer,double>> &distlist1);

vector<double> compute_D(SHE &she,pair<Integer,Integer> sk,vector<pair<Integer,double>> &distlist1,vector<Integer> &ESD);

void trajection_and_ID(vector<vector<pair<double,string>>> &encodingList,pair<vector<string>,Capsule> &mess,vector<pair<double,int>> K,vector<string> &kid,vector<vector<pair<double,string>>> &result_trajection,vector<pair<double,int>> &k);