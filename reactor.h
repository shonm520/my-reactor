
#pragma once

class EventHandler;
class EventDemultiplexer;

class Reactor  {
public:
    Reactor();
    ~Reactor(){}

    int RegisterHandler(EventHandler* handler, int event);

    int RemoveHandler(EventHandler*);

    bool HandleEvents();
private:
    EventDemultiplexer*  m_demultiplexer;
};