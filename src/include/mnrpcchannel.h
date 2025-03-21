#ifndef MNRPC_MNRPCCHANNEL_H
#define MNRPC_MNRPCCHANNEL_H

#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>


namespace MINIRPC {

    class cMnrpcChannel : public google::protobuf::RpcChannel {
    public:
        // 所有通过stub代理对象调用的rpc方法，都运行到这，统一做rpc方法调用数据序列化和网络发送
        //All RPC methods called through stub proxy objects are run here, and the data serialization and network transmission of RPC method calls are unified
        virtual void CallMethod(const google::protobuf::MethodDescriptor *method,
                                google::protobuf::RpcController *controller, const google::protobuf::Message *request,
                                google::protobuf::Message *response, google::protobuf::Closure *done);
    };
}
#endif //MNRPC_MNRPCCHANNEL_H
