#ifndef __SORT
#define __SORT
#include <queue>
#include <crypto++/integer.h>
#include "SHE_Encryption.cc"
#include "Proxy_ReEncryption.cc"
#include "DSED.cc"
using namespace std;
using namespace CryptoPP;
#define random(x) (rand()%x)

void top_K(const int k,vector<double> &distlist,vector<pair<double,int>> &K)
{
    using pairdi=pair<double,int>;
    auto cmp=[](const pairdi&a,const pairdi&b){return a.first<b.first;};
    priority_queue<pairdi,vector<pairdi>,decltype(cmp)> top_K(cmp);
    if(distlist.size()<=k)
    {
        for(int i=0;i<distlist.size();i++)
        {
            top_K.push(make_pair(distlist[i],i));
        }
    }  // return....
    else
    {
        int i=0;
        for(;i<k;i++)
        {
            top_K.push(make_pair(distlist[i],i));
        }
        for(;i<distlist.size();i++)
        {
            if(top_K.top().first>distlist[i])
            {
                top_K.pop();
                top_K.push(make_pair(distlist[i],i));
            }
        }
    }
    while(top_K.size()){
        auto c=top_K.top(); 
        K.push_back(c);
        top_K.pop();
    }
}

void compute_dist(vector<double> &distlist,vector<vector<pair<double,string>>> &encodingList,map<string,vector<string>> &dict,vector<pair<double,string>> &encoding_qu)
{
    vector<double> tmp;
    for(int i=0;i<encodingList.size();i++)
    {
        tmp.push_back(DSED(encoding_qu,encodingList[i],dict));
    }
    distlist = tmp;
}

void compute_dist1(vector<pair<Integer,double>> &distlist1,vector<pair<double,int>> &K,vector<vector<pair<double,string>>> &encondinglist_K,vector<vector<pair<double,string>>> &encodingList,map<string,vector<string>> &dict,vector<pair<double,string>> &encoding_qu)
{   
    vector<vector<pair<double,string>>> tmp;
    for(auto x:K)
    {
        tmp.push_back(encodingList[x.second]);
    }
    encondinglist_K = tmp;
    vector<pair<Integer,double>> tmp1;
    for(int i=0;i<encondinglist_K.size();i++)
    {
        tmp1.push_back(DSED1(encoding_qu,encondinglist_K[i],dict));
    }
    distlist1 = tmp1;
}

vector<Integer> compute_ESD(vector<pair<Integer,double>> &distlist1)
{

    int ra = random(10);
    int rb = random(10)-5;
    vector<Integer> ESD;
    for(auto x:distlist1)
    {
        ESD.push_back(ra*x.first+rb*x.second);
    }
    return ESD;
}

vector<double> compute_D(SHE &she,pair<Integer,Integer> sk,vector<double> &L,vector<Integer> &ESD)
{
    vector<Integer> SD;
    for(auto x:ESD)
    {
        Integer tmp = she.Decryption(sk,x);
        SD.push_back(tmp);
    }
    vector<double> D;
    for(int i=0;i<SD.size();i++)
    {
        D.push_back(Integer_to_double(SD[i])/L[i]);
    }
    return D;
}

void trajection_and_ID(vector<vector<pair<double,string>>> &encodingList,pair<vector<string>,Capsule> &mess,vector<pair<double,int>> &K,vector<string> &kid,vector<vector<pair<double,string>>> &result_trajection,vector<pair<double,int>> &k)
{
    vector<std::string> tmp;
    vector<vector<pair<double,string>>> tmp1;
    for(auto x:k)
    {
        tmp.push_back(mess.first[K[x.second].second]);
        cout<<1;
        tmp1.push_back(encodingList[K[x.second].second]);
        cout<<2;
    }
    kid = tmp;
    result_trajection = tmp1;
}
#endif