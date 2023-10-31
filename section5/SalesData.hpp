// Copyright (c) 2020 by Chrono

#ifndef _SALES_DATA_HPP
#define _SALES_DATA_HPP

#include "cpplang.hpp"

#include <msgpack.hpp>

#if MSGPACK_VERSION_MAJOR < 2
#error "msgpack  is too old"
#endif

BEGIN_NAMESPACE(cpp_study)

// demo oop in C++
class SalesData final // final禁止继承
{
public:
    using this_type = SalesData; // 自己的类型别名

public:
    using string_type = std::string; // 外部的类型别名
    using string_view_type = const std::string &;
    using uint_type = unsigned int;
    using currency_type = double;

    STATIC_ASSERT(sizeof(uint_type) >= 4); // 静态断言
    STATIC_ASSERT(sizeof(currency_type) >= 4);

public:
    SalesData(string_view_type id, uint_type s, currency_type r) noexcept // 构造函数保证不抛出异常
        : m_id(id), m_sold(s), m_revenue(r)
    {
    }

    SalesData(string_view_type id) noexcept // 委托构造
        : SalesData(id, 0, 0)
    {
    }

public:
#if 0
    SalesData(SalesData&& s) noexcept
        : m_id(std::move(s.m_id)),
          m_sold(std::move(s.m_sold)),
          m_revenue(std::move(s.m_revenue))
    {}

    SalesData& operator=(SalesData&& s) noexcept
    {
        m_id = std::move(s.m_id);
        m_sold = std::move(s.m_sold);
        m_revenue = std::move(s.m_revenue);

        return *this;
    }
#endif

    SalesData() = default;
    ~SalesData() = default;

    SalesData(const this_type &) = default; // 显式default
    SalesData &operator=(const this_type &) = default;

    SalesData(this_type &&s) = default; // 显式转移构造
    SalesData &operator=(this_type &&s) = default;

private:
    string_type m_id = ""; // 成功变量初始化
    uint_type m_sold = 0;
    uint_type m_revenue = 0;

public:
    /*
    在一定程度上违反了单一职责原则和接口隔离原则。需要权衡: 是扩充接口还是不那么单一职责
    */
    MSGPACK_DEFINE(m_id, m_sold, m_revenue); /// 实现MessagePack序列化功能

    msgpack::sbuffer pack() const // 序列化函数
    {
        msgpack::sbuffer sbuf;
        msgpack::pack(sbuf, *this);

        return sbuf;
    }

    SalesData(const msgpack::sbuffer &sbuf)
    {
        auto obj = msgpack::unpack(
                       sbuf.data(), sbuf.size())
                       .get();
        obj.convert(*this);
    }

public:
    void inc_sold(uint_type s) noexcept // 不抛出异常
    {
        m_sold += s;
    }

    void inc_revenue(currency_type r) noexcept
    {
        m_revenue += r;
    }

public:
    string_view_type id() const noexcept // 常函数,不抛出异常
    {
        return m_id;
    }

    uint_type sold() const noexcept
    {
        return m_sold;
    }

    currency_type revenue() const noexcept
    {
        return m_revenue;
    }

    CPP_DEPRECATED
    currency_type average() const
    {
        return m_revenue / m_sold;
    }
};

END_NAMESPACE(cpp_study)

#endif //_SALES_DATA_HPP

/*
代码里显式声明了转移构造和转移赋值函数，这样，在放入容器的时候就避免了拷贝，能提高运行效率。
=> 这里还不是很懂,引用传递的玩法
*/