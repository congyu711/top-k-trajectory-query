#include <bits/stdc++.h>
#include <gmpxx.h>
using namespace std;

// template<class Point>
// bool mycmp(Point a,Point b)
// {
//     return a.time<=b.time;
// }

bool mycmp1(pair<double,string> a,pair<double,string> b)
{
    return a.first<=b.first;
}

// template<class Point>
// double dist(Point a,vector<Point> &b)
// {
//     double res 
//     auto p = lower_bound(b.begin(),b.end(),a,mycmp<Point>);
//     if(p != b.end())
//     {
//         auto q = p-1;
//         double w1 = ((*p).time-a.time)/((*p).time-(*q).time);
//         double w2 = 1-w1;
//         double bx = w1*(*p).x + w2*(*q).x;
//         double by = w1*(*p).y + w2*(*q).y;
//         res = sqrt(pow(a.x-bx,2)+pow(a.y-by,2));
//     }
//     else
//     {
//         res = 0.0;
//     }
//     return res;
// }

int D_H(string &a,string &b,map<string,vector<string>> &dict)
{
    vector<string> tmp1 = dict[a];
    vector<string> tmp2 = dict[b];
    vector<int> res;
    res.push_back(abs(atoi(a.c_str())-atoi(b.c_str())));
    for(int i=0;i<tmp1.size()-2;i++)
    {
        res.push_back(abs(atoi(tmp1[i].c_str())-atoi(tmp2[i].c_str())));
    }
    return *min_element(res.begin(),res.end());
}

double dist(pair<double,string> &a,vector<pair<double,string>> b,map<string,vector<string>> &dict)
{
    double tmp;
    auto p = lower_bound(b.begin(),b.end(),a,mycmp1);
    if(p!=b.end())
    {
        auto q = p-1;
        double w1 = ((*p).first-a.first)/((*p).first-(*q).first);
        double w2 = 1-w1;
        tmp = w1*D_H(a.second,(*q).second,dict)+w2*D_H(a.second,(*p).second,dict)-w1*w2*D_H((*p).second,(*q).second,dict);
    }
    return tmp;
}

mpz_class dist1(pair<double,string> &a,vector<pair<double,string>> b,map<string,vector<string>> &dict)
{
    mpz_class tmp;
    auto p = lower_bound(b.begin(),b.end(),a,mycmp1);
    if(p!=b.end())
    {
        auto q = p-1;
        mpz_class ay(dict[a.second].back());
        mpz_class b1y(dict[(*q).second].back());
        mpz_class b2y(dict[(*p).second].back());
        mpz_class ax(*(dict[a.second].end()-2));
        mpz_class b1x(*(dict[(*q).second].end()-2));
        mpz_class b2x(*(dict[(*p).second].end()-2));
        mpz_class w1(((*p).first-a.first)/((*p).first-(*q).first));
        mpz_class w2((a.first-(*q).first)/((*p).first-(*q).first));
        mpz_class bx = w1*b1x + w2*b2x;
        mpz_class by = w1*b1y + w2*b2y;
        tmp = (ax-bx)*(ax-bx)+(ay-by)*(ay-by);
    }
    return tmp;
}

vector<pair<double,string>> merge(vector<pair<double,string>> &a,vector<pair<double,string>> &b)
{
    vector<pair<double,string>> c;
    auto p=a.begin();
    auto q=b.begin();
    for(;p!=a.end()||q!=b.end();)
    {
        if(q!=b.end()&&p!=a.end())
        {
            if((*p).first<(*q).first)
            {
                c.push_back(*p);
                p++;
            }
            else if((*p).first>(*q).first)
            {
                c.push_back(*q);
                q++;
            }
            else{
                c.push_back(*p);
                p++;
                q++;
            }
        }
        else if(p==a.end())
        {
            c.push_back(*q);
            q++;
        }
        else 
        {
            c.push_back(*p);
            p++;
            q++;
        }
    }
    return c;
}

double DSED(vector<pair<double,string>> &a,vector<pair<double,string>> &b,map<string,vector<string>> &dict)
{
    vector<pair<double,string>> c = merge(a,b);
    // cout<<c.size()<<"\n";
    vector<double> d;
    if(c.size()<3)
    {
        cerr<<"num of Point";
        return -1;
    }
    for(int i=1;i<c.size()-1;i++)
    {   
        auto flag = find(a.begin(),a.end(),c[i]);
        if(flag != a.end())
        {
            d.push_back(dist(c[i],b,dict));
        }
        else
        {
            d.push_back(dist(c[i],a,dict));
        }

    }
    double dsed = d[0]*(c[2].first-c[1].first);
    if(d.size()>=2)
    {
        for(int i=1;i<d.size()-1;i++)
        {
            dsed += d[i]*(c[i+2].first-c[i].first);
        }
        dsed += d.back()*(c[d.size()].first-c[d.size()-1].first);
        dsed = dsed/(2*(c[d.size()].first-c[1].first));
    }
    else 
    {
        return dsed;
    }
    return dsed;
}

pair<mpz_class,double> DSED1(vector<pair<double,string>> &a,vector<pair<double,string>> &b,map<string,vector<string>> &dict)
{
    vector<pair<double,string>> c = merge(a,b);
    // cout<<c.size()<<"\n";
    vector<mpz_class> d;
    double time;
    if(c.size()<3)
    {
        cerr<<"num of Point";
        return make_pair(-1,-1);
    }
    for(int i=1;i<c.size()-1;i++)
    {   
        auto flag = find(a.begin(),a.end(),c[i]);
        if(flag != a.end())
        {
            d.push_back(dist1(c[i],b,dict));
        }
        else
        {
            d.push_back(dist1(c[i],a,dict));
        }

    }
    mpz_class dsed = d[0]*(c[2].first-c[1].first);
    if(d.size()>=2)
    {
        for(int i=1;i<d.size()-1;i++)
        {
            dsed += d[i]*(c[i+2].first-c[i].first);
        }
        dsed += d.back()*(c[d.size()].first-c[d.size()-1].first);        
        time = 2*(c[d.size()].first-c[1].first);
    }
    else 
    {
        time = 1.0;
        return make_pair(dsed,time);
    }
    return make_pair(dsed,time);
}