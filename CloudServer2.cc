#include "main.cc"
#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/health_check_service_interface.h>
#include "./cmake/build/CS1_and_CS2.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::Channel;

CloudServer2 CS2;

class GreeterServiceImpl final: public CS1_CS2::CS1CS2_Greeter::Service {
    Status exactQuery(ServerContext* context, const CS1_CS2::PreResults* request,
                CS1_CS2::ExactResult* reply) override {
        CS2.k = request->k();
        // std::cout<<"k = "<<CS2.k<<"\n";
        vector<Integer> tmp;
        for(auto x:request->esd()) {
            tmp.push_back(Integer(x.c_str()));
        }
        CS2.ESD = tmp;
        // std::cout<<"ESD"<<"\n";
        // if(CS2.ESD.empty()) cout<<"ESD is empty"<<"\n";
        vector<double> tmp1;
        for(auto x:request->timelist()) {
            tmp1.push_back(x);
        }
        CS2.L = tmp1;
        // if(CS2.L.empty()) cout<<"L is empty"<<"\n";
        CS2.Topk();
        vector<CS1_CS2::track_Sel> tmp3;
        for(auto x:CS2.kid) {
            // reply->add_kid()->set_dis(x.first);
            // reply->add_kid()->set_lable(x.second);
            CS1_CS2::track_Sel tmp2;
            tmp2.set_dis(x.first);
            tmp2.set_lable(x.second);
            tmp3.push_back(tmp2);
        }
        reply->mutable_kid()->CopyFrom({tmp3.begin(),tmp3.end()});
        // std::cout<<"kid"<<"\n";
        // if(CS2.kid.empty()) cout<<"kid is empty"<<"\n";
        CS2.kid.clear();
        return Status::OK;
    }

    Status SeedMessage(ServerContext* context, const CS1_CS2::SHE_pk* request,
                        google::protobuf::Empty* reply) override {
        CS2.sk.first = Integer(request->sk1().c_str());
        CS2.sk.second = Integer(request->sk2().c_str());
        // std::cout<<"SHE_sk = "<<CS2.sk.first<<"\n"<<CS2.sk.second<<"\n";
        return Status::OK;
    }
};

void RunServer() {
  std::string server_address("0.0.0.0:50053");
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

int main() {
    RunServer();
    return 0;
}