#include "mnrpccontroller.h"

namespace MINIRPC {

    cMnrpcController::cMnrpcController() {
        m_failed = false;
        m_errtext = "";
    }

    void cMnrpcController::Reset() {
        m_failed = false;
        m_errtext = "";
    }

    bool cMnrpcController::Failed() const {
        return m_failed;
    }

    std::string cMnrpcController::ErrorText() const {
        return m_errtext;
    }

    void cMnrpcController::SetFailed(const std::string &reason) {
        m_failed = true;
        m_errtext = reason;
    }

    void cMnrpcController::StartCancel() {}

    bool cMnrpcController::IsCanceled() const { return false; }

    void cMnrpcController::NotifyOnCancel(google::protobuf::Closure *callback) {}
}

