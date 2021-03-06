// Copyright (c) 2014 The EventServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _EVENT_LOOP_H
#define _EVENT_LOOP_H

#include "Thread.h"
#include "Noncopyable.h"
using namespace utils;

#include "Handler.h"
#include "Acceptor.h"
#include "Selector.h"
#include "SignalHandler.h"

namespace eventserver
{

class EventPool;

/**
** An event loop that multiplexes a set of handlers.
**/
class EventLoop : public Noncopyable
{
public:
    /// Constructor.
    EventLoop(EventPool *pool);

    /// Destructor.
    ~EventLoop();

public:
    /// Invokes the event loop.
    /// Return only when stop by manually
    void runForever();

    /// Invokes the event loop once through.
    void runOnce();

    /// Signals that the event loop should terminate.  May be invoked
    /// either by event handlers or by another thread.
    void stop();

public:
    /// Attach handler to pool
    void attachHandler(int fd, Handler *p);

    /// Detach handler from pool
    void detachHandler(int fd, Handler *p);

public:
    /// Register Read Event of fd
    void registerRead(int fd);

    /// Register Write Event of fd
    void registerWrite(int fd);

    /// Unregister Read Event of fd
    void unRegisterRead(int fd);

    /// Unregister Write Event of fd
    void unRegisterWrite(int fd);

    /// Need to be removed
    void waitRemoved(Handler *handler);

public:
    /// Add active event to `m_active`
    void addActive(int fd, int type);

    /// Add handler to `m_del`
    void addClosed(Handler* handler);

    /// Process the handlers need to be delete
    void finishDelete();

    /// Return one random loop
    EventLoop* getRandomLoop(int &thrid);

    /// Attach Timer to this loop
    void attach(const Callback<void> &call, const Timer &timer);

    /// Attach Signal Handler
    void attach(int signo, void (*sighandler)(int));

    /// Attach signal handler
    void attach();

    /// Attach pipes between threads
    void attach(Socket sock, int thrid, SQueue<Handler*> *h_queue);

    map<int, Handler*> handlers()
    {
        return m_handlers;
    }

private:
    /// Run After dispatch(for all active events)
    void runAllActives();

private:
    /// m_stop, status of whether need to be stopped
    bool       m_stop;

    /// multiplexes dispatch selector
    Selector *m_selector;

    /// event pool
    EventPool *m_pool;

    /// Use to sync the change of status
    Mutex     m_mutex;

    /// list or map
    map<int, int>      m_active;
    vector<Handler*>   m_del;
    map<int, Handler*> m_map;
    TimerEvents        m_heap;
    SignalHandler     *m_signal;
    map<int, Handler*> m_handlers;
    friend class Acceptor;
};

};

#endif