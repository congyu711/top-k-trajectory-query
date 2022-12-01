#include <iostream>
using std::cout;
using std::endl;

#include<cryptopp/integer.h>
#include<cryptopp/cryptlib.h>
#include<cryptopp/osrng.h>
#include<cryptopp/rsa.h>
using CryptoPP::Integer;
int main() {
    ///////////////////////////////////////
    // Pseudo Random Number Generator
    CryptoPP::AutoSeededRandomPool rng;

    ///////////////////////////////////////
    // Generate Parameters
    CryptoPP::InvertibleRSAFunction params;
    params.GenerateRandomWithKeySize(rng, 1000);

    ///////////////////////////////////////
    // Generated Parameters
    const Integer& n = params.GetModulus();
    const Integer& p = params.GetPrime1();
    const Integer& q = params.GetPrime2();
    const Integer& d = params.GetPrivateExponent();
    const Integer& e = params.GetPublicExponent();
    ///////////////////////////////////////
    // Dump
    cout << "RSA Parameters:" << endl;
    cout << " n: " << n << endl;
    cout << " p: " << p << endl;
    cout << " q: " << q << endl;
    cout << " d: " << d << endl;
    cout << " e: " << e << endl;
    cout << "n-pq: " << n-p*q << endl;
    cout << " ed: " << e*d << endl;
    cout << " (n): "<< (p-Integer::One())*(q-Integer::One())<<endl;
    auto phi_n=(p-Integer::One())*(q-Integer::One());
    auto ed1=(e*d)%(p-Integer::One());
    auto ed2 = (e*d)%(q-Integer::One());
    cout << " ed mod (n): " <<  (ed1*ed2)%phi_n<<endl;
    cout << endl;
}