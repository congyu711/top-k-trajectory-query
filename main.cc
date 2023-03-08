// server classes

#include "Cryption_Code.cc"
#include "Preliminarie.cc"
#include "sort.cc"

using namespace std;
using namespace CryptoPP;

class server_base
{
public:
    Proxy_ReEncryption Pre_ReEnc;
    SHE she;
    uint N; // order of hilbert curve
    int r; //number of curce;
    // bool DO();
    // bool QU();
    // bool CS1();
    // bool CS2();
};

//////////////////////////////////////////////////////////////////////////
// TODO: some of definitions in this class should be moved to server_base.
//////////////////////////////////////////////////////////////////////////
class DataOwner: public server_base
{
public:

    vector<Phi> phi_list;   // the return type of hilbertcurve_generator
    string fileName;    // input file
    ///////////////////////////////////////////////////////////////////
    // TODO: use database!!!
    vector<vector<Point>> trajectionList;   // list
    vector<string> ID;                  // primary key in the database
    ///////////////////////////////////////////////////////////////////
    pair<Integer, Integer> she_sk;      // all keys are fixed
    ElGamal_keys DO_key;
    Integer QU_key_publickey;   // this should be received from QU;
    map<string,vector<string>> dict;    // mapping table
    vector<string> ciphertxt_Phi;       // encrypted hilbert curve para.s
                                        // sent this to QU
    vector<vector<pair<double,string>>> encodingList;
    pair<vector<string>,Capsule> mess;
    pair<Integer,Integer> conversion_key;

    DataOwner(uint _N, int _r, string _filename):N(_N), r(_r), fileName(_filename){
        ///////////////////////////////////////////
        read_data(fileName, trajectionList);    // TODO: use real database!!!
        ///////////////////////////////////////////

        phi_list=hilbertcurve_generator(N,r);
        vacuate(trajectionList);
        she_sk=she.Key_Generation();
        DO_key=Pre_ReEnc.ElGamal_key_generation();
        // generate mapping table
        mapping_table(trajectionList,phi_list,dict,she,she_sk);
        database_encode(trajectionList,phi_list[0],encodingList);
        
    }
    void encrypt_QUpubkey()
    {
        mess = Pre_ReEnc.Pre_Enc(ID,DO_key.privatekey,QU_key_publickey);
        // generate ciphertxt_Phi
        Phi_Enc(phi_list[0],ciphertxt_Phi,Pre_ReEnc,QU_key_publickey);
        // generate conversion_key
        conversion_key = Pre_ReEnc.Pre_ReKeyGen(DO_key.privatekey,QU_key_publickey);
    }
};

bool server_base::DO() {
    //generate the parameter of hilbertcurve
    uint N = 3;//order of the curve
    int r=3;//the number of curve;
    auto phi_list = hilbertcurve_generator(N,r);
    
    //服务器数据库中储存轨迹，由Kotlin中调用函数传入
    vector<vector<Point>> trajectionList;
    //read_data(trajectionList);
    string file_name("data.txt");
    read_data(file_name,trajectionList);

    //generate mapping table
    vacuate(trajectionList);

    auto sk = she.Key_Generation();//SHE的密钥
    //send sk to cloud_server2

    map<string,vector<string>> dict;
    mapping_table(trajectionList,phi_list,dict,she,sk);//映射表
    
    //encode and encrypt the trajectionList
    vector<vector<pair<double,string>>> encodingList;//编码轨迹
    database_encode(trajectionList,phi_list[0],encodingList);
    ElGamal_keys DO_key = Pre_ReEnc.ElGamal_key_generation();
    
    Integer QU_key_publickey;//由QU发送给server
    
    vector<string> ID{};//轨迹ID为其在数据库中的ID
    pair<vector<string>,Capsule> mess = Pre_ReEnc.Pre_Enc(ID,DO_key.privatekey,QU_key_publickey);
    
    //Encrypt the paramter of hilbertcurve and generate the conversion_key
    vector<string> ciphertxt_Phi;//希尔伯特曲线参数密文

    Phi_Enc(phi_list[0],ciphertxt_Phi,Pre_ReEnc,QU_key_publickey);//发送给QU

    pair<Integer,Integer> conversion_key = Pre_ReEnc.Pre_ReKeyGen(DO_key.privatekey,QU_key_publickey);

    //send mapping_table(dict),encoding_trajectionList(encodingList) and encoding_ID(mess) conversion_key to cs1
    return true;
}

class QueryUser: public server_base
{

};

bool server_base::QU() {
    ElGamal_keys QU_key = Pre_ReEnc.ElGamal_key_generation();
    //send publicKey to DO and CS1

    //用户传入搜索轨迹
    vector<Point> search_qu;
    
    Integer DO_key_publickey;//accept from server
    
    //decrypt the cipherTxt of paramter
    vector<string> ciphertxt_Phi;//accept from server
    vector<pair<double,string>> encoding_qu;
    vector<string> phi_dec;
    Phi_Dec(ciphertxt_Phi,Pre_ReEnc,phi_dec,QU_key.privatekey);

    //encode the query_trajection
    uint order = strtoul(phi_dec[0].c_str(),NULL,10);
    int x = atoi(phi_dec[1].c_str());
    int y = atoi(phi_dec[2].c_str());
    char direction = phi_dec[3][0];
    double gamma = strtod(phi_dec[4].c_str(),NULL);
    Phi phi{order,{x,y},direction,gamma};
    query_encode(search_qu,phi,encoding_qu);

    Integer CS1_key_publickey;//accept from CS1
    
    //Encrypt the encoding query
    
    vector<pair<double,string>> qu_Enc;
    Elgamal_Enc(encoding_qu,Pre_ReEnc,qu_Enc,CS1_key_publickey);

    //send the qu_Enc and k to CS1

    Integer conversion_key;//accept from CS1
    Capsule cap;//accept from CS1
    vector<string> kid;//accept from CS1
    vector<vector<pair<double,string>>> Enc_result;//accept from CS1

    Integer pri_key = Pre_ReEnc.Pre_ReCreateKey(QU_key.privatekey,QU_key.publickey,DO_key_publickey,cap,conversion_key);
    vector<string> txt_Dec = Pre_ReEnc.Pre_Decryption(pri_key,kid);
    vector<vector<pair<double,string>>> dec_result;
    for(auto a:Enc_result) {
        vector<pair<double,string>> tmp;
        Elgamal_Dec(a,Pre_ReEnc,tmp,QU_key.privatekey);
        dec_result.push_back(tmp);
    }
    vector<vector<Point>> resultList;
    decode(resultList,phi,dec_result);
    // for(auto x:resultList)
    // {
    //     for(auto y:x)
    //     {
    //         cout<<y.time<<" "<<y.x<<" "<<y.y<<" ";
    //     }
    //     cout<<"\n";
    // }
    return true;
}

class CloudServer1: public server_base
{

};

bool server_base::CS1() {
    ElGamal_keys CS1_key = Pre_ReEnc.ElGamal_key_generation(); 

    //accept from client
    vector<pair<double,string>> qu_Enc;
    Integer QU_key_publicKey;
    int k;
    
    //accept from server
    map<string,vector<string>> dict;
    vector<vector<pair<double,string>>> encodingList;
    pair<vector<string>,Capsule> mess;
    pair<Integer,Integer> conversion_key;

    //Decryp the qu_Dec
    vector<pair<double,string>> qu_Dec;
    Elgamal_Dec(qu_Enc,Pre_ReEnc,qu_Dec,CS1_key.privatekey);

    //compute the approximate distance
    vector<double> distlist;
    compute_dist(distlist,encodingList,dict,qu_Dec);

    //top-K
    vector<pair<double,int>> K;
    top_K(2*k,distlist,K);

    vector<vector<pair<double,string>>> encondinglist_K;
    vector<pair<Integer,double>> distlist1;
    compute_dist1(distlist1,K,encondinglist_K,encodingList,dict,qu_Dec);
    vector<Integer> ESD = compute_ESD(distlist1);
    vector<double> L;
    for(auto x:distlist1) {
        L.push_back(x.second);
    }

    //send the k L ESD and KID(K) to cs2

    vector<pair<double,int>> kid;//accept from cs2

    vector<string> ID_Topk;
    vector<vector<pair<double,string>>> result_trajection;
    trajection_and_ID(encodingList,mess,K,ID_Topk,result_trajection,kid);

    Capsule cap =  Pre_ReEnc.Pre_ReEncryption(conversion_key.first,mess.second);
    vector<vector<pair<double,string>>> Enc_result;
    for(auto a:result_trajection) {
        vector<pair<double,string>> tmp;
        Elgamal_Enc(a,Pre_ReEnc,tmp,QU_key_publicKey);
        Enc_result.push_back(tmp);
    }

    //send cap, comversionKey.second, kid and Enc_result to client
    return true;
}

class CloudServer2: public server_base
{

};

bool server_base::CS2() {
    pair<Integer,Integer> sk;//accept from DO
    
    //accept from cs1
    vector<double> L;
    vector<Integer> ESD;

    vector<double> D = compute_D(she,sk,L,ESD);
    vector<pair<double,int>> k;
    top_K(3,D,k);

    //send k to cs1
    return true;
}

// int main() {
//     server_base test;
//     cout<<test.she.k0<<endl;
// }