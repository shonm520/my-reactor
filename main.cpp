#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "event_handler.h"
#include "reactor.h"

class EventLoop {
public:
    EventLoop()  {
        m_reactor = new Reactor();
    }
    ~EventLoop()  {
        delete m_reactor;
    }
    void Loop()  {
        while(1)  {
            if(!m_reactor->HandleEvents())  {
                break;
            }
        }
    }
    Reactor* GetReactor()  {
        return m_reactor;
    }
private:
    Reactor* m_reactor;
};

class ServerHandler : public EventHandler  {
public:
    ServerHandler() {}

    virtual void HandleRead() override {
        sockaddr addr;
        socklen_t addrlen = sizeof(addr);
        int handle = accept(m_fd, &addr, &addrlen);
        if (handle < 0)  {
            perror("accept");
        }
        else  {
            EchoSever* handler = new EchoSever(handle);
            handler->SetEventLoop(m_loop);
            if (m_loop->GetReactor()->RegisterHandler(handler, kReadEvent) != 0 ||
                m_loop->GetReactor()->RegisterHandler(handler, kWriteEvent) != 0)  {
                fprintf(stderr, "error: register handler failed\n");
                delete handler;
            }
            printf("accepted a client!!!\n");
        }
    }

    virtual void HandleWrite() override {}

    virtual void HandleError() override {
        ::close(m_fd);
        m_loop->GetReactor()->RemoveHandler(this);
    }

    void SetEventLoop(EventLoop* loop)  {
        m_loop = loop;
    }
    bool Start(char* ip, int port)  {
        printf("ip : %s, port : %d\n", ip, port);
        m_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (m_fd <= 0)  {
            perror("socket");
            return false;
        }
        int on = 1;
        setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip);
        if (bind(m_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)  {
            perror("bind");
            return false;
        }

        if (listen(m_fd, 10) < 0)  {
            perror("listen");
            return false;
        }

        m_loop->GetReactor()->RegisterHandler(this, kReadEvent);
        return true;
    }

    class EchoSever : public EventHandler  {
    public:
        EchoSever(int fd) {m_fd = fd;}
        void HandleError() override {}
        void HandleRead() override {
            char buf[1024] = {0};
            int ret = ::recv(m_fd, buf, 1024, 0);
            if (ret == 0)  {
                printf("closed\n");
                m_loop->GetReactor()->RemoveHandler(this);
            }
            else if (ret < 0)  {
                printf("sockt read error!!!\n");
            }
            else  {
                printf("recv from client : %s\n", buf);
				::send(m_fd, buf, ret, 0);
            }
        }
        void HandleWrite() override {
        }

        void SetEventLoop(EventLoop* loop)  {
            m_loop = loop;
        }
        EventLoop* m_loop;
    };

private:
    EventLoop* m_loop;

};



int main(int argc, char** argv )  {
    
    ServerHandler server;

    EventLoop eventloop;
    server.SetEventLoop(&eventloop);

    if (!server.Start("192.168.1.136", 7878))  {
        fprintf(stderr, "start server failed\n");
        return -1;
    }
    fprintf(stderr, "server started!\n");

    eventloop.Loop();

    return 0;
}
