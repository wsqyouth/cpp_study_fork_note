// Copyright (c) 2020 by Chrono

#ifndef _ZMQ_HPP
#define _ZMQ_HPP

#include "cpplang.hpp"

// /usr/include/zmq.hpp
#include <zmq.hpp>

BEGIN_NAMESPACE(cpp_study)

using zmq_context_type = zmq::context_t;
using zmq_socket_type = zmq::socket_t;
using zmq_message_type = zmq::message_t;

template <int thread_num = 1> // 使用整型模板参数指定线程数
class ZmqContext final
{
#if 0
public:
    using zmq_context_type  = zmq::context_t;
    using zmq_socket_type   = zmq::socket_t;
    using zmq_message_type  = zmq::message_t;
#endif
public:
    ZmqContext() = default;
    ~ZmqContext() = default;

public:
    static zmq_context_type &context() // 静态成员函数代替静态成员变量
    // [绕过了 C++ 的语言限制，不必在实现文件“*.cpp”里再写一遍变量定义，全部的代码都可以集中在 hpp 头文件里]
    {
        static zmq_context_type ctx(thread_num);
        return ctx;
    }

public:
    static zmq_socket_type recv_sock(int hwm = 1000, int linger = 10) // 静态工厂函数: 创建接受socket
    {
        zmq_socket_type sock(context(), ZMQ_PULL); // note: 可能抛出异常

        sock.setsockopt(ZMQ_RCVHWM, hwm);
        sock.setsockopt(ZMQ_LINGER, linger); // wait for 10ms

        return sock;
    }

    static zmq_socket_type send_sock(int hwm = 1000, int linger = 10) // 静态工厂函数: 创建发送socket
    {
        zmq_socket_type sock(context(), ZMQ_PUSH); // note: 可能抛出异常

        sock.setsockopt(ZMQ_SNDHWM, hwm);
        sock.setsockopt(ZMQ_LINGER, linger); // wait for 10ms

        return sock;
    }
};

END_NAMESPACE(cpp_study)

#endif //_ZMQ_HPP
