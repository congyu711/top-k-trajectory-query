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


// struct hashpair
// {
//     template<typename T1,typename T2>
//     size_t operator()(const pair<T1,T2>& p) const
//     {
//         auto hash1 = hash<T1>{}(p.first);
//         auto hash2 = hash<T2>{}(p.second);
//         if (hash1 != hash2) {
//             return hash1 ^ hash2;             
//         }
//         return hash1;
//     }
// };

class hilbertcurve
{
public:
    // parameters for the curve
    pair<uint,uint> p0;     // start point
    char d;                 // direction 'u,d,l,r'
    uint N;                 // order of the curve, N=2^k
    double gamma;           // scale factor
    
    // unordered_map<pair<uint,uint>, string, hashpair> xy2H;
    vector<vector<string>> xy2H;
    unordered_map<string,pair<uint,uint>> H2xy;
    hilbertcurve(uint _N, pair<uint,uint> _p0, char _d, double _gamma): N(_N), p0(_p0), d(_d), gamma(_gamma)
    {
        while(_N>2) {_N>>=1;assert((_N&1u)==0);}
        assert(_p0.first==0||_p0.first==N-1);
        assert(_p0.second==0||_p0.second==N-1);
        assert(_d=='u'||_d=='d'||_d=='l'||_d=='r');
        assert(_p0.first==0&&_d!='l'||_p0.first==N-1&&_d!='r');
        assert(_p0.second==0&&_d!='d'||_p0.second==N-1&&_d!='u');
        xy2H.resize(N,vector<string>(N));
        // build standard hilbert curve: p0=(0,0); d=u when N%2==1; d=r when N%2==0
        char dd=(N%2==0?'r':'u');
        // xy2d(N,x,y)=H
        if(p0==make_pair(0u,0u)&&dd==d)   // same curve
        {
            for(uint i=0;i<N;i++)
            {
                for(uint j=0;j<N;j++)
                {
                    xy2H[i][j]=to_string(xy2d(N,i,j));
                    H2xy[to_string(xy2d(N,i,j))]=make_pair(i,j);
                }
            }
        }
        else if(p0==make_pair(0u,0u)&&dd!=d)  // same startpoint but different direction: reflection in the line y=x
        {
            for(uint i=0;i<N;i++)
            {
                for(uint j=0;j<N;j++)
                {
                    xy2H[i][j]=to_string(xy2d(N,j,i));
                    H2xy[to_string(xy2d(N,j,i))]=make_pair(i,j);
                }
            }
        }
        else if((p0==make_pair(0u,N-1u)&&d=='d'&&dd=='u')||(p0==make_pair(N-1u,0u)&&d=='l'&&dd=='r'))   // just reverse the curve.
        {
            for(uint i=0;i<N;i++)
            {
                for(uint j=0;j<N;j++)
                {
                    xy2H[i][j]=to_string(N*N-1-xy2d(N,i,j));
                    H2xy[to_string(N*N-1-xy2d(N,i,j))]=make_pair(i,j);
                }
            }
        }
        else if(p0==make_pair(0u,N-1u)&&(dd=='u'&&d=='r'||dd=='r'&&d=='d'))   // rotate 90 degrees clockwise
        {
            // y'=N-1-x, x'=y
            for(uint i=0;i<N;i++)
            {
                for(uint j=0;j<N;j++)
                {
                    xy2H[i][j]=to_string(xy2d(N,N-1-j,i));
                    H2xy[to_string(xy2d(N,N-1-j,i))]=make_pair(i,j);
                }
            }
        }
        else if(p0==make_pair(N-1u,0u)&&(dd=='u'&&d=='l'||dd=='r'&&d=='u'))   // rotate 90 degrees counter clockwise
        {
            // y'=x,x'=N-1-y
            for(uint i=0;i<N;i++)
            {
                for(uint j=0;j<N;j++)
                {
                    xy2H[i][j]=to_string(xy2d(N,j,N-1-i));
                    H2xy[to_string(xy2d(N,j,N-1-i))]=make_pair(i,j);
                }
            }
        }
        else if(p0==make_pair(0u,N-1u)&&dd=='r'&&d=='r')    // filp y
        {
            for(uint i=0;i<N;i++)
            {
                for(uint j=0;j<N;j++)
                {
                    xy2H[i][j]=to_string(xy2d(N,i,N-1-j));
                    H2xy[to_string(xy2d(N,i,N-1-j))]=make_pair(i,j);
                }
            }
        }
        else if(p0==make_pair(N-1u,0u)&&dd=='u'&&d=='u')    // flip x
        {
            for(uint i=0;i<N;i++)
            {
                for(uint j=0;j<N;j++)
                {
                    xy2H[i][j]=to_string(xy2d(N,N-1-i,j));
                    H2xy[to_string(xy2d(N,N-1-i,j))]=make_pair(i,j);
                }
            }
        }
        else if(p0==make_pair(N-1u,N-1u)&&(dd=='u'&&d=='l'||dd=='r'&&d=='d'))   // reflection in the line y=N-1-x
        {
            for(uint i=0;i<N;i++)
            {
                for(uint j=0;j<N;j++)
                {
                    xy2H[i][j]=to_string(xy2d(N,N-1-j,N-1-i));
                    H2xy[to_string(xy2d(N,N-1-j,N-1-i))]=make_pair(i,j);
                }
            }
        }
        else    //rotate 180
        {
            for(uint i=0;i<N;i++)
            {
                for(uint j=0;j<N;j++)
                {
                    xy2H[i][j]=to_string(xy2d(N,N-1-i,N-1-j));
                    H2xy[to_string(xy2d(N,N-1-i,N-1-j))]=make_pair(i,j);
                }
            }
        }
    }
};
#ifdef __HILBERTCURVE_TEST__
int main()
{
    // cout<<xy2d(2,0,1)<<endl;
    hilbertcurve hc(64,{0,0},'u',1.0);
    for(int i=0;i<hc.N;i++)
    {
        for(int j=0;j<hc.N;j++)
        {
            cout<<setw(5)<<hc.xy2H[i][j];
        }
        cout<<endl;
    }
}
#endif