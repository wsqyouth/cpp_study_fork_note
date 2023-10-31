// Copyright (c) 2020 by Chrono
//
// g++ srv.cpp -std=c++14 -I../common -I../common/include -I/usr/local/include/luajit-2.1 -lluajit-5.1 -ldl -lzmq -lpthread -lcpr -lcurl -o a.out;./a.out
// g++ srv.cpp -std=c++14 -I../common -I../common/include -I/usr/local/include/luajit-2.1 -lluajit-5.1 -ldl -lzmq -lpthread -lcpr -lcurl -g -O0 -o a.out
// g++ srv.cpp -std=c++14 -I../common -I../common/include -I/usr/local/include/luajit-2.1 -lluajit-5.1 -ldl -lzmq -lpthread -lcpr -lcurl -g -O0 -o a.out;./a.out

// #include <iostream>

#include "cpplang.hpp"
#include "Summary.hpp"
#include "Zmq.hpp"
#include "Config.hpp"

// you should put json.hpp in ../common
#include "json.hpp"

#include <cstdio>
#include <cpr/cpr.h>

USING_NAMESPACE(std);
USING_NAMESPACE(cpp_study);

static auto debug_print = [](auto &b)
{
    using json_t = nlohmann::json;

    json_t j;

    j["id"] = b.id();
    j["sold"] = b.sold();
    j["revenue"] = b.revenue();
    // j["average"] = b.average();

    std::cout << j.dump(2) << std::endl;
};

int main()
try
{
    cout << "hello cpp_study server" << endl;

    Config conf;
    conf.load("./conf.lua"); // 解析luna配置文件

    Summary sum;              // 数据存储统计
    std::atomic_int count{0}; // 计数用的原子变量,应该是多个线程共用了

    // zmq recv: ZMQ 接收数据，然后 MessagePack 反序列化，存储数据
    // todo: try-catch 引用捕获上面的那些变量
    auto recv_cycle = [&]()
    {
        using zmq_ctx = ZmqContext<1>;
        auto sock = zmq_ctx::recv_sock(); // 自动类型推导获取接收sock

        sock.bind(conf.get<string>("config.zmq_ipc_addr"));
        assert(sock.connected());

        for (;;)
        {

            // xxx : shared_ptr/unique_ptr
            auto msg_ptr = std::make_shared<zmq_message_type>();

            sock.recv(msg_ptr.get()); // 智能指针指向这个接收缓冲区
            // cout << msg_ptr->size() << endl;

            ++count;
            cout << count << endl;
            // printf("count = %d\n", static_cast<int>(count));

            // async process msg
            // todo: try-catch
            // 再启动一个线程反序列化并存储, 仅捕获部分变量
            std::thread(
                [&sum, msg_ptr]()
                //[&sum, &count](decltype(msg_ptr) ptr)
                {
                    // cout << ptr.unique() << endl;

                    SalesData book;

                    // 序列化: json/msgpack/protobuf
                    auto obj = msgpack::unpack(
                                   msg_ptr->data<char>(), msg_ptr->size())
                                   .get();
                    obj.convert(book);

                    // cout << book.id() << endl;
                    // debug_print(book);

                    sum.add_sales(book);
                })
                .detach(); // 分离线程,异步运行
        }
    };
    /*
    特别注意 lambda 表达式与智能指针的配合方式，要用值捕获而不能是引用捕获，
    否则，在线程运行的时候，智能指针可能会因为离开作用域而被销毁，引用失效，导致无法预知的错误。
    */

    // 采用了 HTTP 协议，把数据打包成 JSON，发送到后台的某个 RESTful 服务器
    // 引用捕获上面的那些变量
    auto log_cycle = [&]()
    {
        auto http_addr = conf.get<string>("config.http_addr");
        auto time_interval = conf.get<int>("config.time_interval"); // 读取http服务器地址和周期运行时间

        for (;;)
        {
            std::this_thread::sleep_for(time_interval * 1s);
            // cout << "log_cycle" << endl;

            // auto info = sum.minmax_sales();
            // cout << "log_cycle get info" << endl;

            using json_t = nlohmann::json;

            json_t j;

            j["count"] = static_cast<int>(count);
            j["minmax"] = sum.minmax_sales(); //{info.first, info.second};

            auto res = cpr::Post( // http请求, json序列化, 设置超时时间
                cpr::Url{http_addr},
                cpr::Body{j.dump()},
                cpr::Timeout{200ms});

            if (res.status_code != 200)
            {
                cerr << "http post failed" << endl;
                // printf("http post failed\n");
            }
        }
    };

    // launch log_cycle
    auto fu1 = std::async(std::launch::async, log_cycle);

    // launch recv_cycle then wait
    auto fu2 = std::async(std::launch::async, recv_cycle);

    fu2.wait();
}
catch (std::exception &e)
{
    std::cerr << e.what() << std::endl;
}
