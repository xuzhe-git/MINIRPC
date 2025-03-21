#include "mnrpcconfig.h"
#include <iostream>
#include <string>

namespace MINIRPC {

    void cmnrpcconfig::loadconfigfile(const char *config_file) {
        FILE *pf = fopen(config_file, "r");
        if (pf == nullptr) {
            std::cout << config_file << " is not exit!" << std::endl;
            exit(EXIT_FAILURE);
        }

        while (!feof(pf)) {
            char buf[512] = {0};
            fgets(buf, 512, pf);
            // 去掉字符串多余空格
            std::string read_buf(buf);
            Trim(read_buf);
            int idx;
            // 判断#注释
            if (read_buf[0] == '#' || read_buf.empty()) {
                continue;
            }
            // 解析配置项
            idx = read_buf.find('=');
            if (idx == -1) {
                // 配置项不合法
                continue;
            }
            std::string key;
            std::string value;
            key = read_buf.substr(0, idx);
            Trim(key);
            int endidx = read_buf.find('\n', idx);
            value = read_buf.substr(idx + 1, endidx - idx - 1);
            endidx = read_buf.find('\r', idx);
            value = read_buf.substr(idx + 1, endidx - idx - 1);
            Trim(value);
            m_configMap.insert({key, value});
        }
        fclose(pf);
    }

    std::string cmnrpcconfig::Load(const std::string &key) const {
        auto it = m_configMap.find(key);
        if (it == m_configMap.end()) {
            return "";
        }
        return it->second;
    }

    void cmnrpcconfig::Trim(std::string &src_buf) {
        int idx = src_buf.find_first_not_of(' ');
        if (idx != -1) {
            // 字符串有空格
            src_buf = src_buf.substr(idx, src_buf.size() - idx);
        }
        // 去掉字符串后面的空格
        idx = src_buf.find_last_not_of(' ');
        if (idx != -1) {
            src_buf = src_buf.substr(0, idx + 1);
        }
    }
}