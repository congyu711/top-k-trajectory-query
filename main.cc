#include <bits/stdc++.h>
using namespace std;
#include <gmpxx.h>
#include <cryptopp/integer.h>
#include"Proxy_ReEncryption.cc"
#include"Douglas_Peucker.cc"
#include"SHE_encrypt.cpp"
#include"hilbertcurve.cc"
#include"DSED.cc"

mpz_class DO_pk;
map<string,vector<string>> dict;
Proxy_ReEncryption Pre_ReEnc;
SHE myshe = SHE(500,16,100);

struct Phi
{
    uint order;
    pair<int,int> origin;
    char direcion;
    double gamma;
};

void read_data(string file_name,vector<vector<Point>> &trajectionList)
{   
    trajectionList.clear();
    ifstream fin(file_name); 
    int N;
    fin>>N;
    for(int i=0;i<N;i++)
    {
        int n;
        fin>>n;
        double a,b,c;
        vector<Point> tmp;
        for(int j=0;j<n;j++)
        {
            fin>>a>>b>>c;
            tmp.push_back({a,b,c});
        }
        trajectionList.push_back(tmp);
    }
}

void vacuate(vector<vector<Point>> &trajectionList)
{
    Douglas_Peucker DP;
    for(auto &a:trajectionList)
    {
        vector<Point> resultPointList;
        DP.execute(a,resultPointList,0.5);
        a.swap(resultPointList);
    }
}

pair<string,string> SHE_Enc(int x,int y)
{
    DO_pk = myshe.GetPublicKey().n;
    mpz_class x0(x);
    mpz_class y0(y);
    mpz_class enc1;
    mpz_class enc2;
    myshe.SHE_ency(x0,enc1);
    myshe.SHE_ency(y0,enc2);
    return make_pair(enc1.get_str(),enc2.get_str());
}

void mapping_table(vector<vector<Point>> &trajectionList,vector<Phi> phi_list,map<string,vector<string>> &dict)
{
    vector<hilbertcurve> hc;
    for(int i=0;i<phi_list.size();i++)
    {
        hc.emplace_back(phi_list[i].order,phi_list[i].origin,phi_list[i].direcion,phi_list[i].gamma);
    }
    for(auto a:trajectionList)
    {
        for(auto b:a)
        {
            auto h = hc[0].xy2H[b.x][b.y];
            // if(dict.count(h))
            // {
            //     break;
            // }
            vector<string> a;
            for(int i=1;i<phi_list.size();i++)
            {
                a.push_back(hc[i].xy2H[b.x][b.y]);
            }
            pair<string,string> Enc = SHE_Enc(b.x,b.y);
            a.push_back(Enc.first);
            a.push_back(Enc.second);
            dict.insert(make_pair(h,a));
        }
    }
}

void Elgamal_Enc(Phi a,vector<string> &cipher_txt,Integer publickey)
{
    string tmp;
    string order = to_string(a.order);
    string origin1 = to_string(a.origin.first);
    string origin2 = to_string(a.origin.second);
    string gamma = to_string(a.gamma);
    string direction = &(a.direcion);
    Pre_ReEnc.ElGamal_Encryption(order,tmp,publickey);
    cipher_txt.push_back(tmp);
    tmp.clear();
    Pre_ReEnc.ElGamal_Encryption(origin1,tmp,publickey);
    cipher_txt.push_back(tmp);
    tmp.clear();
    Pre_ReEnc.ElGamal_Encryption(origin2,tmp,publickey);
    cipher_txt.push_back(tmp);
    tmp.clear();
    Pre_ReEnc.ElGamal_Encryption(direction,tmp,publickey);
    cipher_txt.push_back(tmp);
    tmp.clear();
    Pre_ReEnc.ElGamal_Encryption(gamma,tmp,publickey);
    cipher_txt.push_back(tmp);
}

void Elgamal_Enc(vector<pair<double,string>> &qurey,vector<pair<double,string>> &cipher_txt,Integer publickey)
{
    for(auto a:qurey)
    {   
        string tmp;
        Pre_ReEnc.ElGamal_Encryption(a.second,tmp,publickey);
        cipher_txt.push_back(make_pair(a.first,tmp));
    }
}

void Elgamal_Dec(vector<string> cipher_txt,vector<string> &phi_dec,Integer privatekey)
{
    string tmp;
    string order = cipher_txt[0];
    string origin1 = cipher_txt[1];
    string origin2 = cipher_txt[2];
    string direction = cipher_txt[3];
    string gamma = cipher_txt[4];
    Pre_ReEnc.ElGamal_Decryption(order,tmp,privatekey);
    phi_dec.push_back(tmp);
    tmp.clear();
    Pre_ReEnc.ElGamal_Decryption(origin1,tmp,privatekey);
    phi_dec.push_back(tmp);
    tmp.clear();
    Pre_ReEnc.ElGamal_Decryption(origin2,tmp,privatekey);
    phi_dec.push_back(tmp);
    tmp.clear();
    Pre_ReEnc.ElGamal_Decryption(direction,tmp,privatekey);
    phi_dec.push_back(tmp);
    tmp.clear();
    Pre_ReEnc.ElGamal_Decryption(gamma,tmp,privatekey);
    phi_dec.push_back(tmp);
}

void Elgamal_Dec(vector<pair<double,string>> &cipher_txt,vector<pair<double,string>> &dec_txt,Integer privatekey)
{
    for(auto a:cipher_txt)
    {   
        string tmp;
        Pre_ReEnc.ElGamal_Decryption(a.second,tmp,privatekey);
        dec_txt.push_back(make_pair(a.first,tmp));
    }
}

void encode(vector<vector<Point>> &trajectionList,vector<vector<pair<double,string>>> &encodingList)
{
    hilbertcurve hc({8,{0,0},'u',1.0});
    for(auto a:trajectionList)
    {
        vector<pair<double,string>> tmp;
        for(auto b:a)
        {
            tmp.push_back(make_pair(b.time,hc.xy2H[b.x][b.y]));
        }
        encodingList.push_back(tmp);
    }
}

void decode(vector<vector<Point>> &trajectionList,vector<vector<pair<double,string>>> &encodingList)
{
    hilbertcurve hc({8,{0,0},'u',1.0});
    for(auto a:encodingList)
    {
        vector<Point> tmp;
        for(auto b:a)
        {
            pair<uint,uint> x = hc.H2xy[b.second];
            tmp.push_back({b.first,(double) x.first,(double) x.second});
        }
        trajectionList.push_back(tmp);
    }
}

void encode(vector<Point> &trajectionList,vector<pair<double,string>> &encodingList)
{
    hilbertcurve hc({8,{0,0},'u',1.0});
    for(auto a:trajectionList)
    {
        pair<double,string> tmp;
        tmp = make_pair(a.time,hc.xy2H[a.x][a.y]);
        encodingList.push_back(tmp);
    }
}

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

int main(){
    string file_name("data.txt");
    uint N = pow(2,3);//order of the curve
    int r=3;//the number of curve;
    vector<Phi> phi_list;
    phi_list.push_back({N,{0,0},'u',1.0});
    phi_list.push_back({N,{0,0},'r',1.0});
    phi_list.push_back({N,{7,0},'u',1.0});
    vector<vector<Point>> trajectionList;
    read_data(file_name,trajectionList);
    vacuate(trajectionList);
    // for(auto a:trajectionList)
    // {
    //     for(auto b:a)
    //     {
    //         cout<<b.time<<b.x<<b.y<<" ";
    //     }
    //     cout <<"\n";
    // }
    mapping_table(trajectionList,phi_list,dict);
    // for(auto a:trajectionList[1])
    // {
    //     hilbertcurve hc({N,{0,0},'u',1.0});
    //     string h = hc.xy2H[a.x][a.y];
    //     vector<string> b = dict[h];
    //     for(auto c:b)
    //     {
    //         cout << c <<"\n";
    //     }
    // }
    vector<vector<pair<double,string>>> encodingList;
    encode(trajectionList,encodingList);
    ElGamal_keys DO_key = Pre_ReEnc.ElGamal_key_generation();
    ElGamal_keys QU_key = Pre_ReEnc.ElGamal_key_generation();
    ElGamal_keys CS1_key = Pre_ReEnc.ElGamal_key_generation();
    vector<string> ID = {string("0"),string("1"),string("2"),string("3"),string("4")};
    pair<vector<string>,Capsule> mess = Pre_ReEnc.Pre_Enc(ID,DO_key.publickey);
    vector<string> cipher_txt;
    Elgamal_Enc(phi_list[0],cipher_txt,QU_key.publickey);
    pair<Integer,Integer> comversion_key = Pre_ReEnc.Pre_ReKeyGen(DO_key.privatekey,QU_key.publickey);
    vector<Point> search_qu = trajectionList[0];
    vector<pair<double,string>> encoding_qu;
    vector<string> phi_dec;
    Elgamal_Dec(cipher_txt,phi_dec,QU_key.privatekey);
    // for(auto a:phi_dec)
    // {
    //     cout<<a<<" "<<"\n";
    // }
    encode(search_qu,encoding_qu);
    // for(auto a:encoding_qu)
    // {
    //     cout<<a.first<<" "<<a.second<<" ";
    // }
    vector<pair<double,string>> qu_Enc;
    vector<pair<double,string>> qu_Dec;
    Elgamal_Enc(encoding_qu,qu_Enc,CS1_key.publickey);
    Elgamal_Dec(qu_Enc,qu_Dec,CS1_key.privatekey);
    // for(auto a:qu_Dec)
    // {
    //     cout<<a.first<<" "<<a.second<<" ";
    // }
    vector<double> distlist;
    for(int i=0;i<encodingList.size();i++)
    {
        distlist.push_back(DSED(encoding_qu,encodingList[i],dict));
    }
//     for(auto a:distlist){
//         cout << a <<"\n";
//     }
    vector<pair<double,int>> K;
    top_K(4,distlist,K);
    // for(auto x:K)
    // {
    //     cout<<x.first<<" "<<x.second<<"\n";
    // }
    vector<vector<pair<double,string>>> encondinglist_K;
    for(auto x:K)
    {
        encondinglist_K.push_back(encodingList[x.second]);
    }
    vector<pair<mpz_class,double>> distlist1;
    for(int i=0;i<encondinglist_K.size();i++)
    {
        distlist1.push_back(DSED1(encoding_qu,encondinglist_K[i],dict));
    }
    int ra = rand()%10;
    int rb = rand()%10-5;
    // string ra_str = to_string(ra);
    // string rb_str = to_string(rb);
    // string ra_Enc,rb_Enc;
    // Pre_ReEnc.ElGamal_Encryption(ra_str,ra_Enc,QU_key.publickey);
    // Pre_ReEnc.ElGamal_Encryption(rb_str,rb_Enc,QU_key.publickey);
    // string ra_Dec,rb_Dec;
    // Pre_ReEnc.ElGamal_Decryption(ra_Enc,ra_Dec,QU_key.privatekey);
    // Pre_ReEnc.ElGamal_Decryption(rb_Enc,rb_Dec,QU_key.privatekey);
    // cout<<ra<<" "<<rb<<"\n";
    // cout<<ra_Dec<<" "<<rb_Dec<<"\n";
    vector<mpz_class> ESD;
    for(auto x:distlist1)
    {
        mpz_class y;
        myshe.SHE_ency(mpz_class(x.second),y);
        ESD.push_back(ra*x.first+y);
    }
    vector<mpz_class> SD;
    for(auto x:ESD)
    {
        mpz_class tmp;
        myshe.SHE_dec(tmp,x);
        SD.push_back(tmp);
    }
    vector<double> D;
    for(int i=0;i<SD.size();i++)
    {
        D.push_back(SD[i].get_d()/distlist1[i].second);
    }
    vector<pair<double,int>> k;
    top_K(3,D,k);
    vector<string> kid;
    vector<vector<pair<double,string>>> R;
    for(auto x:k)
    {
        kid.push_back(mess.first[K[x.second].second]);
        R.push_back(encodingList[K[x.second].second]);
    }
    Capsule cap =  Pre_ReEnc.Pre_ReEncryption(comversion_key.first,mess.second);
    Integer pri_key = Pre_ReEnc.Pre_ReCreateKey(QU_key.privatekey,QU_key.publickey,cap,comversion_key.second);
    vector<string> txt_Dec = Pre_ReEnc.Pre_Decryption(pri_key,kid);
    // for(auto x:txt_Dec)
    // {
    //     cout<<x<<"\n";
    // }
    vector<vector<Point>> resultList;
    decode(resultList,R);
    for(auto x:resultList)
    {
        for(auto y:x)
        {
            cout<<y.time<<" "<<y.x<<" "<<y.y<<" ";
        }
        cout<<"\n";
    }
}