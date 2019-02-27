/*
    Copyright 2017, orcaer@yeah.net  All rights reserved.

    Author: orcaer@yeah.net

    Last modified: 2018-8-27

    Description: https://github.com/wlgq2/libuv_cpp11
*/


#include <iostream>


#include "EchoServer.h"
#include "Clinet.h"
#include "LogDemo.h"
#include <chrono>

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

#if  1

    //���̷߳�������
    std::thread thread([&client]()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        char* data = new char[4] {'t','e','s','t'};
       
        //�̰߳�ȫ;
        client.writeInLoop(data,sizeof(data),
            [](uv::WriteInfo& info)
        {
            //������Ҫ�ڷ�����ɻص����ͷ�
            //write message error.
            if (0 != info.status)
            {
                //��ӡ������Ϣ
                std::cout << "Write error ��" << EventLoop::GetErrorMessage(info.status) << std::endl;
            }
            delete[] info.buf;
        });
    });
#endif

#endif


    //loop�߳����첽ִ�к��������ڿ��̲߳�����
    //�����ԭ��libuv async�ӿڣ��޸��˵��ö�ο���ֻ����һ�ε����⡣
#if  TEST_ASYNC
    loop->runInThisLoop(
        []()
    {
        std::cout << "run function in loop thread one." << std::endl;
    });

    loop->runInThisLoop(
        []()
    {
        std::cout << "run function in loop thread two." << std::endl;
    });
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
        handle->close([](Timer<int>* ptr)
        {
            std::cout << "release timer handle."<< std::endl;
            //�ͷŶ�ʱ������
            delete ptr;
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