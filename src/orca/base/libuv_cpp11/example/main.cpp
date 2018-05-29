/*
    Copyright 2017, orcaer@yeah.net  All rights reserved.

    Author: orcaer@yeah.net

    Last modified: 2018-4-18

    Description: https://github.com/wlgq2/libuv_cpp11
*/


#include <iostream>


#include "EchoServer.h"
#include "Clinet.h"
#include "LogDemo.h"

using namespace uv;

#define      TEST_SIGNAL      1
#define      TEST_SERVER      1
#define      TEST_CLIENT      1
#define      TEST_ASYNC       1
#define      TEST_TIMER       1
#define      TEST_LOG         1

#define       TEST_IPV6       0

int main(int argc, char** args)
{
    //�����¼��ַ�����
    EventLoop* loop = new EventLoop();
    //or
    //EventLoop* loop = EventLoop::DefalutLoop();
  
#if    TEST_SIGNAL
    //�ӹ�SIGPIPE�źš�
    Signal signal(loop,13,[](int sig)
    {
        //SIGPIPE
    });
    
#endif


    //server����
#if   TEST_SERVER
#if    TEST_IPV6 
    SocketAddr addr1("0:0:0:0:0:0:0:0", 10002, SocketAddr::Ipv6);
#else
    SocketAddr addr1("0.0.0.0", 10002, SocketAddr::Ipv4);
#endif

    EchoServer server(loop, addr1);
    //������ʱ
    server.setTimeout(40);
    server.start();
#endif


    //client����
#if  TEST_CLIENT
#if    TEST_IPV6
    SocketAddr addr2("0:0:0:0:0:0:0:1", 10002, SocketAddr::Ipv6);
#else
    SocketAddr addr2("127.0.0.1", 10002, SocketAddr::Ipv4);
#endif
    Client client(loop);
    client.connectToServer(addr2);
#endif


    //loop�߳����첽ִ�к���
#if  TEST_ASYNC
    Async<int>* handle = new Async<int>(loop,
    [](Async<int>* ptr, int data)
    {
        std::cout << data << std::endl;
        delete ptr;
    },
    1024);
    handle->runInLoop();
#endif


    //��ʱ������
#if  TEST_TIMER
    Timer<void*> timer(loop, 1000, 1000,
    [&client](Timer<void*>*,void*)
    {
        std::cout << "timer callback with null arg" << std::endl;
    }, nullptr);
    timer.start();

    //��ʱ��ֻ����һ�μ��ͷ�,������tcp����,������Ϣ��ʱ�ȡ�
    Timer<int>* pTimer  =new Timer<int>(loop, 1000, 0,
        [](Timer<int>* handle, int data)
    {
        std::cout << "timer callback run onice with arg:" <<data<< std::endl;
        handle->close([handle]()
        {
            std::cout << "release timer handle."<< std::endl;
            //�ͷŶ�ʱ������
            delete handle;
        });
    }, 1024);
    pTimer->start();
#endif

    //log�ӿڰ�
#if  TEST_LOG
    auto log = uv::Log::Instance();
    log->registerInterface(new LogDemo);

    log->debug("debug message");
    log->warn("warn message");
    log->error("error message");
    log->info("info message");
#endif
    loop->run();
}