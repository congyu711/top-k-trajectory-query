#include "main.cc"
#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/health_check_service_interface.h>

#include "./cmake/build/CS1.grpc.pb.h"
#include "./cmake/build/CS1_and_CS2.grpc.pb.h"

CloudServer1 CS;

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
            request.set_k(CS.k);
            // std::cout<<"k = "<<CS.k<<"\n";
            for(auto x:CS.ESD) {
                request.add_esd(Integer_to_string(x));
            }
            // std::cout<<"ESD"<<"\n";
            // if(CS.ESD.empty()) cout<<"ESD is empty"<<"\n";
            for(auto x:CS.L) {
                request.add_timelist(x);
            }
            // std::cout<<"timeList"<<"\n";
            // if(CS.L.empty()) cout<<"L is empty"<<"\n";
            CS1_CS2::ExactResult reply;
            ClientContext context;
            Status status = stub_->exactQuery(&context,request,&reply);

            if(status.ok()) {
                vector<std::pair<double, int>> tmp;
                for(auto x:reply.kid()) {
                    tmp.push_back(make_pair(x.dis(),x.lable()));
                }
                CS.kid = tmp;
                // std::cout<<"kid"<<"\n";
                // if(CS.kid.empty()) cout<<"kid is empty"<<"\n";
                return "OK ";
            }
            else {
                return "grpc error";
            }
        }

    private:
        std::unique_ptr<CS1_CS2::CS1CS2_Greeter::Stub> stub_;
};

class GreeterServiceImpl final : public CS1::QUCS1_Greeter::Service{
    grpc::Status PublicKey(ServerContext* context, const CS1::PublicKeyRequest* request,
                CS1::PublicKeyReply* reply) override {
        CS.QU_key_publicKey = Integer(request->name().c_str());
        // std::cout<<"set_QUpublicKey = "<<CS.QU_key_publicKey<<"\n";
        reply->set_message(Integer_to_string(CS.CS1_key.publickey));
        return Status::OK;
    }

    grpc::Status SeedConversionKey(ServerContext* context, const CS1::conversion_key* request,
                google::protobuf::Empty* reply) override {
        CS.conversion_key = Integer(request->key().c_str());
        CS.mess.second = Capsule(Integer(request->encryptedid().very_val().x1().c_str()),
        Integer(request->encryptedid().very_val().x2().c_str()),
        Integer(request->encryptedid().very_val().x3().c_str()));
        vector<string> tmp;
        for(auto x:request->encryptedid().enc_val()) {
            tmp.push_back(x);
        }
        CS.mess.first = tmp;
        // if(CS.mess.first.empty()) std::cout<< "mess"<<"\n";
        // std::cout<<"CS.conversion_key = "<<CS.conversion_key<<"\n";
        return Status::OK;
    }

    grpc::Status SeedMessage(ServerContext* context, const CS1::msg* request,
                google::protobuf::Empty* reply) override {
        // CS.conversion_key = Integer(request->rk().c_str());
        // std::cout<<"CS.conversion_key = "<<CS.conversion_key<<"\n";
        for(auto x:request->mapping_table()) {
            vector<string> tmp;
            for(auto y:x.second.table_content()) {
                tmp.push_back(y);
            }
            CS.dict[x.first] = tmp;
        }
        // std::cout<< "mapping_table"<<"\n";
        // if(CS.dict.empty()) cout<<"mapping_table is empty"<<"\n";
        for(auto x:request->encodinglist()) {
            vector<pair<double,string>> tmp;
            for(auto y:x.encoded()) {
                tmp.push_back(make_pair(y.time(),y.hil()));
            }
            CS.encodingList.push_back(tmp);
        }
        // std::cout<< "encodingList"<<"\n";
        // if(CS.encodingList.empty()) cout<<"encodingList is empty"<<"\n";
        // CS.mess.second = Capsule(Integer(request->encryptedid().very_val().x1().c_str()),
        // Integer(request->encryptedid().very_val().x2().c_str()),
        // Integer(request->encryptedid().very_val().x3().c_str()));
        // for(auto x:request->encryptedid().enc_val()) {
        //     CS.mess.first.push_back(x);
        // }
        // std::cout<< "mess"<<"\n";
        // reply->~Empty();
        return Status::OK;
    }

    grpc::Status QUSearch(ServerContext* context, const CS1::QURequest* request,
                CS1::QUReply* reply) override {
        CS.k = request->k();
        // std::cout<<"k = "<<CS.k<<"\n";
        vector<std::pair<double, std::string>> tmp;
        for(auto x:request->qu_encs()) {
            tmp.push_back(make_pair(x.t(),x.hpoint()));
        }
        // std::cout<<"qu_enc"<<"\n";
        CS.qu_Enc = tmp;
        for(auto x:CS.qu_Enc) {
            printstring(x.second);
        }
        // if(CS.qu_Enc.empty()) cout<<"qu_enc is empty"<<"\n";
        CS.Decrypt_Query();
        CS.Compute_ApproximateDistance();
        CS.firstTopK();
        CS.Compute_ESD();
        // std::cout<<"seed message to CS2"<<"\n";
        CS1CS2_Client greeter(grpc::CreateChannel("localhost:50053",grpc::InsecureChannelCredentials()));
        std::string reponse = greeter.exactQuery();
        // std::cout << "Greeter1 received: " << reponse << std::endl;
        CS.Encrypt_Result();
        CS.Pre_ReEncryption();
        reply->mutable_cap()->set_e(Integer_to_string(CS.cap.E));
        reply->mutable_cap()->set_v(Integer_to_string(CS.cap.V));
        reply->mutable_cap()->set_s(Integer_to_string(CS.cap.s));
        // std::cout<<"reply from CS2"<<"\n";
        for(auto x:CS.ID_Topk) {
            reply->add_kid(x);
        }
        // std::cout<<"kid"<<"\n";
        vector<CS1::QUReply_Enc_Result> tmp3;
        CS1::QUReply_Enc_Result tmp4;
        // if(CS.Enc_result.empty()) cout<<"Enc_result is empty"<<"\n";
        for(auto x:CS.Enc_result) {
            vector<CS1::QURequest_qu_Enc> tmp2;
            for(auto y:x) {
                CS1::QURequest::qu_Enc tmp1;
                //reply->add_enc_results()->add_query_result()->set_t(y.first);
                // reply->add_enc_results()->add_query_result()->set_hpoint(y.second);
                tmp1.set_t(y.first);
                tmp1.set_hpoint(y.second);
                tmp2.push_back(tmp1);
            }
            tmp4.mutable_query_result()->CopyFrom({tmp2.begin(),tmp2.end()});
            tmp3.push_back(tmp4);
        }
        reply->mutable_enc_results()->CopyFrom({tmp3.begin(),tmp3.end()});
        // if(tmp3.empty()) cout<<"tmp3 is empty"<<"\n";
        // std::cout<<"Enc_result"<<"\n";
        for(auto x:CS.K) {
            reply->add_d(x.first);
        }
        CS.K.clear();
        return Status::OK;
    }
};

void RunServer() {
  std::string server_address("0.0.0.0:50052");
  GreeterServiceImpl service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;
  builder.SetMaxMessageSize(0x7fffffff);
  builder.SetMaxReceiveMessageSize(0x7fffffff);
  builder.SetMaxSendMessageSize(0x7fffffff);
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