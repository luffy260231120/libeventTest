// cpp实现http服务器，+ User.hpp User.cpp
#include <iostream>
#include <string>
#include <vector>
#include "evhttp.h"
#include "event2/event.h"
#include "event2/buffer.h"
#include "User.hpp"
#define PROT 9999
#define ADDRESS "127.0.0.1"
std::vector<User *> users;

// get请求的回调
void getcb(struct evhttp_request *request, void *arg) {
    if (request == nullptr) {
        std::cout << __LINE__ << "get data为空" << std::endl;
        return;
    }
    User *user = new User;
    user->parseHttp(request); // 还需要加一个错误码
    if (user->getError()) {
        std::cout << "解析失败" << std::endl;
        return;
    }
    user->show();
    users.push_back(user);

    // response
    auto retbuff = evbuffer_new();
    if (retbuff == nullptr) {
        std::cout << __LINE__ << "retbuff 为空" << std::endl;
        return;
    }
    // 忘evbuffer里写
    evbuffer_add_printf(retbuff, "response to Get request");
    // 回复http请求
    evhttp_send_reply(request, HTTP_OK, "Client", retbuff);
    evbuffer_free(retbuff);
}

void postcb(struct evhttp_request *request, void *arg) {

}


int main(int argc, char **argv) {
    // 初始化一个全局base
    event_init();

    // 启动http服务
    struct evhttp *httpServer = evhttp_start(ADDRESS, PROT);
    if (httpServer == nullptr) {
        std::cout << "连接失败" << std::endl;
    } else {
        std::cout << "连接成功" << std::endl;
    }

    evhttp_set_cb(httpServer, "/testGet", getcb, NULL);
    evhttp_set_cb(httpServer, "/testPost", postcb, NULL);

    // 循环全局监听
    event_dispatch();
    evhttp_free(httpServer);
    return 0;
}
