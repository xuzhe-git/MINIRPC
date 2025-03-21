// Service call tester

#include <iostream>
#include "mnrpcapplication.h"
#include "user.pb.h"
#include "mnrpcchannel.h"
#include "mnrpccontroller.h"

int main(int argc, char *argv[]){

    // 框架初始化
    // Framework initialization
    MINIRPC::cMnrpcApplication::init(argc, argv);

    // 调用远程发布的rpc方法
    // Call RPC method published remotely
    fixbug::UserServiceRpc_Stub stub(new MINIRPC::cMnrpcChannel());
    fixbug::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");

    fixbug::LoginResponse response;
    MINIRPC::cMnrpcController controller;

    stub.Login(&controller, &request, &response, nullptr); //RpcChannel->RpcChannel::callMethod


    if(controller.Failed()){
        std::cout<<controller.ErrorText()<<std::endl;
    }
    else{
        if(response.result().errcode() == 0){
            std::cout<<"rpc login response:"<<response.success()<<std::endl;
        }
        else{
            std::cout<<"rpc login response error:"<<response.result().errcode()<<std::endl;
        }
    }


    return 0;
}