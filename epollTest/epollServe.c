#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <sys/epoll.h>

#define SERVER_PORT         (7778)
#define EPOLL_MAX_NUM       (2048)
#define BUFFER_MAX_LEN      (4096)

char buffer[BUFFER_MAX_LEN];

void str_toupper(char *str)
{
    int i;
    for (i = 0; i < strlen(str); i ++) {
        str[i] = toupper(str[i]);
    }
}

// 服务器socket:
// socket()->bind()->listen()->accept()
// read()/ send()
// close()
int main(int argc, char **argv)
{
    int listen_fd = 0;
    int client_fd = 0;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t           client_len;

    int epfd = 0;
    struct epoll_event event, *my_events;

    // socket 创建新的fd
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    // bind
    server_addr.sin_family = AF_INET; // #define PF_INET		2	/* IP protocol family.  */
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);
    bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    // listen
    listen(listen_fd, 10);

    // 创建epoll句柄集
    epfd = epoll_create(EPOLL_MAX_NUM);
    if (epfd < 0) {
        perror("epoll create");
        goto END;
    }

    // 设置监听事件的属性
    // struct epoll_event {
    //     __uint32_t events; /* epoll 事件 {EPOLLIN, EPOLLOUT, EPOLLPRI, EPOLLHUP, EPOLLET, EPOLLONESHOT}*/
    //     epoll_data_t data; /* 用户传递的数据 */
    // }
    event.events = EPOLLIN;
    event.data.fd = listen_fd;
    // 把新的listen_fd加入到epfd内核控制的epfd中
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &event) < 0) {
        perror("epoll ctl add listen_fd ");
        goto END;
    }

    my_events = malloc(sizeof(struct epoll_event) * EPOLL_MAX_NUM);

    while (1) {
        printf("阻塞等待fd触发....\n");
        // epoll wait 阻塞等待fd触发
        int active_fds_cnt = epoll_wait(epfd, my_events, EPOLL_MAX_NUM, -1);
        int i = 0;
        printf("触发%d个\n", active_fds_cnt);
        for (i = 0; i < active_fds_cnt; i++) {
            // if fd == listen_fd 当注册的新fd被触发时，即socket连接成功时
            if (my_events[i].data.fd == listen_fd) {
                // accept接收请求
                client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
                if (client_fd < 0) {
                    perror("accept");
                    continue;
                }

                char ip[20];
                printf("new connection[%s:%d]\n", inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip)), ntohs(client_addr.sin_port));

                // 既然连接成功了，那么开始监听客户端消息
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = client_fd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &event);
            } else if (my_events[i].events & EPOLLIN) {
                printf("EPOLLIN\n");
                client_fd = my_events[i].data.fd;

                // do read
                buffer[0] = '\0';
                // 从client_fd中只读5个字节__>todo 这里为啥超过5个就会再也读不到此客户端的消息了？  
                int n = read(client_fd, buffer, 5);
                if (n < 0) {
                    printf("n < 0");
                    perror("read");
                    continue;
                } else if (n == 0) {
                    printf("n == 0");
                    epoll_ctl(epfd, EPOLL_CTL_DEL, client_fd, &event);
                    close(client_fd);
                } else {
                    printf("[read]: %s\n", buffer);
                    buffer[n] = '\0';
#if 1
                    str_toupper(buffer);
                    write(client_fd, buffer, strlen(buffer));
                    printf("[write]: %s\n", buffer);
                    memset(buffer, 0, BUFFER_MAX_LEN);
#endif

/*
                    event.events = EPOLLOUT;
                    event.data.fd = client_fd;
                    epoll_ctl(epfd, EPOLL_CTL_MOD, client_fd, &event);
*/
                }
            } else if (my_events[i].events & EPOLLOUT) {
                printf("EPOLLOUT\n");
/*
                client_fd = my_events[i].data.fd;
                str_toupper(buffer);
                write(client_fd, buffer, strlen(buffer));
                printf("[write]: %s\n", buffer);
                memset(buffer, 0, BUFFER_MAX_LEN);

                event.events = EPOLLIN;
                event.data.fd = client_fd;
                epoll_ctl(epfd, EPOLL_CTL_MOD, client_fd, &event);
*/
            }
        }
        printf("结束epoll事件处理\n");
    }

    printf("跳出循环了\n");

END:
    close(epfd);
    close(listen_fd);
    return 0;
}