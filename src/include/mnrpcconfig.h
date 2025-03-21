#ifndef MNRPC_MNRPCCONFIG_H
#define MNRPC_MNRPCCONFIG_H

#include <unordered_map>
#include <string>

namespace MINIRPC {

    class cmnrpcconfig {

    public:
        void loadconfigfile(const char *config_file);

        // 查询配置项消息
        // Query configuration item message
        std::string Load(const std::string &key) const;

    private:
        std::unordered_map<std::string, std::string> m_configMap;

        // 去掉字符串前后的空格
        // Remove the space before and after the string
        void Trim(std::string &src_buf);

    };
}
#endif //MNRPC_MNRPCCONFIG_H
