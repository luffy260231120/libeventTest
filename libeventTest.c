#include <stdio.h>
#include <event.h>
// 使用libevent设置定时器

// 回调函数
void time_cb(int fd, short event, void *argc) {
    printf("time out");
    // event_add()
}

int main() {
    // 初始化libevent库base
    // 一个base里可以注册多个event, 
    // 一个loop对应一个base
    struct event_base *base = event_init();

    // 初始化事件
    struct event ev;
    // 设置时间的回调函数time_cb，设置好事件类型(这里不需要)
    evtimer_set(&ev, time_cb, NULL);

    // 设置event从属的event_base
    // 如果一个线程里有多个base,那么必须先设置base，再做其他的操作
    event_base_set(base, &ev);

    // 函数注册事件，并设置超时时间
    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    event_add(&ev, &tv);

    // 进入无限循环，等待事件。
    // select() 每次循环前会检查事件的最小超时时间tv,以便及时处理超时事件。
    // poll()
    // epoll() 非阻塞等待tv的到来，到了就处理回调事件。
    event_base_dispatch(base);

}