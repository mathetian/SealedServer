#ifndef _RECTOR_SEL_H
#define _RECTOR_SEL_H

#include <vector>
using namespace std;

#include "../../utils/Thread.h"
using namespace utils;
#include "../ReactorImpl.h"

#include "assert.h"

class ReactorSelect : public ReactorImpl
{
public:
	ReactorSelect(EventLoop* const loop) : ReactorImpl(loop) 
	{ 
		FD_ZERO(&m_readfds);
		FD_ZERO(&m_writefds);
		m_maxfd = -1;
	}

	virtual const char *getMethod() { return "RectorSelect"; }

	virtual int dispatch(TimeStamp next);

	virtual int registerEvent(SocketHandler *handler, short event)
	{
		Socket sock = handler->getSocket();
		if(event == 0) 
		{
			assert(FD_ISSET(sock, &m_readfds) == false);
			FD_SET(sock,&m_readfds);
		}
		else
		{
			assert(FD_ISSET(sock, &m_writefds) == false);
			FD_SET(sock,&m_writefds);
		}

		m_maxfd = max(m_maxfd, (int)sock);
	}

	virtual int unRegisterEvent(SocketHandler *handler, short event)
	{
		Socket sock = handler->getSocket();
		if(event == 0) 
		{
			assert(FD_ISSET(sock, &m_readfds) == true);
			FD_CLR(sock,&m_readfds);
		}
		else
		{
			assert(FD_ISSET(sock, &m_writefds) == true);
			FD_CLR(sock,&m_writefds);
		}
	}

private:
	int       m_maxfd; 
	fd_set    m_readfds;
	fd_set    m_writefds;
	Mutex     m_mutex;
};


#endif