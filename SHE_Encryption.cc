#ifndef __SHE
#define __SHE

#include <iostream>
#include <string>
#include <iomanip>
#include <crypto++/integer.h>
#include <crypto++/osrng.h>
#include <crypto++/nbtheory.h>
using namespace std;
using namespace CryptoPP;

class SHE
{
    public:
        SHE(int _k0=50,int _k1=16,int _k2=100):k0(_k0),k1(_k1),k2(_k2){}
        Integer N;
        AutoSeededRandomPool prng;
        int k0,k1,k2;
        pair<Integer,Integer> Key_Generation();
        Integer Encryption(Integer plain_txt,pair<Integer,Integer> sk,Integer N);
        Integer Decryption(pair<Integer,Integer> sk,Integer cipher_txt);
};


pair<Integer,Integer> SHE::Key_Generation(){
    PrimeAndGenerator pg;
    Integer p;
    Integer q;
    Integer L;
    pg.Generate(1,prng,k0,k0-1);
    p = pg.Prime();
    pg.Generate(1,prng,k0,k0-1);
    q = pg.Prime();
    L = Integer(prng,k2);
    N = p*q;
    // cout << hex << p<<endl<<q<<endl<<L;
    return make_pair(p,L);
}

Integer SHE::Encryption(Integer plain_txt,pair<Integer,Integer> sk,Integer N){
    Integer r = Integer(prng,k2);
    Integer r1 = Integer(prng,k0);
    Integer Em = ((plain_txt+r*sk.second)*(Integer::One()+r1*sk.first))%N;
    return Em;
}

Integer SHE::Decryption(pair<Integer,Integer> sk,Integer cipher_txt){
    Integer Dm = (cipher_txt%sk.first)%sk.second;
    return Dm;
}

#ifdef __SHE_encrypt__
int main()
{
    SHE a(500,16,100);
    pair<Integer,Integer> sk = a.Key_Generation();
    cout<<sk.first<<endl<<sk.second;
    Integer m("521");
    Integer Em = a.Encryption(m,sk,a.N);
    cout<<Em<<endl;
    Integer Dm = a.Decryption(sk,Em+Integer::Two());
    cout<<dec<<Dm<<endl;
}
#endif
#endif