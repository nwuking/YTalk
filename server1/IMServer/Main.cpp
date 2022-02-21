/**
 * @file Main.cpp
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-15
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <iostream>

#include <signal.h>

#include "../base/ConfigParse.h"
#include "../base/Logger.h"
#include "../netlib/Types.h"
#include "../base/Singleton.h"
#include "../mysql/MysqlManager.h"
#include "UserManager.h"
#include "ChatService.h"
#include "../netlib/EventLoop.h"

using namespace YTalk;
using namespace YTalk::base;

#define CONFIG_LOG_DIR "log_dir"
#define CONFIG_LOG_FILE_NAME "log_file_name"
#define CONFIG_DB_HOST "db_host"
#define CONFIG_DB_PORT "db_port"
#define CONFIG_DB_USER "db_user"
#define CONFIG_DB_PW "db_pw"
#define CONFIG_DB_NAME "db_name"
#define CONFIG_CLIENT_LISTEN_IP "client_listen_ip"
#define CONFIG_CLIENT_LISTEN_PORT "client_listen_port"


/**
 * @brief 主循环loop
 * 
 */
netlib::EventLoop g_loop;

/**
 * @brief 
 * 
 * @param sig 
 */
void sig_exit(int sig) {
    std::cout << "YTalk Server receive signal[" << sig << "] to exit\n";

    Singleton<IMServer::ChatService>::getInstance().stop();
    g_loop.quit();
    LOG_SERVICE_STOP();
}

int main(int argc, char *argv[])
{
    // 设置信号处理
    signal(SIGCHLD, SIG_DFL);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, sig_exit);
    signal(SIGTERM, sig_exit);

// 读取相关配置
    base::ConfigParse Config("etc/IMServer.conf");

/// 初始化LogService
    std::string logDir = Config.getConfigValue(CONFIG_LOG_DIR);
    if(logDir.empty()) {
        // 配置文件没有，程序终止
        std::cout << "You need to configure [" << CONFIG_LOG_DIR << "] in IMServer.conf\n";
        return 1;
    }
    DIR *dir = opendir(logDir.c_str());
    if(dir == nullptr) {
        // 目录不存在就创建目录
        if(mkdir(logDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
            std::cout << "Fail to create [" << CONFIG_LOG_DIR << "]\n";
            return 2;
        }
    }
    closedir(dir);
    std::string logPath = logDir;
    std::string logFile = Config.getConfigValue(CONFIG_LOG_FILE_NAME);
    logPath += logFile;
    if(logFile.empty()) {
        LOG_SERVICE_INIT();
    }
    else {
        LOG_SERVICE_INIT(logPath.c_str());
    }

/// 初始化化数据库管理模块
    std::string dbHost = Config.getConfigValue(CONFIG_DB_HOST);
    std::string dbPortStr = Config.getConfigValue(CONFIG_DB_PORT);
    std::uint16_t dbPort = std::stoi(dbPortStr);
    std::string dbUser = Config.getConfigValue(CONFIG_DB_USER);
    std::string dbPW = Config.getConfigValue(CONFIG_DB_PW);
    std::string dbName = Config.getConfigValue(CONFIG_DB_NAME);
    if(Singleton<mysql::MysqlManager>::getInstance().init(dbHost, dbPort, dbUser, dbPW, dbName)) {
        LOG_FATAL("Fail to init mysql");                                                     
    }

/// 初始化用户管理模块
    if(Singleton<IMServer::UserManager>::getInstance().init()) {
        LOG_FATAL("Fail to init UserManager");
    }

/// 初始化聊天服务模块
    std::string clientListenIP = Config.getConfigValue(CONFIG_CLIENT_LISTEN_IP);
    std::string clientListenPortStr = Config.getConfigValue(CONFIG_CLIENT_LISTEN_PORT);
    std::uint16_t clientListenPort = static_cast<std::uint16_t>(std::stoi(clientListenPortStr));
    Singleton<IMServer::ChatService>::getInstance().init(&g_loop, clientListenIP, clientListenPort);

    g_loop.loop();

    return 0;
}