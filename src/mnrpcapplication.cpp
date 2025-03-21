#include "mnrpcapplication.h"
#include <iostream>
#include <unistd.h>
#include <string>

namespace MINIRPC {

    cmnrpcconfig cMnrpcApplication::m_config;

    void ShowArgsHelp() {
        std::cout << "format: command -i <configfile>" << std::endl;
    }


    void cMnrpcApplication::init(int argc, char *argv[]) {
        if (argc < 2) {
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        }

        int c = 0;
        std::string config_file;
        while ((c = getopt(argc, argv, "i:")) != -1) {
            switch (c) {
                case 'i':
                    config_file = optarg;
                    break;
                case '?':
                    //std::cout<<"invalid args!"<<std::endl;
                    ShowArgsHelp();
                    exit(EXIT_FAILURE);
                case ':':
                    //std::cout<<"need <configfile>!"<<std::endl;
                    ShowArgsHelp();
                    exit(EXIT_FAILURE);
                default:
                    break;
            }
        }

        // 加载配置文件  rpcserver_ip =  rpcserver_port =  zookeeper_ip =  zookeeper_port =
        m_config.loadconfigfile(config_file.c_str());

        /*
        std::cout<<"rpcserverip:"<<m_config.Load("rpcserverip")<<std::endl;
        std::cout<<"rpcserverport:"<<m_config.Load("rpcserverport")<<std::endl;
        std::cout<<"zookeeperip:"<<m_config.Load("zookeeperip")<<std::endl;
        std::cout<<"zookeeperport:"<<m_config.Load("zookeeperport")<<std::endl;
         */
    }

    cMnrpcApplication &cMnrpcApplication::GetInstance() {
        static cMnrpcApplication MnrpcApplication;
        return MnrpcApplication;
    }

    cMnrpcApplication::cMnrpcApplication() {

    }

    cmnrpcconfig &cMnrpcApplication::GetConfig() {
        return m_config;
    }
}