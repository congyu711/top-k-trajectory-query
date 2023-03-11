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

        PublicKeyReply reply;

        ClientContext context;

        Status status = stub_->PublicKey(&context,request,&reply);

        if (status.ok()) {
        // std::string str=reply.SerializeAsString();
            QU.CS1_key_publickey = Integer(reply.message().c_str());
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
        for(auto x:qu) {
            request.add_qu_encs()->set_t(x.first);
            request.add_qu_encs()->set_hpoint(x.second);
        }
        QUReply reply;
        ClientContext context;
        Status status = stub_->QUSearch(&context,request,&reply);

        if(status.ok()) {
            QU.cap = Capsule(Integer(reply.cap().e().c_str()),Integer(reply.cap().v().c_str()),Integer(reply.cap().s().c_str()));
            for(auto x:reply.kid()) {
                QU.kid.push_back(x);
            }
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
        auto parasize=reply.parameter_size();
        for(int i=0;i<parasize;i++)
        {
            QU.ciphertxt_Phi.push_back(reply.parameter(i));
        }
        QU.decrypt_Phi();
        QU.conversion_key = Integer(reply.conversionkey().c_str());
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

  //QU.Encode_Query();
  QU.Encrypt_Query();
  std::string reply2 = greeter1.QUSearch(3,QU.qu_Enc);
  
  QU.Decrypt_ID();
  QU.Decrypt_Result();

  return 0;
}
