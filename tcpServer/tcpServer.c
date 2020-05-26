#include <stdio.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <string.h>
#define PORT 9999

void listener_cb(struct evconnlistener *, evutil_socket_t, struct sockaddr *, int socklen, void *);
void buffereventRead_cb(struct bufferevent *bev, void *ctx);
void buffereventWrite_cb(struct bufferevent *bev, void *ctx);
void bufferevent_cb(struct bufferevent *bev, short what, void *ctx);

int main(int argc, char **argv) {
    struct event_base *base = event_base_new();
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET; 
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    struct evconnlistener *listener = evconnlistener_new_bind(base, listener_cb, NULL, LEV_OPT_CLOSE_ON_FREE, 10, (const struct sockaddr *)&server_addr, sizeof(server_addr));

    event_base_dispatch(base);

    evconnlistener_free(listener);
	event_base_free(base);
}

void listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *server_addr, int socklen, void *pdata) {
    // 先从监听者事件中取出base管理者
    struct event_base *base = evconnlistener_get_base(listener);

    struct bufferevent *buffer = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE );
    // 设置回调函数
    bufferevent_setcb(buffer, buffereventRead_cb, buffereventWrite_cb, bufferevent_cb, NULL);

    bufferevent_enable(buffer, EV_READ|EV_WRITE);
}

// 输入缓冲区到达水位线或者更高时调用
void buffereventRead_cb(struct bufferevent *bev, void *ctx) {
	printf("buffereventRead_cb\n");
 
	//读取输入缓冲数据
	char data[1024] = {0};
	int len = bufferevent_read(bev, data, sizeof(data)-1);
	printf("%s\n读取全部数据了\n", data);
	if(len <= 0) return;
	if(strstr(data, "quit") != NULL)
	{
		printf("quit\n");
		
		//退出并关闭socket 
		bufferevent_free(bev);
	}
 
	//发送数据 写入到输出缓冲
	bufferevent_write(bev, "OK", 3);
}

// 输出缓冲区被清空时才调用写回调函数。
void (struct bufferebuffereventWrite_cbvent *bev, void *ctx) {
    printf("buffereventWrite_cb\n");
}

void bufferevent_cb(struct bufferevent *bev, short what, void *ctx) {
    if (what & BEV_EVENT_ERROR)
        perror("Error from bufferevent");
    if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
        bufferevent_free(bev);
    } else {
        printf("????");
    }
}