#include "main.cc"
#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/health_check_service_interface.h>

#include "./cmake/build/DO_and_CS1.grpc.pb.h"
#include "./cmake/build/QU_and_CS1.grpc.pb.h"
#include "./cmake/build/CS1_and_CS2.grpc.pb.h"

CloudServer1 CS1;

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::Channel;
using grpc::ClientContext;

class CS1CS2_Client {
    public:
        CS1CS2_Client(std::shared_ptr<Channel> channel)
        : stub_(CS1_CS2::CS1CS2_Greeter::NewStub(channel)) {}

        std::string exactQuery() {
            CS1_CS2::PreResults request;
            request.set_k(CS1.k);
            for(auto x:CS1.ESD) {
                request.add_esd(Integer_to_string(x));
            }
            for(auto x:CS1.L) {
                request.add_timelist(x);
            }

            CS1_CS2::ExactResult reply;
            ClientContext context;
            Status status = stub_->exactQuery(&context,request,&reply);

            if(status.ok()) {
                for(auto x:reply.kid()) {
                    CS1.kid.push_back(make_pair(x.dis(),x.lable()));
                }
                return "OK ";
            }
            else {
                return "grpc error";
            }
        }

    private:
        std::unique_ptr<CS1_CS2::CS1CS2_Greeter::Stub> stub_;
};

class GreeterServiceImpl final : public QU_and_CS1::QUCS1_Greeter::Service,public DO_CS1::DOCS1_Greeter::Service {
    grpc::Status PublicKey(ServerContext* context, const QU_and_CS1::PublicKeyRequest* request,
                QU_and_CS1::PublicKeyReply* reply) override {
        CS1.QU_key_publicKey = Integer(request->name().c_str());
        reply->set_message(Integer_to_string(CS1.CS1_key.publickey));
        return Status::OK;
    }

    grpc::Status SeedMessage(ServerContext* context, const DO_CS1::msg* request,
                google::protobuf::Empty* reply) {
        CS1.conversion_key = Integer(request->rk().c_str());
        for(auto x:request->mapping_table()) {
            vector<string> tmp;
            for(auto y:x.second.table_content()) {
                tmp.push_back(y);
            }
            CS1.dict[x.first] = tmp;
        }
        for(auto x:request->encodinglist()) {
            vector<pair<double,string>> tmp;
            for(auto y:x.encoded()) {
                tmp.push_back(make_pair(y.time(),y.hil()));
            }
            CS1.encodingList.push_back(tmp);
        }
        CS1.mess.second = Capsule(Integer(request->encryptedid().very_val().x1().c_str()),
        Integer(request->encryptedid().very_val().x2().c_str()),
        Integer(request->encryptedid().very_val().x3().c_str()));
        for(auto x:request->encryptedid().enc_val()) {
            CS1.mess.first.push_back(x);
        }
        reply->~Empty();
        return Status::OK;
    }

    grpc::Status QUSearch(ServerContext* context, const QU_and_CS1::QURequest* request,
                QU_and_CS1::QUReply* reply) override {
        CS1.k = request->k();
        for(auto x:request->qu_encs()) {
            CS1.qu_Enc.push_back(make_pair(x.t(),x.hpoint()));
        }
        CS1.Decrypt_Query();
        CS1.Compute_ApproximateDistance();
        CS1.firstTopK();
        CS1.Compute_ESD();
        CS1CS2_Client greeter(grpc::CreateChannel("localhost:50053",grpc::InsecureChannelCredentials()));
        greeter.exactQuery();
        CS1.Encrypt_Result();
        CS1.Pre_ReEncryption();
        reply->mutable_cap()->set_e(Integer_to_string(CS1.cap.E));
        reply->mutable_cap()->set_v(Integer_to_string(CS1.cap.V));
        reply->mutable_cap()->set_s(Integer_to_string(CS1.cap.s));
        for(auto x:CS1.ID_Topk) {
            reply->add_kid(x);
        }
        for(auto x:CS1.Enc_result) {
            for(auto y:x) {
                reply->add_enc_results()->add_query_result()->set_t(y.first);
                reply->add_enc_results()->add_query_result()->set_hpoint(y.second);
            }
        }
        for(auto x:CS1.K) {
            reply->add_d(x.first);
        }
        return Status::OK;
    }
};

void RunServer() {
  std::string server_address("0.0.0.0:50052");
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