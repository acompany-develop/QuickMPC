#pragma once
#include <algorithm>  // find
#include <fstream>
#include <iostream>
#include <optional>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>

#include "logging/Logger.hpp"
#include "nlohmann/json.hpp"

struct Url
{
public:
    std::string url, path, protocol, host, port;

    static Url Parse(const std::string &url)
    {
        Url result;

        typedef std::string::const_iterator iterator_t;

        if (url.length() == 0)
        {
            qmpc::Log::throw_with_trace(std::runtime_error("ParseError: urlが空文字です"));
        }

        iterator_t url_end = url.end();

        // protocol
        iterator_t protocol_start = url.begin();
        iterator_t protocol_end = std::find(protocol_start, url_end, ':');
        if (protocol_end == url_end)
        {
            throw std::runtime_error(url + " は正しくないurl形式です");
            qmpc::Log::throw_with_trace(
                std::runtime_error("ParseError: " + url + " は正しくないurl形式です")
            );
        }
        result.protocol = std::string(protocol_start, protocol_end);
        if (result.protocol != "http" && result.protocol != "https")
        {
            qmpc::Log::throw_with_trace(std::runtime_error(
                "ParseError: " + result.protocol
                + " は許可されているプロトコル(http/https)ではありません"
            ));
        }

        protocol_end += 3;

        // host
        iterator_t host_start = protocol_end;
        iterator_t path_start = std::find(host_start, url_end, '/');

        iterator_t host_end =
            std::find(protocol_end, (path_start != url_end) ? path_start : url_end, ':');
        result.host = std::string(host_start, host_end);

        // port
        if ((host_end != url_end) && ((&*(host_end))[0] == ':'))
        {
            host_end += 1;
            iterator_t port_end = path_start;
            result.port = std::string(host_end, port_end);
        }

        // path
        if (path_start != url_end) result.path = std::string(path_start, url_end);

        result.url = url;
        return result;
    }

    std::string getAddress() const { return host + ':' + port; }
};

class Config
{
    std::unordered_map<std::string, std::string> env_map;

    Config()
    {
        const auto log_level_opt = getEnvOptionalString("LOG_LEVEL");
        qmpc::Log::loadLogLevel(log_level_opt);

        party_id = getEnvInt("PARTY_ID");
        n_parties = getEnvInt("N_PARTIES");
        sp_id = getEnvInt("SP_ID");
        mc_to_cc = getEnvUrl("MC_TO_CC");
        cc_to_bts = getEnvUrl("CC_TO_BTS");
        cc_to_bts_token = getEnvString("BTS_TOKEN");
        getshare_time_limit = getEnvOptionalInt("GETSHARE_TIME_LIMIT").value_or(10);
        const std::string port_for_job = getEnvString("PORT_FOR_JOB");
        for (int i = 1; i <= n_parties; i++)
        {
            const std::string party_list_str = "PARTY_LIST" + std::to_string(i);
            ip_addr_map[i] = getEnvUrl(party_list_str.c_str());
        }
        for (auto &value : ip_addr_map)
        {
            int party_id = value.first;
            Url ip_address_for_job = value.second;
            ip_address_for_job.port = port_for_job;
            ip_addr_for_job_map[party_id] = ip_address_for_job;

            QMPC_LOG_INFO(
                "{0} maps to {1} and {2}", party_id, value.second.url, ip_address_for_job.url
            );
        }
    }
    ~Config() {}
    Config(const Config &x) {}
    int getEnvInt(const char *s)
    {
        const char *stage = std::getenv("IS_TEST");
        // bazel testでは環境変数が読めないためbazel test実行時のみ
        // bazel test --test_env=IS_TEST=true
        // のように環境変数IS_TESTを定義する
        if (stage)
        {
            if (env_map.empty())
            {
                std::ifstream ifs("/QuickMPC/config/.env");
                std::string env;
                while (getline(ifs, env))
                {
                    std::stringstream ss{env};
                    std::string key, value;
                    getline(ss, key, '=');
                    getline(ss, value, '=');
                    env_map[key] = value;
                }
            }
            if (env_map.count(std::string(s)))
            {
                return std::stoi(env_map[std::string(s)]);
            }
        }
        else
        {
            const char *tmp = std::getenv(s);
            if (tmp)
            {
                return atoi(tmp);
            }
        }
        qmpc::Log::throw_with_trace(std::runtime_error(std::string(s) + " is not defined"));
    }
    std::string getEnvString(const char *s)
    {
        const char *stage = std::getenv("IS_TEST");
        // testステージのみ環境変数STAGEを定義する
        if (stage)
        {
            if (env_map.count(std::string(s)))
            {
                return env_map[std::string(s)];
            }
        }
        else
        {
            const char *tmp = std::getenv(s);
            if (tmp)
            {
                return std::string(tmp);
            }
        }
        qmpc::Log::throw_with_trace(std::runtime_error(std::string(s) + " is not defined"));
    }
    std::optional<std::string> getEnvOptionalString(const char *s)
    {
        const char *tmp = std::getenv(s);
        if (tmp)
        {
            return std::string(tmp);
        }
        else
        {
            return std::nullopt;
        }
    }
    std::optional<int> getEnvOptionalInt(const char *s)
    {
        const char *tmp = std::getenv(s);
        if (tmp)
        {
            return atoi(tmp);
        }
        else
        {
            return std::nullopt;
        }
    }
    Url getEnvUrl(const char *s)
    {
        const char *stage = std::getenv("IS_TEST");
        // testステージのみ環境変数STAGEを定義する
        if (stage)
        {
            if (env_map.count(std::string(s)))
            {
                return Url::Parse(env_map[std::string(s)]);
            }
        }
        else
        {
            const char *tmp = std::getenv(s);
            if (tmp)
            {
                return Url::Parse(tmp);
            }
        }
        qmpc::Log::throw_with_trace(std::runtime_error(std::string(s) + " is not defined"));
    }

public:
    int party_id;
    int n_parties;
    int sp_id;
    int getshare_time_limit;
    Url mc_to_cc;
    Url cc_to_bts;
    std::string cc_to_bts_token;
    std::unordered_map<int, Url> ip_addr_map;
    std::unordered_map<int, Url> ip_addr_for_job_map;

    static Config *getInstance(
    )  // シングルトンなオブジェクトへのポインタを返す static なメンバ関数
    {
        static Config obj;  // スタティク変数として Config オブジェクトを生成
        return &obj;        //  それへのポインタを返す
    }
};
