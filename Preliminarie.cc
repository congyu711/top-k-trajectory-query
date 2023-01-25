#ifndef __PREL
#define __PREL
#include "Douglas_Peucker.cc"
#include "Proxy_ReEncryption.cc"
#include "hilbertcurve.cc"
#include "SHE_Encryption.cc"
using namespace std;

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

// 对序列抽稀
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

//generate the parameters(phi) of hilbertcurve
vector<Phi>  hilbertcurve_generator(uint order,int num)
{
    uint N = pow(2,order);
    vector<Phi> Phi_list;
    if(num>8)
    { 
        return Phi_list;
    }
    else
    {
        vector<Phi> list{
            {N,{0,0},'u',1.0},{N,{N-1,0},'u',1.0},{N,{0,N-1},'d',1.0},{N,{N-1,N-1},'d',1.0},
            {N,{N-1,0},'l',1.0},{N,{N-1,N-1},'l',1.0},{N,{0,0},'r',1.0},{N,{0,N-1},'r',1.0}
        };
        vector<int> a = {0,1,2,3,4,5,6,7};
        random_shuffle(a.begin(),a.end());
        for(int i=0;i<num;i++)
        {
            Phi_list.push_back(list[a[i]]);
        }
    }
    return Phi_list;
}

//establish the mapping table of the trajection list
void mapping_table(vector<vector<Point>> &trajectionList,vector<Phi> phi_list,map<string,vector<string>> &dict,SHE &she,pair<Integer,Integer> sk)
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
            Integer Ex = she.Encryption(b.x,sk,she.N);
            Integer Ey = she.Encryption(b.y,sk,she.N);
            a.push_back(Integer_to_string(Ex));
            a.push_back(Integer_to_string(Ey));
            dict.insert(make_pair(h,a));
        }
    }
}

#ifdef __Preliminarie__test__
int main()
{
   auto phi_list = hilbertcurve_generator(3,4);
//    for(auto s:a)
//    {
//         cout<<s.direcion<<" "<<s.gamma<<" "<<s.order<<" "<<s.origin.first<<" "<<s.origin.second<<endl;
//    }
//    cout<<endl;
//    auto b = hilbertcurve_generator(3,8);
//     for(auto s:b)
//    {
//         cout<<s.direcion<<" "<<s.gamma<<" "<<s.order<<" "<<s.origin.first<<" "<<s.origin.second<<endl;
//    }
//    cout<<endl;
//    auto c = hilbertcurve_generator(3,10);
//    if(c.empty())
//    {
//         cout<<"error"<<endl;
//    }
    string file_name("data.txt");
    vector<vector<Point>> trajectionList;
    map<string,vector<string>> dict;
    SHE she(500,16,100);
    // she.setup();
    auto sk = she.Key_Generation();
    read_data(file_name,trajectionList);
    vacuate(trajectionList);
    mapping_table(trajectionList,phi_list,dict,she,sk);
    for(auto a:trajectionList[1])
    {
        hilbertcurve hc({phi_list[0].order,phi_list[0].origin,phi_list[0].direcion,phi_list[0].gamma});
        string h = hc.xy2H[a.x][a.y];
        vector<string> b = dict[h];
        for(auto c:b)
        {
            cout << c <<"\n";
        }
    }
}
#endif
#endif