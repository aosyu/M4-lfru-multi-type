#pragma once

#include <algorithm>
#include <cstddef>
#include <deque>
#include <list>
#include <new>
#include <ostream>

template <class Key, class KeyProvider, class Allocator>
class Cache
{
public:
    template <class... AllocArgs>
    Cache(const std::size_t cache_size, AllocArgs &&... alloc_args)
        : m_max_top_size(cache_size)
        , m_max_low_size(cache_size)
        , m_alloc(std::forward<AllocArgs>(alloc_args)...)
    {
    }

    std::size_t size() const
    {
        return m_top_queue.size() + m_low_queue.size();
    }

    bool empty() const
    {
        return m_top_queue.empty() && m_low_queue.empty();
    }

    template <class T>
    T & get(const Key & key);

    std::ostream & print(std::ostream & strm) const;

    friend std::ostream & operator<<(std::ostream & strm, const Cache & cache)
    {
        return cache.print(strm);
    }

private:
    const std::size_t m_max_top_size;
    const std::size_t m_max_low_size;
    Allocator m_alloc;
    std::deque<KeyProvider *> m_top_queue;
    std::deque<KeyProvider *> m_low_queue;
};

template <class Key, class KeyProvider>
auto find_element(std::deque<KeyProvider *> & m_queue, const Key & key)
{
    return std::find_if(m_queue.begin(), m_queue.end(), [&key](const KeyProvider * elem) {
        return *elem == key;
    });
}

template <class Key, class KeyProvider, class Allocator>
template <class T>
inline T & Cache<Key, KeyProvider, Allocator>::get(const Key & key)
{
    auto it = find_element(m_top_queue, key);

    if (it != m_top_queue.end()) {
        m_top_queue.push_front(*it);
        m_top_queue.erase(it);
    }
    else {
        it = find_element(m_low_queue, key);

        if (it != m_low_queue.end()) {
            if (m_top_queue.size() == m_max_top_size) {
                m_low_queue.push_front(m_top_queue.back());
                m_top_queue.pop_back();
            }
            m_top_queue.push_front(*it);

            m_low_queue.erase(it);
        }
        else {
            if (m_low_queue.size() == m_max_low_size) {
                m_alloc.template destroy<KeyProvider>(m_low_queue.back());
                m_low_queue.pop_back();
            }

            m_low_queue.push_front(m_alloc.template create<T>(key));
            return *static_cast<T *>(m_low_queue.front());
        }
    }

    return *static_cast<T *>(m_top_queue.front());
}

template <class Key, class KeyProvider, class Allocator>
inline std::ostream & Cache<Key, KeyProvider, Allocator>::print(std::ostream & strm) const
{
    return strm << "Priority: <empty>"
                << "\nRegular: <empty>"
                << "\n";
}
