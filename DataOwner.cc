#include "main.cc"
#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "./cmake/build/DO_and_QU.grpc.pb.h"
#include "./cmake/build/DO_and_CS1.grpc.pb.h"
#include "./cmake/build/DO_and_CS2.grpc.pb.h"

typedef map<string,vector<string>> maptable;
typedef vector<vector<pair<double,string>>> trajectorytype;
typedef pair<vector<string>,Capsule> messagetype;

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::Channel;
using grpc::ClientContext;
using DO_and_QU::DOQU_Greeter;
using DO_and_QU::ParameterReply;
using DO_and_QU::PublicKeyRequest;
using DO_CS1::DOCS1_Greeter;
using DO_CS1::msg;
using DO_CS2::DOCS2_Greeter;
using DO_CS2::SHE_pk;

DataOwner DO(3,3,"data.txt");

class DOCS1Client {
  public:
    DOCS1Client(std::shared_ptr<Channel> channel)
    : stub_(DOCS1_Greeter::NewStub(channel)) {}

    std::string seedMessage(Integer conversionKey,maptable dict,trajectorytype trajectory,messagetype message) {
      msg request;
      request.set_rk(Integer_to_string(conversionKey));
      for(auto x:dict) {
        for(auto y:x.second) {
          request.mutable_mapping_table()->at(x.first).add_table_content(y);
        }
      }
      for(auto x:trajectory) {
        for(auto y:x) {
          request.add_encodinglist()->add_encoded()->set_time(y.first);
          request.add_encodinglist()->add_encoded()->set_hil(y.second);
        }
      }
      for(auto x:message.first) {
        request.mutable_encryptedid()->add_enc_val(x);
      }
      request.mutable_encryptedid()->mutable_very_val()->set_x1(Integer_to_string(message.second.E));
      request.mutable_encryptedid()->mutable_very_val()->set_x1(Integer_to_string(message.second.V));
      request.mutable_encryptedid()->mutable_very_val()->set_x1(Integer_to_string(message.second.s));

      ClientContext context;

      google::protobuf::Empty* reply;

      Status status = stub_->SeedMessage(&context,request,reply);
      if(status.ok()) {
        return "OK ";
      }
      else {
        std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
        return "RPC failed";
      }
    }
  
  private:
    std::unique_ptr<DOCS1_Greeter::Stub> stub_;
};

class DOCS2_Client {
  public:
    DOCS2_Client(std::shared_ptr<Channel> channel)
    : stub_(DOCS2_Greeter::NewStub(channel)) {}

    std::string seedMessage() {
      SHE_pk request;
      request.set_sk1(Integer_to_string(DO.she_sk.first));
      request.set_sk2(Integer_to_string(DO.she_sk.second));

      google::protobuf::Empty* reply;

      ClientContext context;

      Status status = stub_->SeedMessage(&context,request,reply);
      if(status.ok()) {
        return "OK ";
      }
      else {
        std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
        return "grpc error";
      }
    }

  private:
    std::unique_ptr<DOCS2_Greeter::Stub> stub_;
};

// Logic and data behind the server's behavior.
class GreeterServiceImpl final : public DOQU_Greeter::Service {
  Status PublicKey(ServerContext* context, const PublicKeyRequest* request,
                  ParameterReply* reply) override {

    // repeated string
    // reply->set_parameter(prefix + request->publickey());
    // reply->add_parameter("1114514\n");
    // reply->add_parameter("1919810\n");
    // reply->add_parameter("Ahhhhh!!!!!\n");
    DO.QU_key_publickey = Integer(request->publickey().c_str());
    cout<<request->publickey()<<"\n";
    DO.encrypt_QUpubkey();
    for(auto x:DO.ciphertxt_Phi) {
      reply->add_parameter(x);
    }
    reply->set_conversionkey(Integer_to_string(DO.conversion_key.second));
    return Status::OK;  
  }
};

void seedMsgTOCS2() {
  DOCS2_Client greeter1(grpc::CreateChannel("localhost:50053",grpc::InsecureChannelCredentials()));
  std::string reply1 = greeter1.seedMessage();
  std::cout << "Greeter received: " << reply1 << std::endl;
}

void seedMsgTOCS1() {
  DOCS1Client greeter(grpc::CreateChannel("localhost:50052",grpc::InsecureChannelCredentials()));
  std::string reply = greeter.seedMessage(DO.conversion_key.first,DO.dict,DO.encodingList,DO.mess);
  std::cout << "Greeter received: " << reply << std::endl;
}

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  GreeterServiceImpl service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char** argv) {
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
    target_str = "localhost:50052";
  }
  seedMsgTOCS1();
  seedMsgTOCS2();
  RunServer();
  return 0;
}