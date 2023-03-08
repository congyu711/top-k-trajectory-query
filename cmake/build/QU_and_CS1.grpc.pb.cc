// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: QU_and_CS1.proto

#include "QU_and_CS1.pb.h"
#include "QU_and_CS1.grpc.pb.h"

#include <functional>
#include <grpcpp/support/async_stream.h>
#include <grpcpp/support/async_unary_call.h>
#include <grpcpp/impl/channel_interface.h>
#include <grpcpp/impl/client_unary_call.h>
#include <grpcpp/support/client_callback.h>
#include <grpcpp/support/message_allocator.h>
#include <grpcpp/support/method_handler.h>
#include <grpcpp/impl/rpc_service_method.h>
#include <grpcpp/support/server_callback.h>
#include <grpcpp/impl/server_callback_handlers.h>
#include <grpcpp/server_context.h>
#include <grpcpp/impl/service_type.h>
#include <grpcpp/support/sync_stream.h>
namespace QU_and_CS1 {

static const char* Greeter_method_names[] = {
  "/QU_and_CS1.Greeter/PublicKey",
  "/QU_and_CS1.Greeter/QUSearch",
};

std::unique_ptr< Greeter::Stub> Greeter::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< Greeter::Stub> stub(new Greeter::Stub(channel, options));
  return stub;
}

Greeter::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_PublicKey_(Greeter_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_QUSearch_(Greeter_method_names[1], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status Greeter::Stub::PublicKey(::grpc::ClientContext* context, const ::QU_and_CS1::PublicKeyRequest& request, ::QU_and_CS1::PublicKeyReply* response) {
  return ::grpc::internal::BlockingUnaryCall< ::QU_and_CS1::PublicKeyRequest, ::QU_and_CS1::PublicKeyReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_PublicKey_, context, request, response);
}

void Greeter::Stub::async::PublicKey(::grpc::ClientContext* context, const ::QU_and_CS1::PublicKeyRequest* request, ::QU_and_CS1::PublicKeyReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::QU_and_CS1::PublicKeyRequest, ::QU_and_CS1::PublicKeyReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_PublicKey_, context, request, response, std::move(f));
}

void Greeter::Stub::async::PublicKey(::grpc::ClientContext* context, const ::QU_and_CS1::PublicKeyRequest* request, ::QU_and_CS1::PublicKeyReply* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_PublicKey_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::QU_and_CS1::PublicKeyReply>* Greeter::Stub::PrepareAsyncPublicKeyRaw(::grpc::ClientContext* context, const ::QU_and_CS1::PublicKeyRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::QU_and_CS1::PublicKeyReply, ::QU_and_CS1::PublicKeyRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_PublicKey_, context, request);
}

::grpc::ClientAsyncResponseReader< ::QU_and_CS1::PublicKeyReply>* Greeter::Stub::AsyncPublicKeyRaw(::grpc::ClientContext* context, const ::QU_and_CS1::PublicKeyRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncPublicKeyRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status Greeter::Stub::QUSearch(::grpc::ClientContext* context, const ::QU_and_CS1::QURequest& request, ::QU_and_CS1::QUReply* response) {
  return ::grpc::internal::BlockingUnaryCall< ::QU_and_CS1::QURequest, ::QU_and_CS1::QUReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_QUSearch_, context, request, response);
}

void Greeter::Stub::async::QUSearch(::grpc::ClientContext* context, const ::QU_and_CS1::QURequest* request, ::QU_and_CS1::QUReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::QU_and_CS1::QURequest, ::QU_and_CS1::QUReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_QUSearch_, context, request, response, std::move(f));
}

void Greeter::Stub::async::QUSearch(::grpc::ClientContext* context, const ::QU_and_CS1::QURequest* request, ::QU_and_CS1::QUReply* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_QUSearch_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::QU_and_CS1::QUReply>* Greeter::Stub::PrepareAsyncQUSearchRaw(::grpc::ClientContext* context, const ::QU_and_CS1::QURequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::QU_and_CS1::QUReply, ::QU_and_CS1::QURequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_QUSearch_, context, request);
}

::grpc::ClientAsyncResponseReader< ::QU_and_CS1::QUReply>* Greeter::Stub::AsyncQUSearchRaw(::grpc::ClientContext* context, const ::QU_and_CS1::QURequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncQUSearchRaw(context, request, cq);
  result->StartCall();
  return result;
}

Greeter::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Greeter_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Greeter::Service, ::QU_and_CS1::PublicKeyRequest, ::QU_and_CS1::PublicKeyReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](Greeter::Service* service,
             ::grpc::ServerContext* ctx,
             const ::QU_and_CS1::PublicKeyRequest* req,
             ::QU_and_CS1::PublicKeyReply* resp) {
               return service->PublicKey(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Greeter_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Greeter::Service, ::QU_and_CS1::QURequest, ::QU_and_CS1::QUReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](Greeter::Service* service,
             ::grpc::ServerContext* ctx,
             const ::QU_and_CS1::QURequest* req,
             ::QU_and_CS1::QUReply* resp) {
               return service->QUSearch(ctx, req, resp);
             }, this)));
}

Greeter::Service::~Service() {
}

::grpc::Status Greeter::Service::PublicKey(::grpc::ServerContext* context, const ::QU_and_CS1::PublicKeyRequest* request, ::QU_and_CS1::PublicKeyReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Greeter::Service::QUSearch(::grpc::ServerContext* context, const ::QU_and_CS1::QURequest* request, ::QU_and_CS1::QUReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace QU_and_CS1

