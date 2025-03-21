#include "mnrpcchannel.h"
#include <string>
#include "rpcheader.pb.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <error.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "mnrpcapplication.h"
#include "mnrpccontroller.h"
#include "zookeeperutil.h"

namespace MINIRPC {

/*
 * header_size + service_name + method_name + args_size + args
 */
    void
    cMnrpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                              google::protobuf::RpcController *controller,
                              const google::protobuf::Message *request, google::protobuf::Message *response,
                              google::protobuf::Closure *done) {
        const google::protobuf::ServiceDescriptor *sd = method->service();
        std::string service_name = sd->name();
        std::string method_name = method->name();

        // 获取参数的序列化字符串长度
        uint32_t args_size = 0;
        std::string args_str;
        if (request->SerializePartialToString(&args_str)) {
            args_size = args_str.size();
        } else {
            //std::cout<<"serialize request error!"<<std::endl;
            controller->SetFailed("serialize request error!");
            return;
        }

        //定义rpcheader
        mnrpc::RpcHeader rpcheader;
        rpcheader.set_service_name(service_name);
        rpcheader.set_method_name(method_name);
        rpcheader.set_args_size(args_size);

        uint32_t headersize = 0;
        std::string rpc_header_str;
        if (rpcheader.SerializePartialToString(&rpc_header_str)) {
            headersize = rpc_header_str.size();
        } else {
            //std::cout<<"serialize rpc header error!"<<std::endl;
            controller->SetFailed("serialize rpc header error!");
            return;
        }

        // 组织待发送的rpc请求的字符串
        std::string send_rpc_str;
        send_rpc_str.insert(0, std::string((char *) &headersize, 4));
        send_rpc_str += rpc_header_str;
        send_rpc_str += args_str;

        // 打印调试信息
        /*
        std::cout << "================================================" << std::endl;
        std::cout << "header_size: " << headersize << std::endl;
        std::cout << "service_name: " << service_name << std::endl;
        std::cout << "method_name: " << method_name << std::endl;
        std::cout << "args_size: " << args_size << std::endl;
        std::cout << "args: " << args_str << std::endl;
        std::cout << "================================================" << std::endl;
        */

        // tcp
        int clientfd = socket(AF_INET, SOCK_STREAM, 0);
        if (clientfd == -1) {
            //std::cout<<"create socket error:"<<errno<<std::endl;
            char errtext[512] = {0};
            sprintf(errtext, "create socket error:%d", errno);
            controller->SetFailed(errtext);
            return;
        }

        //std::string ip = cMnrpcApplication::GetInstance().GetConfig().Load("rpcserverip");
        //uint16_t port = atoi(cMnrpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
        cZookeeper zkclient;
        zkclient.start();
        std::string method_path = "/" + service_name + "/" + method_name;
        std::string host_data = zkclient.GetData(method_path.c_str());
        if (host_data == "") {
            controller->SetFailed(method_path + " is not exist!");
            return;
        }
        int idx = host_data.find(":");
        if (idx == -1) {
            controller->SetFailed(method_path + " address is invalid!");
            return;
        }
        std::string ip = host_data.substr(0, idx);
        uint16_t port = atoi(host_data.substr(idx + 1, host_data.size() - idx).c_str());

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

        // 发起连接
        if (connect(clientfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
            //std::cout<<"connect error:"<<errno<<std::endl;
            close(clientfd);
            char errtext[512] = {0};
            sprintf(errtext, "connect error:%d", errno);
            controller->SetFailed(errtext);
            return;
        }
        //std::cout << "connect success" << std::endl;
        if (send(clientfd, send_rpc_str.c_str(), send_rpc_str.size(), 0) == -1) {
            //std::cout<<"send error:"<<errno<<std::endl;
            close(clientfd);
            char errtext[512] = {0};
            sprintf(errtext, "send error:%d", errno);
            controller->SetFailed(errtext);
            return;
        }
        //std::cout<<"send success"<<std::endl;

        /*
        int error = 0;
        socklen_t len = sizeof(error);
        if (getsockopt(clientfd, SOL_SOCKET, SO_ERROR, &error, &len) == -1) {
            //std::cout << "getsockopt error: " << errno << std::endl;
            close(clientfd);
            return;
        }
        if (error != 0) {
            //std::cout << "Socket has an error: " << error << std::endl;
            close(clientfd);
            return;
        }

        // 检查连接是否仍然正常
        if (getsockopt(clientfd, SOL_SOCKET, SO_ERROR, &error, &len) == -1) {
            std::cout << "Error checking socket status: " << errno << std::endl;
            close(clientfd);
            return;
        }
        if (error != 0) {
            std::cout << "Connection is no longer valid: " << error << std::endl;
            close(clientfd);
            return;
        }
         */
        // 接收rpc请求的响应值
        char recv_buffer[1024] = {0};

        ssize_t recv_size = recv(clientfd, recv_buffer, 1024 - 1, 0);
        if (recv_size == -1) {
            //std::cerr << "Failed to receive message from server" << std::endl;
            controller->SetFailed("Failed to receive message from server");
            close(clientfd);
            return;
        }

        // 反序列化
        //std::string response_str(recv_buffer, 0, recv_size);
        if (!response->ParseFromArray(recv_buffer, recv_size)) {
            //std::cout<<"parse error! response_str:"<<recv_buffer<<std::endl;
            char errtext[2048] = {0};
            sprintf(errtext, "parse error! response_str:%s", recv_buffer);
            controller->SetFailed(errtext);
            close(clientfd);
            return;
        }
        close(clientfd);
    }
}