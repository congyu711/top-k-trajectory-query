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
};

class DataOwner: public server_base
{
public:
    uint N; // order of hilbert curve
    int r; //number of curce;
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
                                        // send it to CS1
    vector<string> ciphertxt_Phi;       // encrypted hilbert curve para.s
                                        // sent this to QU
    vector<vector<pair<double,string>>> encodingList;//send this to CS1
    pair<vector<string>,Capsule> mess;
    pair<Integer,Integer> conversion_key;//first send to CS1
                                        //second send to QU

    DataOwner(uint _N, int _r, string _filename): N(_N), r(_r),fileName(_filename){
        ///////////////////////////////////////////
        read_data(fileName, trajectionList);    // TODO: use real database!!!
        ///////////////////////////////////////////
        ID = {"1","2","3","4","5"};    //TODO: use the key in the database!!!
        phi_list=hilbertcurve_generator(N,r);
        vacuate(trajectionList);
        she_sk=she.Key_Generation();
        cout<<"SHE: "<<she.k0<<" "<<she.k1<<" "<<she.k2<<endl;
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

class QueryUser: public server_base
{
public:
    ElGamal_keys QU_key;//send publicKey to DO and CS1
    Integer conversion_key;//accept from DO
    Integer CS1_key_publickey;//accept from CS1
    Capsule cap;//accept from CS1
    vector<string> kid;//accept from CS1
    vector<vector<pair<double,string>>> Enc_result;//accept from CS1
    vector<Point> search_qu;//receive from the app
    Integer DO_key_publickey;//receive from DO
    vector<string> ciphertxt_Phi;//receive from DO. It contains the cipherTxt of parameters of hilbert
    vector<string> phi_dec;//the decryption of cipherTxt(Phi)
    Phi phi;//parameters of hilbertCurve
    vector<pair<double,string>> qu_Enc;//encryption of encoding_query
                                      //send the qu_Enc to CS1
    vector<pair<double,string>> encoding_qu;//encode the query
    vector<string> txt_Dec; //Proxy_Encryption: decrypt by reCreateKey
    Integer reCreateKey; //Proxy_Encryption generate the reCreateKey
    vector<vector<Point>> dec_result;

    QueryUser() {
        search_qu = {Point(0.0,1,1),Point(1.0,2,2),Point(2.0,3,3)};
        QU_key = Pre_ReEnc.ElGamal_key_generation();//generate the ElGamal_key(PubKey and PrivateKey) for QU
    }

    void decrypt_Phi() {
        Phi_Dec(ciphertxt_Phi,Pre_ReEnc,phi_dec,QU_key.privatekey);
        uint order = strtoul(phi_dec[0].c_str(),NULL,10);
        int x = atoi(phi_dec[1].c_str());
        int y = atoi(phi_dec[2].c_str());
        char direction = phi_dec[3][0];
        double gamma = strtod(phi_dec[4].c_str(),NULL);
        phi = Phi(order,x,y,direction,gamma);
    }

    void Encode_Query() {
        query_encode(search_qu,phi,encoding_qu);
    }

    void Encrypt_Query() {
        Encode_Query();
        Elgamal_Enc(encoding_qu,Pre_ReEnc,qu_Enc,CS1_key_publickey);
    }

    void Decrypt_ID() {
        reCreateKey = Pre_ReEnc.Pre_ReCreateKey(QU_key.privatekey,QU_key.publickey,DO_key_publickey,cap,conversion_key);
        cout<<"reCreatekey = "<<reCreateKey<<"\n";
        txt_Dec = Pre_ReEnc.Pre_Decryption(reCreateKey,kid);
        cout<<"重加密"<<"\n";
    }

    void Decrypt_Result() {
        vector<vector<pair<double,string>>> tmp1;
        for(auto a:Enc_result) {
            vector<pair<double,string>> tmp;
            Elgamal_Dec(a,Pre_ReEnc,tmp,QU_key.privatekey);
            tmp1.push_back(tmp);
        }
        decode(dec_result,phi,tmp1);
    }
};

class CloudServer1: public server_base
{
public:
    ElGamal_keys CS1_key;
    map<string,vector<string>> dict;//receive from DO
    vector<vector<pair<double,string>>> encodingList;//receive from DO
    pair<vector<string>,Capsule> mess;//receive from DO
    Integer conversion_key;//receive from DO
    vector<pair<double,string>> qu_Enc;//accept from QU. the encryption of encoding Query
    Integer QU_key_publicKey;//accept from QU.
    int k;//accept from QU
    vector<pair<double,int>> kid;//accept from CS2
    vector<double> distanceList;//the distance between searching_Query and database
    vector<pair<double,string>> qu_Dec;
    vector<pair<double,int>> K;
    vector<vector<pair<double,string>>> encondinglist_K;//the K trajectories of all(chosen by K)
    vector<pair<Integer,double>> distanceList1;
    vector<Integer> ESD;//send this to CS2
    vector<double> L;//send this to CS2
    vector<string> ID_Topk;
    vector<vector<pair<double,string>>> result_trajection;
    vector<vector<pair<double,string>>> Enc_result;
    Capsule cap;

    CloudServer1() {
        CS1_key = Pre_ReEnc.ElGamal_key_generation();
    }

    void Decrypt_Query() {
        if(qu_Enc.empty()) cout<<"qu_Enc is empty"<<"\n";
        // for(auto x:qu_Enc) {
        //     cout<<x.first<<" "<<x.second<<"\n";
        // }
        Elgamal_Dec(qu_Enc,Pre_ReEnc,qu_Dec,CS1_key.privatekey);
    }

    void Compute_ApproximateDistance() {
        compute_dist(distanceList,encodingList,dict,qu_Dec);
    }

    void firstTopK() {
        top_K(2*k,distanceList,K);
    }

    void Compute_Distance() {
        compute_dist1(distanceList1,K,encondinglist_K,encodingList,dict,qu_Dec);
    }

    void Compute_ESD() {
        Compute_Distance();
        ESD = compute_ESD(distanceList1);
        vector<double> tmp;
        for(auto x:distanceList1) {
            tmp.push_back(x.second);
        }
        L = tmp;
    }

    void Encrypt_Result() {
        if(encodingList.empty()) cout<<"encodingList is empty"<<"\n";
        if(K.empty()) cout<<"K is empty"<<"\n";
        if(kid.empty()) cout<<"kid is empty"<<"\n";
        if(mess.first.empty()) cout<<"mess.first is empty"<<"\n";
        trajection_and_ID(encodingList,mess,K,ID_Topk,result_trajection,kid);
        cout<<"trajection_and_ID"<<"\n";
        vector<vector<pair<double,string>>> tmp1;
        if(result_trajection.empty()) cout<<"result_trajection is empty"<<"\n";
        for(auto a:result_trajection) {
            vector<pair<double,string>> tmp;
            Elgamal_Enc(a,Pre_ReEnc,tmp,QU_key_publicKey);
            tmp1.push_back(tmp);
        }
        cout<<"Elgamal_Enc"<<"\n";
        Enc_result = tmp1;
    }

    void Pre_ReEncryption() {
        cap =  Pre_ReEnc.Pre_ReEncryption(conversion_key,mess.second);
    }
};

class CloudServer2: public server_base
{
public:
    pair<Integer,Integer> sk;//accept from DO
    vector<double> L;//accept from CS1
    vector<Integer> ESD;//accept from CS1
    vector<double> D;//send this to CS1
    vector<pair<double,int>> kid;//send this to CS1
    int k;

    void Topk() {
        D = compute_D(she,sk,L,ESD);
        top_K(k,D,kid);
    }
};

// int main() {
//     DataOwner Do(3,3,"data.txt");
//     // QueryUser Qu;
//     // CloudServer1();
//     // CloudServer2();
//     // Do.QU_key_publickey = Qu.QU_key.publickey;
//     // Do.encrypt_QUpubkey();
//     // Qu.ciphertxt_Phi = Do.ciphertxt_Phi;
//     // Qu.decrypt_Phi();
//     // for(auto x:Qu.phi_dec) {
//     //     cout<<x<<"\n";
//     // }
//     if(Do.dict.empty()) cout<<"dict is empty"<<endl;
//     if(Do.encodingList.empty()) cout<<"encodingList is empty"<<endl;
//     return 0;
// }   