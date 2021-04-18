#pragma once

#include <cstddef>
#include <initializer_list>
#include <map>
#include <new>
#include <vector>

namespace pool {

class Pool;

Pool * create_pool(std::size_t obj_size, std::size_t obj_count);

void destroy_pool(Pool * pool);

std::size_t pool_obj_size(const Pool & pool);

void * allocate(Pool & pool, std::size_t n);

bool contains(Pool & pool, const void * ptr);

void deallocate(Pool & pool, const void * ptr, std::size_t n);

} // namespace pool

class PoolAllocator
{
public:
    PoolAllocator(const std::size_t count, std::initializer_list<std::size_t> sizes)
        : m_count(count)
    {
        for (size_t cur : sizes) {
            pools[cur] = pool::create_pool(cur, count / cur);
        }
    }

    void * allocate(std::size_t n)
    {
        if (pools.find(n) != pools.end()) {
            return pool::allocate(*pools[n], 1);
        }
        //        return nullptr;
        throw std::bad_alloc{};
    }

    void deallocate(const void * ptr)
    {
        for (auto pool : pools) {
            if (pool::contains(*pool.second, ptr)) {
                pool::deallocate(*pool.second, ptr, pool.first);
            }
        }
    }

private:
    //    std::initializer_list<std::size_t> m_sizes;
    std::size_t m_count;
    std::map<size_t, pool::Pool *> pools;
};