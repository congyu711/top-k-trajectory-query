#include "main.cc"
#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else
#include "./cmake/build/DO_and_QU.grpc.pb.h"
#endif

#include "./cmake/build/CS1.grpc.pb.h"

typedef vector<pair<double,string>> searchQuery;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using DO_and_QU::DOQU_Greeter;
using DO_and_QU::ParameterReply;
using DO_and_QU::PublicKeyRequest;

using CS1::QUCS1_Greeter;
using CS1::PublicKeyReply;
using QUCS1_PublicKeyRequest = CS1::PublicKeyRequest;
using CS1::QUReply;
using CS1::QURequest;

QueryUser QU;

class QUCS1Client {
    public:
    QUCS1Client(std::shared_ptr<Channel> channel)
    : stub_(QUCS1_Greeter::NewStub(channel)) {}

    std::string PublicKey(const std::string& publicKey) {
        QUCS1_PublicKeyRequest request;
        request.set_name(publicKey);
        std::cout<<"set_QUpublicKey"<<"\n";
        PublicKeyReply reply;

        ClientContext context;

        Status status = stub_->PublicKey(&context,request,&reply);

        if (status.ok()) {
        // std::string str=reply.SerializeAsString();
            QU.CS1_key_publickey = Integer(reply.message().c_str());
            std::cout<<"reply_CS1publicKey"<<"\n";
            return "OK";
        } else {
            std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
            return "RPC failed";
        }
    }

    std::string QUSearch(int k,searchQuery &qu) {
        QURequest request;
        request.set_k(k);
        std::cout<<"k = "<<k<<"\n";
        vector<CS1::QURequest_qu_Enc> tmp;
        for(auto x:qu) {
          CS1::QURequest_qu_Enc tmp1;
          tmp1.set_t(x.first);
          tmp1.set_hpoint(x.second);
            // request.add_qu_encs()->set_t(x.first);
            // request.add_qu_encs()->set_hpoint(x.second);
          tmp.push_back(tmp1);
        }
        request.mutable_qu_encs()->CopyFrom({tmp.begin(),tmp.end()});
        std::cout<<"qu"<<"\n";
        QUReply reply;
        ClientContext context;
        Status status = stub_->QUSearch(&context,request,&reply);

        if(status.ok()) {
            QU.cap = Capsule(Integer(reply.cap().e().c_str()),Integer(reply.cap().v().c_str()),Integer(reply.cap().s().c_str()));
            cout<<"cap"<<"\n";
            vector<string> tmp;
            for(auto x:reply.kid()) {
                tmp.push_back(x);
            }
            QU.kid = tmp;
            std::cout<<"kid"<<"\n";
            vector<vector<pair<double, string>>> tmp1;
            for(auto x:reply.enc_results()) {
              vector<pair<double,string>> tmp;
              for(auto y:x.query_result()) {
                tmp.push_back(make_pair(y.t(),y.hpoint()));
                // cout<<y.t()<<" "<<y.hpoint()<<"\n";
              }
              tmp1.push_back(tmp);
            }
            if(tmp1.empty()) cout<<"tmp1 is empty"<<endl;
            QU.Enc_result = tmp1;
            cout<<"Enc_result"<<"\n";
            return "OK";
          }
          else {
            std::cout << status.error_code() << ": " << status.error_message()
                      << std::endl;
                   return "RPC failed";
           }
      }

    private:
        std::unique_ptr<QUCS1_Greeter::Stub> stub_;
};

class GreeterClient {
 public:
  GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(DOQU_Greeter::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  std::string PublicKey(const std::string& publicKey) {
    // Data we are sending to the server.
    PublicKeyRequest request;
    request.set_publickey(publicKey);
    std::cout <<"set_QUpublicKey"<<"\n";
    // Container for the data we expect from the server.
    ParameterReply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;
    
    // The actual RPC.
    Status status = stub_->PublicKey(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {
        // std::string str=reply.SerializeAsString();
        auto parasize = reply.parameter_size();
        vector<std::string> tmp;
        for(int i=0;i<parasize;i++)
        {
            tmp.push_back(reply.parameter(i));
        }
        QU.ciphertxt_Phi = tmp;
        std::cout<<"accept"<<"\n";
        QU.decrypt_Phi();
        std::cout<<"decrypt"<<"\n";
        QU.conversion_key = Integer(reply.conversionkey().c_str());
        QU.DO_key_publickey = Integer(reply.publickey().c_str());
        // std::cout<<str<<'\n';
        for(auto &e:QU.phi_dec)   std::cout<<e<<'\n';
      return "OK";
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }

 private:
  std::unique_ptr<DOQU_Greeter::Stub> stub_;
};

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint specified by
  // the argument "--target=" which is the only expected argument.
  // We indicate that the channel isn't authenticated (use of
  // InsecureChannelCredentials()).
  std::string target_str;
  std::string arg_str("--target");
  if (argc > 1) {
    std::string arg_val = argv[1];
    size_t start_pos = arg_val.find(arg_str);
    if (start_pos != std::string::npos) {
      start_pos += arg_str.size();
      if (arg_val[start_pos] == '=') {
        target_str = arg_val.substr(start_pos + 1);
      } else {
        std::cout << "The only correct argument syntax is --target="
                  << std::endl;
        return 0;
      }
    } else {
      std::cout << "The only acceptable argument is --target=" << std::endl;
      return 0;
    }
  } else {
    target_str = "localhost:50051";
  }

  GreeterClient greeter(
      grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials())
  );
  std::string QUpublicKey = Integer_to_string(QU.QU_key.publickey);
  std::string reply = greeter.PublicKey(QUpublicKey);
  std::cout << "Greeter received: " << reply << std::endl; 
  
  QUCS1Client greeter1(
    grpc::CreateChannel("localhost:50052", grpc::InsecureChannelCredentials())
  );
  std::string reply1 = greeter1.PublicKey(QUpublicKey);
  std::cout << "Greeter1 received: " << reply1 << std::endl;

  QU.Encrypt_Query();
  std::string reply2 = greeter1.QUSearch(3,QU.qu_Enc);
  std::cout << "Greeter1 received: " << reply2 << std::endl;
  QU.Decrypt_ID();
  cout<<1<<'\n';
  QU.Decrypt_Result();
  cout<<2<<'\n';
  if(QU.Enc_result.empty()) cout<<"Enc_result is empty";
  if(!QU.dec_result.empty()) {
    for(auto x:QU.dec_result) {
      for(auto y:x) {
        cout<<y.time<<" "<<y.x<<" "<<y.y<<" ";
      }
      cout<<"\n";
    }
  } else {
    cout<<"empty"<<endl;
  }                                           
  return 0;
}
