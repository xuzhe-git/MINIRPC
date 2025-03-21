// Service publisher test procedure

#include <iostream>
#include <string>
#include "user.pb.h"
#include "mnrpcapplication.h"
#include "rpcprovider.h"
#include "logger.h"

class cUserService : public fixbug::UserServiceRpc{
public:
    bool Login(std::string name, std::string pwd){
        std::cout<<"doing local service:Login"<<std::endl;
        std::cout<<"name:"<<name<<" "<<"pwd:"<<pwd<<std::endl;
        return true;
    }

    /*
     * 1. caller ===》 Login(LoginRequest) ===》net ===》callee
     * 2. callee ===》Login(LoginRequest) ===》Login rewritten below
     */
    void Login(::google::protobuf::RpcController* controller,
               const ::fixbug::LoginRequest* request,
               ::fixbug::LoginResponse* response,
               ::google::protobuf::Closure* done){
        // 框架上报请求参数，业务获取相应数据做本地业务
        // The framework reports the request parameters, and the business obtains the corresponding data for local business
        std::string name = request->name();
        std::string pwd = request->pwd();

        // 本地业务
        // Local business
        bool login_result = Login(name, pwd);

        fixbug::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_result);

        // 执行回调操作  执行响应消息的序列化与发送
        // Execute callback operation to serialize and send response message
        done->Run();

    }

};

int main(int argc, char *argv[]){

    // 调用框架初始化
    // Call framework initialization
    MINIRPC::cMnrpcApplication::init(argc, argv);
    // 把cUserService对象发布到rpc站点
    // Publish the userservice object to RPC site
    MINIRPC::cRpcProvider provider;
    provider.NotifyService(new cUserService());
    // 启动一个rpc发布节点，进程进入阻塞状态，等待远程的请求
    // Start an RPC publishing node, the process enters the blocking state and waits for remote requests
    provider.Run();
    return 0;
}