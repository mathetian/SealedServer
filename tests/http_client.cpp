// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "HttpClient.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
using namespace sealedserver;

HttpClient client;

void get(HttpRequest *req, void *arg)
{	
}

void error(HttpRequest *req, void *arg)
{
}

/// Signal Stop the server
void signalStop(int)
{
    INFO << "Stop running...by manually";
    server.stop();
}

/// Change the configure
int setlimit(int num_pipes)
{
    struct rlimit rl;
    rl.rlim_cur = rl.rlim_max = num_pipes * 2 + 50;
    if (::setrlimit(RLIMIT_NOFILE, &rl) == -1)
    {
        fprintf(stderr, "setrlimit error: %s", strerror(errno));
        return 1;
    }
}

int main()
{
	Log::setLevel(Log::debug);

    ::signal(SIGINT, signalStop);
    setlimit(100000); errno = 0;

	client.request("http://www.google.com", get, error);
	client.request("http://128.199.204.82", get, error);
	
	client.wait();

	return 0;
}