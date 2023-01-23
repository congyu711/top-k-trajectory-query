#include <crypto++/integer.h>
#include <iostream>
#include <string>
#include <iomanip>
#include "function_declaration.h"
using namespace std;
using namespace CryptoPP;

//outpt the cipher_txt in hex
void printstring(const string& str, ostream& out=cout)
{
    string o;
    for(int i=0;i<str.length();i++)
    {
        stringstream ss;
        string a;
        int num=0;
        uint tmp=str[i];
        for(int j=0;j<8;j++)    {num+=((tmp&0x80)>>7)<<(8-j-1);tmp<<=1;}
        ss << hex << num;
        ss >> a;
        o += a;
    }
    out<<"Cipher_txt: "<<o<<endl;
}

Integer to_Integer(string a){
    Integer b;
    for (auto i = 0;i < a.length()-1;i++){
        b = (Integer(a[i]) - Integer("48")) + b*Integer("10");
    }
    return b;
}

string Integer_to_string(Integer x){
    stringstream ss;
    ss<<x;
    string y(ss.str());
    return y;
}

double Integer_to_double(Integer x)
{
    stringstream ss;
    ss<<x;
    double b;
    ss>>b;
    return b;
}