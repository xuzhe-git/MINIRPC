// 框架负责发布rpc的网络对象类
//The framework is responsible for publishing RPC network objects
#ifndef MNRPC_RPCPROVIDER_H
#define MNRPC_RPCPROVIDER_H

#include "google/protobuf/service.h"
#include <memory>
#include "libevent_tcp_server.h"
#include "libevent_event_loop.h"
#include "libevent_inet_address.h"
#include "libevent_tcp_connection.h"
#include <string>
#include "mnrpcapplication.h"
#include <google/protobuf/descriptor.h>
#include <unordered_map>
#include <event2/buffer.h>

namespace MINIRPC {


    class cRpcProvider {
    public:
        // 框架提供给外部使用的，可以发布rpc方法的函数接口
        //The framework provides a function interface for external use that can publish RPC methods
        void NotifyService(google::protobuf::Service *service);

        // 启动rpc服务节点
        //Start RPC service node
        void Run();

    private:
        LibeventEventLoop m_eventLoop;

        struct ServiceInfo {
            google::protobuf::Service *m_service;
            std::unordered_map<std::string, const google::protobuf::MethodDescriptor *> m_methodMap;
        };

        //存储注册成功的服务对象和信息  类名  与  描述
        //Store the name and description of the service object and information class registered successfully
        std::unordered_map<std::string, ServiceInfo> m_serviceMap;

        void Onconnection(const std::shared_ptr<LibeventTcpConnection> &conn);

        void OnMessage(const std::shared_ptr<LibeventTcpConnection> &conn, struct evbuffer *buffer, size_t len);

        // Closure回调操作 用于序列化rpc的响应和网络
        //Closure callback operation is used to serialize RPC response and network
        void SendRpcResponse(const std::shared_ptr<LibeventTcpConnection> &conn, google::protobuf::Message *);


    };
}

#endif //MNRPC_RPCPROVIDER_H
