// Copyright (c) 2020 by Chrono

#ifndef _SPIN_LOCK_HPP
#define _SPIN_LOCK_HPP

#include "cpplang.hpp"

BEGIN_NAMESPACE(cpp_study)

// atomic spinlock with TAS
class SpinLock final // 自旋锁类
{
public:
    using this_type = SpinLock; // 类型别名
    using atomic_type = std::atomic_flag;

public:
    SpinLock() = default; // 默认构造函数
    ~SpinLock() = default;

    SpinLock(const this_type &) = delete; // 禁止拷贝
    SpinLock &operator=(const this_type &) = delete;

public:
    void lock() noexcept // 自旋锁定函数, 决不会抛出异常
    {
        for (;;)
        {
            if (!m_lock.test_and_set()) // 原子变量的TAS操作
            {
                return; // TAS成功则锁定
            }

            std::this_thread::yield(); // TAS失败则让出线程
        }
    }

    bool try_lock() noexcept
    {
        return !m_lock.test_and_set();
    }

    void unlock() noexcept // 自旋解除锁定函数, 决不会抛出异常
    {
        m_lock.clear();
    }

private:
    atomic_type m_lock{false};
};

// RAII for lock 利用RAII技术再编写一个LockGuard类。它在构造时锁定，在析构时解锁(为了保证异常安全，在任何时候都不会死锁)
// you can change it to a template class
class SpinLockGuard final // 自旋锁RAII类,自动解锁
{
public:
    using this_type = SpinLockGuard; // 类型别名
    using spin_lock_type = SpinLock;

public:
    SpinLockGuard(spin_lock_type &lock) noexcept
        : m_lock(lock)
    {
        m_lock.lock();
    }

    ~SpinLockGuard() noexcept
    {
        m_lock.unlock();
    }

public:
    SpinLockGuard(const this_type &) = delete; // 禁止拷贝
    SpinLockGuard &operator=(const this_type &) = delete;

private:
    spin_lock_type &m_lock;
};

END_NAMESPACE(cpp_study)

#endif //_SPIN_LOCK_HPP

/*
自旋锁RAII实现,可以在多线程应用里保护共享的数据，避免数据竞争
*/