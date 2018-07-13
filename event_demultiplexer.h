#include <sys/epoll.h>
#include <unistd.h>
#include "event_handler.h"


class EventDemultiplexer  {
public:
    virtual ~EventDemultiplexer()   {}

    virtual int WaitEvents(int timeout = -1) = 0;

    virtual int AddEvents(EventHandler* handle, int event) = 0;

    virtual int RemoveEvent(EventHandler* handle) = 0;
};


class EventHandler;

class EpollDemultiplexer : public EventDemultiplexer  {
public:
    EpollDemultiplexer()  {
        m_epollFd = epoll_create(1);
    }

    virtual ~EpollDemultiplexer()  {
        ::close(m_epollFd);
    }

    virtual int WaitEvents(int timeout = -1) override;

    virtual int AddEvents(EventHandler* handle, int event) override;

    virtual int RemoveEvent(EventHandler* handle) override;

private:
    int m_epollFd;
};