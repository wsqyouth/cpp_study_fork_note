// Copyright (c) 2020 by Chrono

#ifndef _SUMMARY_HPP
#define _SUMMARY_HPP

#include "cpplang.hpp"
#include "SalesData.hpp"
#include "SpinLock.hpp"

BEGIN_NAMESPACE(cpp_study)

class Summary final // final禁止继承
{
public:
    using this_type = Summary; // 类型别名

public:
    using sales_type = SalesData;
    using lock_type = SpinLock;
    using lock_guard_type = SpinLockGuard;

    using string_type = std::string;
    using map_type =
        std::map<string_type, sales_type>; // 容器类型定义 [考虑到销售记录不仅要存储，还有对数据的排序要求]
    // std::unordered_map<string_type, sales_type>;
    using minmax_sales_type =
        std::pair<string_type, string_type>;

public:
    Summary() = default; // 显式default
    ~Summary() = default;

    Summary(const this_type &) = delete; // 显式delete
    Summary &operator=(const this_type &) = delete;

private:
    mutable lock_type m_lock; // 自旋锁 [使用自旋锁来保护核心数据，在对容器进行任何操作前都要获取锁。锁不影响类的状态，所以要用 mutable 修饰。=> 没懂为啥要用mutable]
    map_type m_sales;         // Map存储销售记录

public:
    void add_sales(const sales_type &s)
    {
        lock_guard_type guard(m_lock); // RAII的SpinLockGuard，所以自旋锁用起来很优雅，直接构造一个变量就行，不用担心异常安全的问题： 自动锁定,自动解锁

        const auto &id = s.id(); // 自动类型推导

        // not found
        if (m_sales.find(id) == m_sales.end()) // map查找算法, not found就添加
        {
            m_sales[id] = s;
            return;
        }

        // found
        // you could use iter to optimize it
        m_sales[id].inc_sold(s.sold()); // 找到就修改对应id的销售数据
        m_sales[id].inc_revenue(s.revenue());
    }

    minmax_sales_type minmax_sales() const
    {
        lock_guard_type guard(m_lock);

        if (m_sales.empty())
        {
            return minmax_sales_type();
        }

        // algorithm
        auto ret = std::minmax_element( // 求最小最大值, 匿名lambda表达式
            std::begin(m_sales), std::end(m_sales),
            [](const auto &a, const auto &b)
            {
                return a.second.sold() < b.second.sold();
            });

        // min max 迭代器
        auto min_pos = ret.first;
        auto max_pos = ret.second;

        return {min_pos->second.id(), max_pos->second.id()};
    }
};

END_NAMESPACE(cpp_study)

#endif //_SUMMARY_HPP
