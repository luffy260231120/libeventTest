#ifndef __user_hpp
#define __user_hpp

#include <string>
#include <evhttp.h>

class User {
private:
    std::string username = "";
    std::string words = "";
    bool error = false;
public:
    void parseHttp(struct evhttp_request *req);
    void show();
    bool getError();
};

#endif