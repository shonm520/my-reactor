#include <errno.h>
#include <stdio.h>
#include "event_demultiplexer.h"
#include "event_handler.h"



int EpollDemultiplexer::WaitEvents(int timeout)  {
    epoll_event ev[1024];
    int num = epoll_wait(m_epollFd, ev, 1024, timeout);
    if (num > 0)  {
        for (int i = 0; i < num; i++)  {
            EventHandler* handler = (EventHandler*)ev[i].data.ptr;
            if (ev[i].events & EPOLLERR || 
                ev[i].events & EPOLLHUP)  {
                    handler->HandleError();
                } 
            else  {
                if (ev[i].events & EPOLLIN)  {
                    handler->HandleRead();
                }
                else if (ev[i].events & EPOLLOUT)  {
                    handler->HandleWrite();
                }
            }
        }
    }
    if (num == -1)  {
        perror("epoll_wait");
    }
    return num;
}

int EpollDemultiplexer::AddEvents(EventHandler* handle, int event)  {
    epoll_event ev;
    ev.data.ptr = handle;
    if (event & EventHandler::kReadEvent)  {
        ev.events |= EPOLLIN;
    }
    else if (event & EventHandler::kWriteEvent)  {
        ev.events |= EPOLLOUT;
    }

    if (epoll_ctl(m_epollFd, EPOLL_CTL_MOD, handle->GetFd(), &ev))  {
        if (errno == ENOENT)  {
            if (epoll_ctl(m_epollFd, EPOLL_CTL_ADD, handle->GetFd(), &ev) != 0)  {
                return -errno;
            }
        }
    }
    return 0;
}

int EpollDemultiplexer::UnrequestEvent(EventHandler* handle)  {
    epoll_event ep_evt;
    if (epoll_ctl(m_epollFd, EPOLL_CTL_DEL, handle->GetFd(), &ep_evt) != 0)  {
        return -errno;
    }
    return 0;
}


