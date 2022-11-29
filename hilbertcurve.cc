#include <bits/stdc++.h>
#include <gmpxx.h>
using namespace std;
///////////////////////////////////////////////////////////////////////////
// https://en.wikipedia.org/wiki/Hilbert_curve
// n must be power of 2.
// rotate/flip a quadrant appropriately
void rot(uint n, uint &x, uint &y, uint rx, uint ry)
{
    if (ry == 0)
    {
        if (rx == 1)
        {
            x = n - 1 - x;
            y = n - 1 - y;
        }
        swap(x,y);
    }
}
// convert (x,y) to d
uint xy2d(uint n, uint x, uint y)
{
    uint rx, ry, s, d = 0;
    for (s = n / 2; s > 0; s /= 2)
    {
        rx = (x & s) > 0;
        ry = (y & s) > 0;
        d += s * s * ((3 * rx) ^ ry);
        rot(n, x, y, rx, ry);
    }
    return d;
}

// convert d to (x,y)
void d2xy(uint n, uint d, uint x, uint y)
{
    uint rx, ry, s, t = d;
    x = y = 0;
    for (s = 1; s < n; s *= 2)
    {
        rx = 1 & (t / 2);
        ry = 1 & (t ^ rx);
        rot(s, x, y, rx, ry);
        x += s * rx;
        y += s * ry;
        t /= 4;
    }
}

struct hashpair
{
    template<typename T1,typename T2>
    size_t operator()(const pair<T1,T2>& p) const
    {
        auto hash1 = hash<T1>{}(p.first);
        auto hash2 = hash<T2>{}(p.second);
        if (hash1 != hash2) {
            return hash1 ^ hash2;             
        }
        return hash1;
    }
};

class hilbertcurve
{
public:
    // parameters for the curve
    pair<uint,uint> p0;     // start point
    char d;                 // direction 'u,d,l,r'
    uint N;                 // order of the curve, N=2^k
    double gamma;           // scale factor
    
    unordered_map<pair<uint,uint>, string, hashpair> xy2H;
    unordered_map<string,pair<uint,uint>> H2xy;
    hilbertcurve(uint _N, pair<uint,uint> _p0, char _d, double _gamma): N(_N), p0(_p0), d(_d), gamma(_gamma)
    {
        while(_N>2) {_N>>=1;assert((_N&1u)==0);}
        assert(_p0.first==0||_p0.first==N-1);
        assert(_p0.second==0||_p0.second==N-1);
        assert(_d=='u'||_d=='d'||_d=='l'||_d=='r');
        assert(_p0.first==0&&_d!='l'||_p0.first==N-1&&_d!='r');
        assert(_p0.second==0&&_d!='d'||_p0.second==N-1&&_d!='u');
        // build standard hilbert curve: p0=(0,0); d=u when N%2==1; d=r when N%2==0
        char dd=(N%2==0?'r':'u');
        // xy2d(N,x,y)=H
        }
    }
};
#ifdef __HILBERTCURVE_TEST__
int main()
{
    cout<<xy2d(2,0,1)<<endl;
    hilbertcurve(64,{0,0},'u',1.0);
}
#endif