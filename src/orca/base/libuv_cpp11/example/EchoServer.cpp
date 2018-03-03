#include    <cstring>
#include    <iostream>

#include    "EchoServer.h"


using namespace uv;
using namespace std;

EchoServer::EchoServer(EventLoop* loop, SocketAddr& addr)
    :TcpServer(loop, addr)
{
    setMessageCallback(std::bind(&EchoServer::newMessage,this,placeholders::_1,placeholders::_2,placeholders::_3));
}

void EchoServer::newMessage(shared_ptr<TcpConnection> connection,const char* buf,ssize_t size)
{
#if    0
    connection->write(buf,size,nullptr);

#else
    //ʵ�ʻ�ֱ�ӵ���write��������Ҫmemcpy��
    //writeInLoop��Ҫ�����ڻص����ͷš�
    char* data =  new  char [size]();
    memcpy(data, buf, size);
    connection->writeInLoop(data, size,
    [this](WriteInfo& info)
    {
		//write message error.
		if (0 != info.status)
		{
			cout << "Write error ��" << EventLoop::GetErrorMessage(info.status) << endl;
		}
        delete [] info.buf;
    });
#endif
}
