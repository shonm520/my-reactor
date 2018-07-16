#pragma once


class EventHandler  {
public:
    virtual void HandleRead() = 0;
    virtual void HandleWrite() = 0 ;
    virtual void HandleError() = 0;
    int GetFd()  {return m_fd;}

    enum  {
        kReadEvent    = 0x01,
        kWriteEvent   = 0x02,
        kErrorEvent   = 0x04,
        kEventMask    = 0xff
    } ;

protected:
    int m_fd;
};




