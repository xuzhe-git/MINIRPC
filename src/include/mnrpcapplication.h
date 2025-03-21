//Mnrpc framework initialization class

#ifndef MNRPC_MNRPCAPPLICATION_H
#define MNRPC_MNRPCAPPLICATION_H
#include "mnrpcconfig.h"


namespace MINIRPC {

    class cMnrpcApplication {

    public:
        static void init(int argc, char *argv[]);

        static cMnrpcApplication &GetInstance();

        static cmnrpcconfig &GetConfig();

    private:
        static cmnrpcconfig m_config;

        cMnrpcApplication();

        cMnrpcApplication(const cMnrpcApplication &) = delete;

        cMnrpcApplication(cMnrpcApplication &&) = delete;

        cMnrpcApplication &operator=(cMnrpcApplication &) = delete;


    };
}
#endif //MYRPC_MNRPCAPPLICATION_H
