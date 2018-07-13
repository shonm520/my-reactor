
#include "reactor.h"
#include "event_handler.h"
#include "event_demultiplexer.h"

Reactor::Reactor() :
m_demultiplexer(new EpollDemultiplexer()) {

}

int Reactor::RegisterHandler(EventHandler* handler, int event)  {
    return m_demultiplexer->AddEvents(handler, event);
}

int Reactor::RemoveHandler(EventHandler* handler)  {
    return m_demultiplexer->RemoveEvent(handler);
}

bool Reactor::HandleEvents()  {
    int ret = m_demultiplexer->WaitEvents();
    return ret >= 0;
}


