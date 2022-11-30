// g++ fun++.cpp -g  -lgmpxx -lgmp -o fun++ 
// utf-8
//
#include<stdio.h>
#include<iostream>
#include <gmpxx.h>
#include <assert.h>
using namespace std;
typedef struct Publickey{
    mpz_class  n;
    mp_bitcnt_t k0;
    mp_bitcnt_t k1;
    mp_bitcnt_t k2;
    mpz_class en1;
    mpz_class en0;
    mpz_class en2;
} Publickey;
typedef struct Privatekey{
    mpz_class  p;
    mpz_class  L;
} Privatekey;
mpz_class get_prime(mp_bitcnt_t n)
{
    mpz_class ran;
    gmp_randclass r1(gmp_randinit_mt);
    //r1.seed((unsigned long)time(0));
    r1.seed(123);
    ran =r1.get_z_bits(n);
    while(!mpz_probab_prime_p(ran.get_mpz_t(),20)){
        ran++;
    }
    return  ran;  //gmp_printf("c= %Zd\n",ran);
}
mpz_class get_rand(mp_bitcnt_t n)
{
    gmp_randclass r1(gmp_randinit_mt);
    r1.seed(123);
    return r1.get_z_bits(n);
}

class SHE{
private:
    Publickey pub;
    Privatekey pri;
public:
    SHE(mp_bitcnt_t k0, mp_bitcnt_t k1, mp_bitcnt_t k2);
    void SHE_ency(mpz_class plantext , mpz_class & enc);
    void SHE_dec(mpz_class & message , mpz_class  enc);
    const Privatekey & GetPriveKey();
    const Publickey  & GetPublicKey();
    void Generate_EncPublic();
};
void SHE::Generate_EncPublic()
{     
    mpz_class enc0,enc1,enc2;
    /*memcpy(&enc_pk,&pub,sizeof(Publickey));*/
    this->SHE_ency(0,enc0);
    this->SHE_ency(1,enc1);
    this->SHE_ency(-1,enc2);
    pub.en0  = enc0;
    pub.en1  = enc1;
    pub.en2 = enc2;
}
SHE::SHE(mp_bitcnt_t k0, mp_bitcnt_t k1, mp_bitcnt_t k2){

    mpz_class p,q;    
    gmp_randstate_t state;
    assert( (k1 < k2) && (k2 < k0) );
    pub.k0 = k0;
    pub.k1 = k1;
    pub.k2 = k2;
    p = get_prime(k0);
    q = get_prime(k0);
    pub.n = p * q;
    pri.p = p;
    pri.L = get_rand(k2);
}
void SHE::SHE_ency(mpz_class plantext , mpz_class & enc){
    mpz_class r1, r2, m;
    mpz_class tmp_a, tmp_b;
    signed long bits;
    mpz_get_d_2exp(&bits,plantext.get_mpz_t());
    assert (bits <= pub.k1); // 检测 bit 
    r1 =get_rand(pub.k2);
    r2 = get_rand(pub.k0);
    m = plantext;
    tmp_a = (r1*pri.L + m) % pub.n;
    tmp_b = (r2*pri.p + 1) % pub.n;
    enc = (tmp_a * tmp_b) % pub.n;
}
void SHE::SHE_dec(mpz_class & m ,mpz_class enc){
    m  = enc % pri.p;
    m  %= pri.L ; 
    if(m > pri.L /2 )
        m -= pri.L;
}
const Privatekey & SHE::GetPriveKey(){
    return pri;
}
const Publickey & SHE::GetPublicKey(){
    return pub;
}
// 公钥加法
void SHE_encry_pub(mpz_class plantext, mpz_class & enc ,const Publickey & pub)
{
    mpz_class tmp,r;
    signed long bits;
    mpz_get_d_2exp(&bits,plantext.get_mpz_t());
    assert (bits <= pub.k1);
    r = get_rand(pub.k2);
    tmp = (r * pub.en0 ) % pub.n;
    if(plantext <0)
        enc = (-plantext * pub.en2) % pub.n;
    else
        enc = (plantext *  pub.en1 ) % pub.n;
    enc = (enc+ tmp) % pub.n;
}

int main()
{
    mpz_class a;
    mp_bitcnt_t k0;
    SHE myshe = SHE(500,16,100);
    gmp_printf("privatekey.L is %Zd\n",myshe.GetPriveKey().L);
    gmp_printf("privatekey.p is %Zd\n",myshe.GetPriveKey().p);
    gmp_printf("publickey.n  is %Zd\n",myshe.GetPublicKey().n);
    mpz_class enc;
    mpz_class mm =1234;
    mpz_class message;
    myshe.SHE_ency(mm,enc);
    cout << "origal messgal is : " <<mm<<endl;
    cout << "ecnry test is " <<enc<< endl;
    myshe.SHE_dec(message,enc);
    cout << "decry test is " << message <<endl;
}


