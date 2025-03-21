#include "rpcprovider.h"
#include "rpcheader.pb.h"
#include "logger.h"
#include "zookeeperutil.h"

namespace MINIRPC {


    void cRpcProvider::NotifyService(google::protobuf::Service *service) {

        ServiceInfo service_info;


        const google::protobuf::ServiceDescriptor *pserviceDesc = service->GetDescriptor();

        std::string service_name = pserviceDesc->name(); //类名

        int methodCnt = pserviceDesc->method_count();
        LOG_INFO("service_name:%s", service_name.c_str());

        for (int i = 0; i < methodCnt; ++i) {
            // 获取服务对象指定小标的服务方法描述
            const google::protobuf::MethodDescriptor *pmethodDesc = pserviceDesc->method(i);
            std::string method_name = pmethodDesc->name();
            service_info.m_methodMap.insert({method_name, pmethodDesc});
            LOG_INFO("method_name:%s", method_name.c_str());
        }
        service_info.m_service = service;
        m_serviceMap.insert({service_name, service_info});

        /*
        for(auto it : m_serviceMap){
            std::cout<<it.first<<std::endl;
            for(auto it2:it.second.m_methodMap){
                std::cout<<it2.first<<std::endl;
            }
        }
        */
    }

    void cRpcProvider::Run() {
        std::string ip = cMnrpcApplication::GetInstance().GetConfig().Load("rpcserverip");
        uint16_t port = atoi(cMnrpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
        LibeventInetAddress address(ip, port);

        LibeventTcpServer server(&m_eventLoop, address, "RPCprovider");

        // 绑定连接回调和消息读写回调方法
        server.setConnectionCallback(std::bind(&cRpcProvider::Onconnection, this, std::placeholders::_1));
        server.setMessageCallback(
                std::bind(&cRpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2,
                          std::placeholders::_3));

        // 设置线程数量
        server.setThreadNum(4);

        // 把当前rpc节点上要发布的服务全部注册到zk上
        cZookeeper zkclient;
        zkclient.start();
        for (auto &sp: m_serviceMap) {
            std::string service_path = "/" + sp.first;
            zkclient.create(service_path.c_str(), nullptr, 0);
            for (auto &mp: sp.second.m_methodMap) {
                std::string method_path = service_path + "/" + mp.first;
                char method_path_data[128] = {0};
                sprintf(method_path_data, "%s:%d", ip.c_str(), port);
                zkclient.create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
            }
        }


        std::cout << "RpcProvider start service at ip:" << ip << " port:" << port + 1 << std::endl;
        // 启动
        server.start();
        m_eventLoop.loop();
        //std::unique_ptr<LibeventTcpServer> m_tcpserverPtr;
    }

    void cRpcProvider::Onconnection(const std::shared_ptr<LibeventTcpConnection> &conn) {
        if (conn->connected()) {
            std::cout << "New connection from " << conn->peerAddress().toIpPort() << std::endl;
        } else {
            std::cout << "Connection closed: " << conn->peerAddress().toIpPort() << std::endl;
        }
    }

// 已经连接的用户的读写事件回调， 远程有一个rpc调用服务，该方法响应
// header_name 4 + header_str + args_str
    void
    cRpcProvider::OnMessage(const std::shared_ptr<LibeventTcpConnection> &conn, struct evbuffer *buffer, size_t len) {
        // 接收的字符流
        /*
        char data[len + 1];
        evbuffer_remove(buffer, data, len+1);
        std::string recv_buf(data);
        */
        // 接收的字符流
        std::string recv_buf;
        while (evbuffer_get_length(buffer) > 0) {
            char data[1024];
            size_t read_len = evbuffer_remove(buffer, data, sizeof(data));
            recv_buf.append(data, read_len);
        }

        //std::cout << "Received data length: " << recv_buf.size() << std::endl;


        // 从字符流中读取前四个
        uint32_t header_size = 0;
        recv_buf.copy((char *) &header_size, 4, 0);
        // recv_buf.size()  == 1 ?

        // 从header_size读取数据头的原始字符流
        std::string rpc_header_str = recv_buf.substr(4, header_size);

        mnrpc::RpcHeader rpcHeader;
        std::string service_name;
        std::string method_name;
        uint32_t args_size;
        if (rpcHeader.ParseFromString(rpc_header_str)) {
            // 数据头反序列化成功
            service_name = rpcHeader.service_name();
            method_name = rpcHeader.method_name();
            args_size = rpcHeader.args_size();
        } else {
            // 数据头反序列化失=失败
            //std::cout << "rpc_header_str:" << rpc_header_str << " parse error" << std::endl;
            return;
        }

        // 获取参数
        std::string args_str = recv_buf.substr(4 + header_size, args_size);


        // 打印调试信息
        /*
        std::cout << "================================================" << std::endl;
        std::cout << "header_size: " << header_size << std::endl;
        std::cout << "service_name: " << service_name << std::endl;
        std::cout << "method_name: " << method_name << std::endl;
        std::cout << "args_size: " << args_size << std::endl;
        std::cout << "args: " << args_str << std::endl;
        std::cout << "================================================" << std::endl;
        */
        // 获取service对象和method对象
        auto it = m_serviceMap.find(service_name);
        if (it == m_serviceMap.end()) {
            //std::cout << service_name << " is not exist" << std::endl;
            return;
        }

        auto mit = it->second.m_methodMap.find(method_name);
        if (mit == it->second.m_methodMap.end()) {
            //std::cout << service_name << ":" << method_name << " is not exist" << std::endl;
            return;
        }

        google::protobuf::Service *service = it->second.m_service;
        const google::protobuf::MethodDescriptor *method = mit->second;

        // 生成rpc方法调用的request和响应response参数
        google::protobuf::Message *request = service->GetRequestPrototype(method).New();
        if (!request->ParseFromString(args_str)) {
            //std::cout << " request parse error, content:" << args_str << std::endl;
            return;
        }
        google::protobuf::Message *response = service->GetResponsePrototype(method).New();

        // 给method方法的调用绑定一个回调函数
        google::protobuf::Closure *done =
                google::protobuf::NewCallback<cRpcProvider,
                        const std::shared_ptr<LibeventTcpConnection> &,
                        google::protobuf::Message *>
                        (this, &cRpcProvider::SendRpcResponse, conn, response);

        // 在框架上根据远端rpc请求，调用当前rpc上的节点发布的方法
        service->CallMethod(method, nullptr, request, response, done);

    }

// 用于序列化和网络发送

    void cRpcProvider::SendRpcResponse(const std::shared_ptr<LibeventTcpConnection> &conn,
                                       google::protobuf::Message *response) {
        std::string response_str;
        if (response->SerializePartialToString(&response_str)) {
            // 序列化成功后，通过网络方法把rpc调用结果返回给调用方
            if (conn->connected()) {
                // 序列化成功后，通过网络方法把rpc调用结果返回给调用方
                //std::cout << "Serialized response length: " << response_str.length() << std::endl; // 添加日志
                conn->send(response_str);
                //std::cout << "Response sent successfully" << std::endl;
            } else {
                //std::cout << "Connection is not in a valid state. Cannot send response." << std::endl;
            }
            //conn->connectDestroyed(); //由rpcprovider主动断开连接
        } else {
            std::cout << "serialize response_str error!" << std::endl;
        }
    }

}