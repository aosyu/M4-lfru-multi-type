#pragma once

#include <cstddef>
#include <initializer_list>
#include <map>
#include <new>
#include <vector>

namespace pool {

class Pool;

Pool * create_pool(std::size_t obj_size, std::size_t obj_count);

void * allocate(Pool & pool);

bool contains(Pool & pool, const void * ptr);

void deallocate(Pool & pool, const void * ptr, std::size_t n);

void destroy_pool(Pool * pool);

} // namespace pool

class PoolAllocator
{
public:
    PoolAllocator(const std::size_t count, std::initializer_list<std::size_t> sizes)
    {
        for (const size_t cur : sizes) {
            pools[cur] = pool::create_pool(cur, count / cur);
        }
    }

    void * allocate(std::size_t n)
    {
        if (pools.find(n) != pools.end()) {
            return pool::allocate(*pools[n]);
        }
        throw std::bad_alloc{};
    }

    void deallocate(const void * ptr)
    {
        for (const auto pool : pools) {
            if (pool::contains(*pool.second, ptr)) {
                pool::deallocate(*pool.second, ptr, pool.first);
            }
        }
    }

    virtual ~PoolAllocator()
    {
        for (auto pool : pools) {
            pool::destroy_pool(pool.second);
        }
    }

private:
    std::map<size_t, pool::Pool *> pools;
};