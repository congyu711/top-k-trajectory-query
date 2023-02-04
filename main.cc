// #include <bits/stdc++.h>
// #include <cryptopp/integer.h>
#include "Cryption_Code.cc"
#include "Preliminarie.cc"
#include "sort.cc"
#include <boost/python.hpp>

using namespace std;
using namespace CryptoPP;

class fullprocess
{
public:
    const int a=10;
    map<string,vector<string>> dict;
    Proxy_ReEncryption Pre_ReEnc;
    SHE she;
    bool exec(string datafile="data.txt");
};

bool fullprocess::exec(string datafile)
{
    // she.setup();
    string file_name(datafile);
    uint N = 3;//order of the curve
    int r=3;//the number of curve;
    auto phi_list = hilbertcurve_generator(N,r);
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
    auto sk = she.Key_Generation();
    mapping_table(trajectionList,phi_list,dict,she,sk);
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
    database_encode(trajectionList,phi_list[0],encodingList);
    ElGamal_keys DO_key = Pre_ReEnc.ElGamal_key_generation();
    ElGamal_keys QU_key = Pre_ReEnc.ElGamal_key_generation();
    ElGamal_keys CS1_key = Pre_ReEnc.ElGamal_key_generation();
    vector<string> ID = {string("0"),string("1"),string("2"),string("3"),string("4")};
    pair<vector<string>,Capsule> mess = Pre_ReEnc.Pre_Enc(ID,DO_key.privatekey,QU_key.publickey);
    vector<string> cipher_txt;
    Phi_Enc(phi_list[0],cipher_txt,Pre_ReEnc,QU_key.publickey);
    pair<Integer,Integer> comversion_key = Pre_ReEnc.Pre_ReKeyGen(DO_key.privatekey,QU_key.publickey);
    vector<Point> search_qu = trajectionList[0];
    vector<pair<double,string>> encoding_qu;
    vector<string> phi_dec;
    Phi_Dec(cipher_txt,Pre_ReEnc,phi_dec,QU_key.privatekey);
    // for(auto a:phi_dec)
    // {
    //     cout<<a<<" "<<"\n";
    // }
    query_encode(search_qu,phi_list[0],encoding_qu);
    // for(auto a:encoding_qu)
    // {
    //     cout<<a.first<<" "<<a.second<<" ";
    // }
    vector<pair<double,string>> qu_Enc;
    vector<pair<double,string>> qu_Dec;
    Elgamal_Enc(encoding_qu,Pre_ReEnc,qu_Enc,CS1_key.publickey);
    Elgamal_Dec(qu_Enc,Pre_ReEnc,qu_Dec,CS1_key.privatekey);
    // for(auto a:qu_Dec)
    // {
    //     cout<<a.first<<" "<<a.second<<" ";
    // }
    vector<double> distlist;
    compute_dist(distlist,encodingList,dict,encoding_qu);
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
    vector<pair<Integer,double>> distlist1;
    compute_dist1(distlist1,K,she,sk,encondinglist_K,encodingList,dict,encoding_qu);
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
    vector<Integer> ESD = compute_ESD(distlist1);
    vector<double> D = compute_D(she,sk,distlist1,ESD);
    vector<pair<double,int>> k;
    top_K(3,D,k);
    vector<string> kid;
    vector<vector<pair<double,string>>> result_trajection;
    trajection_and_ID(encodingList,mess,K,kid,result_trajection,k);
    Capsule cap =  Pre_ReEnc.Pre_ReEncryption(comversion_key.first,mess.second);
    Integer pri_key = Pre_ReEnc.Pre_ReCreateKey(QU_key.privatekey,QU_key.publickey,DO_key.publickey,cap,comversion_key.second);
    vector<string> txt_Dec = Pre_ReEnc.Pre_Decryption(pri_key,kid);
    // for(auto x:txt_Dec)
    // {
    //     cout<<x<<"\n";
    // }
    vector<vector<Point>> resultList;
    decode(resultList,phi_list[0],result_trajection);
    for(auto x:resultList)
    {
        for(auto y:x)
        {
            cout<<y.time<<" "<<y.x<<" "<<y.y<<" ";
        }
        cout<<"\n";
    }
    return true;
}
BOOST_PYTHON_MODULE(fullprocess)
{
    boost::python::class_<fullprocess, boost::noncopyable>("fullprocess", boost::python::init<>())
        .def("exec", &fullprocess::exec);
}


/*

g++ main.cc -fPIC -shared -o fullprocess.so -I/usr/include/python3.10 -L. -lboost_python310 -lcryptopp

example.py:
    import fullprocess

    ins=fullprocess.fullprocess()
    ins.exec("data.txt")
*/