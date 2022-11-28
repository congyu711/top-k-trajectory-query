#include <bits/stdc++.h>
#include <gmpxx.h>
using namespace std;

///////////////////////////////////////////////////////////////////////////
// https://en.wikipedia.org/wiki/Hilbert_curve
// n must be power of 2.
// rotate/flip a quadrant appropriately
void rot(mpz_class n, mpz_class *x, mpz_class *y, mpz_class rx, mpz_class ry)
{
    if (ry == 0)
    {
        if (rx == 1)
        {
            *x = n - 1 - *x;
            *y = n - 1 - *y;
        }

        // Swap x and y
        mpz_class t = *x;
        *x = *y;
        *y = t;
    }
}
// convert (x,y) to d
mpz_class xy2d(mpz_class n, mpz_class x, mpz_class y)
{
    mpz_class rx, ry, s, d = 0;
    for (s = n / 2; s > 0; s /= 2)
    {
        rx = (x & s) > 0;
        ry = (y & s) > 0;
        d += s * s * ((3 * rx) ^ ry);
        rot(n, &x, &y, rx, ry);
    }
    return d;
}

// convert d to (x,y)
void d2xy(mpz_class n, mpz_class d, mpz_class *x, mpz_class *y)
{
    mpz_class rx, ry, s, t = d;
    *x = *y = 0;
    for (s = 1; s < n; s *= 2)
    {
        rx = 1 & (t / 2);
        ry = 1 & (t ^ rx);
        rot(s, x, y, rx, ry);
        *x += s * rx;
        *y += s * ry;
        t /= 4;
    }
}

#ifdef __HILBERTCURVE_TEST__
int main()
{
    mpz_class x=0,y=1;
    cout<<xy2d(4,x,y)<<endl;
}
#endif