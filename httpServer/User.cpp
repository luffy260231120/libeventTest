#include "User.hpp"
#include <iostream>
#include "event2/http.h"

void User::parseHttp(struct evhttp_request *req) {
    if (req == nullptr) {
        std::cout << __LINE__ << "request为空" << std::endl;
        error = true;
        return;
    }
    // 127.0.0.1:9999/ 后面的全部内容
    const char *uriString = evhttp_request_get_uri(req);
    if (uriString == nullptr) {
        std::cout << __LINE__ << "url为空" << std::endl;
        error = true;
        return;
    } else {
        std::cout << __LINE__ << "uri = " << uriString << std::endl;
    }

    // 解码
    struct evhttp_uri *uri = evhttp_uri_parse(uriString);
    if (uri == nullptr) {
        std::cout << __LINE__ << "错误的URI" << std::endl;
        error = true;
        return;
    }

    char *query = const_cast<char *>(evhttp_uri_get_query(uri));
    if (query == nullptr) {
        std::cout << __LINE__ << "query为空" << std::endl;
        error = true;
        return;
    } else {
        std::cout << __LINE__ << "query=" << query << std::endl;
    }

    // 查询username
    struct evkeyvalq headers = {0};
    if (evhttp_parse_query_str(query, &headers) == 0) {
        username = std::string(evhttp_find_header(&headers, "username"));
        words = std::string(evhttp_find_header(&headers, "words"));
        return;
    } 
    std::cout << "查询出错" << std::endl;
    error = true;
}

void User::show() {
    std::cout << username << " says " << words << std::endl;
}
bool User::getError() {
    return error;
}