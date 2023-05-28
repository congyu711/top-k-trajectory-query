#ifndef __DSED
#define __DSED
#include <vector>

#include <crypto++/integer.h>
#include "base.h"
using namespace CryptoPP;
using namespace std;

bool mycmp1(const pair<double, string> &a, const pair<double, string> &b)
{
    return a.first <= b.first;
}

int D_H(const string &a, const string &b, map<string, vector<string>> &dict)
{
    vector<string> tmp1 = dict[a];
    vector<string> tmp2 = dict[b];
    vector<int> res;
    res.push_back(abs(atoi(a.c_str()) - atoi(b.c_str())));
    for (int i = 0; i < tmp1.size() - 2; i++)
    {
        res.push_back(abs(atoi(tmp1[i].c_str()) - atoi(tmp2[i].c_str())));
    }
    return *min_element(res.begin(), res.end());
}

double dist(const pair<double, string> &a, const vector<pair<double, string>>& b, map<string, vector<string>> &dict)
{
    double tmp=1e9;
    auto p = lower_bound(b.begin(), b.end(), a, mycmp1);
    if (p != b.end())
    {
        if(p==b.begin()) p++;
        auto q = p - 1;
        double w1 = ((*p).first - a.first) / ((*p).first - (*q).first);
        double w2 = 1 - w1;
        tmp = w1 * D_H(a.second, (*q).second, dict) + w2 * D_H(a.second, (*p).second, dict) - w1 * w2 * D_H((*p).second, (*q).second, dict);
    }
    return tmp;
}

Integer dist1(const pair<double, string> &a, const vector<pair<double, string>>& b, map<string, vector<string>> &dict)
{
    Integer tmp=Integer("1145141919810");
    auto p = lower_bound(b.begin(), b.end(), a, mycmp1);
    if (p != b.end())
    {
        if(p==b.begin()) p++;
        auto q = p - 1;
        Integer ay = to_Integer(dict[a.second].back());
        Integer b1y = to_Integer(dict[(*q).second].back());
        Integer b2y = to_Integer(dict[(*p).second].back());
        Integer ax = to_Integer(*(dict[a.second].end() - 2));
        Integer b1x = to_Integer(*(dict[(*q).second].end() - 2));
        Integer b2x = to_Integer(*(dict[(*p).second].end() - 2));
        Integer w1(((*p).first - a.first) / ((*p).first - (*q).first));
        Integer w2((a.first - (*q).first) / ((*p).first - (*q).first));
        Integer bx = w1 * b1x + w2 * b2x;
        Integer by = w1 * b1y + w2 * b2y;
        tmp = (ax - bx) * (ax - bx) + (ay - by) * (ay - by);
    }
    return tmp;
}

vector<pair<double, string>> merge(const vector<pair<double, string>> &a, const vector<pair<double, string>> &b)
{
    vector<pair<double, string>> c;
    auto p = a.begin();
    auto q = b.begin();
    for (; p != a.end() || q != b.end();)
    {
        if (q != b.end() && p != a.end())
        {
            if ((*p).first < (*q).first)
            {
                c.push_back(*p);
                p++;
            }
            else if ((*p).first > (*q).first)
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
        else if (p == a.end())
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

double DSED(const vector<pair<double, string>> &a, const vector<pair<double, string>> &b, map<string, vector<string>> &dict)
{
    vector<pair<double, string>> c = merge(a, b);
    // cout<<c.size()<<"\n";
    vector<double> d;
    if (c.size() < 3)
    {
        cerr << "num of Point";
        return -1;
    }
    for (int i = 1; i < c.size() - 1; i++)
    {
        auto flag = find(a.begin(), a.end(), c[i]);
        if (flag != a.end())
        {
            d.push_back(dist(c[i], b, dict));
        }
        else
        {
            d.push_back(dist(c[i], a, dict));
        }
    }
    double dsed = d[0] * (c[2].first - c[1].first);
    if (d.size() >= 2)
    {
        for (int i = 1; i < d.size() - 1; i++)
        {
            dsed += d[i] * (c[i + 2].first - c[i].first);
        }
        dsed += d.back() * (c[d.size()].first - c[d.size() - 1].first);
        dsed = dsed / (2 * (c[d.size()].first - c[1].first));
    }
    else
    {
        return dsed;
    }
    return dsed;
}

pair<Integer, double> DSED1(const vector<pair<double, string>> &a, const vector<pair<double, string>> &b, map<string, vector<string>> &dict)
{
    vector<pair<double, string>> c = merge(a, b);
    // cout<<c.size()<<"\n";
    vector<Integer> d;
    double time;
    if (c.size() < 3)
    {
        cerr << "num of Point";
        return make_pair(-1, -1);
    }
    for (int i = 1; i < c.size() - 1; i++)
    {
        auto flag = find(a.begin(), a.end(), c[i]);
        if (flag != a.end())
        {
            d.push_back(dist1(c[i], b, dict));
        }
        else
        {
            d.push_back(dist1(c[i], a, dict));
        }
    }
    Integer dsed = d[0] * (c[2].first - c[1].first);
    if (d.size() >= 2)
    {
        for (int i = 1; i < d.size() - 1; i++)
        {
            dsed += d[i] * (c[i + 2].first - c[i].first);
        }
        dsed += d.back() * (c[d.size()].first - c[d.size() - 1].first);
        time = 2 * (c[d.size()].first - c[1].first);
    }
    else
    {
        time = 1.0;
        return make_pair(dsed, time);
    }
    return make_pair(dsed, time);
}
#endif