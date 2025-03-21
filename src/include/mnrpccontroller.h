#ifndef MNRPC_MNRPCCONTROLLER_H
#define MNRPC_MNRPCCONTROLLER_H

#include <google/protobuf/service.h>
#include <string>

namespace MINIRPC {

    class cMnrpcController : public google::protobuf::RpcController {

    public:
        cMnrpcController();

        void Reset();

        bool Failed() const;

        std::string ErrorText() const;

        void SetFailed(const std::string &reason);

        void StartCancel();

        bool IsCanceled() const;

        void NotifyOnCancel(google::protobuf::Closure *callback);

    private:
        bool m_failed;
        std::string m_errtext;
    };

}
#endif //MNRPC_MNRPCCONTROLLER_H
