#ifndef __BASE
#define __BASE
#include <crypto++/integer.h>
#include <iostream>
#include <string>
#include <iomanip>
using namespace std;
using namespace CryptoPP;

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

Integer to_Integer(string a){
    Integer b;
    for (auto i = 0;i < a.length()-1;i++){
        b = int(a[i]-'0') + b*10;
    }
    return b;
}

string Integer_to_string(Integer x){
    stringstream ss;
    ss<<x;
    string y(ss.str());
    return y;
}

double Integer_to_double(Integer x)
{
    stringstream ss;
    ss<<x;
    double b;
    ss>>b;
    return b;
}
#endif

// #include <bits/stdc++.h>
// #include <cryptopp/integer.h>
// #include <cryptopp/eccrypto.h>
// #include <cryptopp/elgamal.h>
// #include <cryptopp/osrng.h>
// #include <cryptopp/hex.h>
// #include <cryptopp/oids.h>
// using namespace std;
// using namespace CryptoPP;


// void Phi_Enc(Phi a,vector<string> &cipher_txt,Proxy_ReEncryption &Pre_ReEnc,Integer publickey);

// void Elgamal_Enc(vector<pair<double,string>> &qurey,Proxy_ReEncryption &Pre_ReEnc,vector<pair<double,string>> &cipher_txt,Integer publickey);

// void Phi_Dec(vector<string> cipher_txt,Proxy_ReEncryption &Pre_ReEnc,vector<string> &phi_dec,Integer privatekey);

// void Elgamal_Dec(vector<pair<double,string>> &cipher_txt,Proxy_ReEncryption &Pre_ReEnc,vector<pair<double,string>> &dec_txt,Integer privatekey);

// void database_encode(vector<vector<Point>> &trajectionList,Phi phi,vector<vector<pair<double,string>>> &encodingList);

// void decode(vector<vector<Point>> &trajectionList,Phi phi,vector<vector<pair<double,string>>> &encodingList);

// void query_encode(vector<Point> &trajectionList,Phi phi,vector<pair<double,string>> &encodingList);

// Integer to_Integer(string a);

// bool mycmp1(pair<double,string> a,pair<double,string> b);

// int D_H(string &a,string &b,map<string,vector<string>> &dict);

// double dist(pair<double,string> &a,vector<pair<double,string>> b,map<string,vector<string>> &dict);

// Integer dist1(pair<double,string> &a,vector<pair<double,string>> b,map<string,vector<string>> &dict);

// vector<pair<double,string>> merge(vector<pair<double,string>> &a,vector<pair<double,string>> &b);

// double DSED(vector<pair<double,string>> &a,vector<pair<double,string>> &b,map<string,vector<string>> &dict);

// pair<Integer,double> DSED1(vector<pair<double,string>> &a,vector<pair<double,string>> &b,map<string,vector<string>> &dict);

// void printstring(const string& str, ostream& out=cout);

// string Integer_to_string(Integer x);

// double Integer_to_double(Integer x);

// void rot(uint n, uint &x, uint &y, uint rx, uint ry);

// uint xy2d(uint n, uint x, uint y);

// void read_data(string file_name,vector<vector<Point>> &trajectionList);

// void vacuate(vector<vector<Point>> &trajectionList);

// vector<Phi>  hilbertcurve_generator(uint order,int num);

// void mapping_table(vector<vector<Point>> &trajectionList,vector<Phi> phi_list,map<string,vector<string>> &dict,SHE &she,pair<Integer,Integer> sk);

// void top_K(const int k,vector<double> &distlist,vector<pair<double,int>> &K);

// void compute_dist(vector<double> &distlist,vector<vector<pair<double,string>>> &encodingList,map<string,vector<string>> &dict,vector<pair<double,string>> &encoding_qu);

// void compute_dist1(vector<pair<Integer,double>> distlist1,vector<pair<double,int>> &K,SHE &she,pair<Integer,Integer> sk,vector<vector<pair<double,string>>> &encondinglist_K,vector<vector<pair<double,string>>> &encodingList,map<string,vector<string>> &dict,vector<pair<double,string>> &encoding_qu);

// vector<Integer> compute_ESD(vector<pair<Integer,double>> &distlist1);

// vector<double> compute_D(SHE &she,pair<Integer,Integer> sk,vector<pair<Integer,double>> &distlist1,vector<Integer> &ESD);

// void trajection_and_ID(vector<vector<pair<double,string>>> &encodingList,pair<vector<string>,Capsule> &mess,vector<pair<double,int>> K,vector<string> &kid,vector<vector<pair<double,string>>> &result_trajection,vector<pair<double,int>> &k);