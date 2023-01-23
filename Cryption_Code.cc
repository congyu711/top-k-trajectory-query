#include <bits/stdc++.h>
#include "hilbertcurve.cc"
#include "function_declaration.h"
using namespace std;
using namespace CryptoPP;

void Phi_Enc(Phi a,vector<string> &cipher_txt,Proxy_ReEncryption &Pre_ReEnc,Integer publickey)
{
    string tmp;
    string order = to_string(a.order);
    string origin1 = to_string(a.origin.first);
    string origin2 = to_string(a.origin.second);
    string gamma = to_string(a.gamma);
    string direction = &(a.direcion);
    Pre_ReEnc.ElGamal_Encryption(order,tmp,publickey);
    cipher_txt.push_back(tmp);
    tmp.clear();
    Pre_ReEnc.ElGamal_Encryption(origin1,tmp,publickey);
    cipher_txt.push_back(tmp);
    tmp.clear();
    Pre_ReEnc.ElGamal_Encryption(origin2,tmp,publickey);
    cipher_txt.push_back(tmp);
    tmp.clear();
    Pre_ReEnc.ElGamal_Encryption(direction,tmp,publickey);
    cipher_txt.push_back(tmp);
    tmp.clear();
    Pre_ReEnc.ElGamal_Encryption(gamma,tmp,publickey);
    cipher_txt.push_back(tmp);
}

void Elgamal_Enc(vector<pair<double,string>> &qurey,Proxy_ReEncryption &Pre_ReEnc,vector<pair<double,string>> &cipher_txt,Integer publickey)
{
    for(auto a:qurey)
    {   
        string tmp;
        Pre_ReEnc.ElGamal_Encryption(a.second,tmp,publickey);
        cipher_txt.push_back(make_pair(a.first,tmp));
    }
}

void Phi_Dec(vector<string> cipher_txt,Proxy_ReEncryption &Pre_ReEnc,vector<string> &phi_dec,Integer privatekey)
{
    string tmp;
    string order = cipher_txt[0];
    string origin1 = cipher_txt[1];
    string origin2 = cipher_txt[2];
    string direction = cipher_txt[3];
    string gamma = cipher_txt[4];
    Pre_ReEnc.ElGamal_Decryption(order,tmp,privatekey);
    phi_dec.push_back(tmp);
    tmp.clear();
    Pre_ReEnc.ElGamal_Decryption(origin1,tmp,privatekey);
    phi_dec.push_back(tmp);
    tmp.clear();
    Pre_ReEnc.ElGamal_Decryption(origin2,tmp,privatekey);
    phi_dec.push_back(tmp);
    tmp.clear();
    Pre_ReEnc.ElGamal_Decryption(direction,tmp,privatekey);
    phi_dec.push_back(tmp);
    tmp.clear();
    Pre_ReEnc.ElGamal_Decryption(gamma,tmp,privatekey);
    phi_dec.push_back(tmp);
}

void Elgamal_Dec(vector<pair<double,string>> &cipher_txt,Proxy_ReEncryption &Pre_ReEnc,vector<pair<double,string>> &dec_txt,Integer privatekey)
{
    for(auto a:cipher_txt)
    {   
        string tmp;
        Pre_ReEnc.ElGamal_Decryption(a.second,tmp,privatekey);
        dec_txt.push_back(make_pair(a.first,tmp));
    }
}

void database_encode(vector<vector<Point>> &trajectionList,Phi phi,vector<vector<pair<double,string>>> &encodingList)
{
    hilbertcurve hc({phi.order,phi.origin,phi.direcion,phi.gamma});
    for(auto a:trajectionList)
    {
        vector<pair<double,string>> tmp;
        for(auto b:a)
        {
            tmp.push_back(make_pair(b.time,hc.xy2H[b.x][b.y]));
        }
        encodingList.push_back(tmp);
    }
}

void decode(vector<vector<Point>> &trajectionList,Phi phi,vector<vector<pair<double,string>>> &encodingList)
{
    hilbertcurve hc({phi.order,phi.origin,phi.direcion,phi.gamma});
    for(auto a:encodingList)
    {
        vector<Point> tmp;
        for(auto b:a)
        {
            pair<uint,uint> x = hc.H2xy[b.second];
            tmp.push_back({b.first,(double) x.first,(double) x.second});
        }
        trajectionList.push_back(tmp);
    }
}

void query_encode(vector<Point> &trajectionList,Phi phi,vector<pair<double,string>> &encodingList)
{
    hilbertcurve hc({phi.order,phi.origin,phi.direcion,phi.gamma});
    for(auto a:trajectionList)
    {
        pair<double,string> tmp;
        tmp = make_pair(a.time,hc.xy2H[a.x][a.y]);
        encodingList.push_back(tmp);
    }
}