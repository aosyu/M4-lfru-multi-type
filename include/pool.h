#pragma once

#include <cstddef>
#include <initializer_list>
#include <new>

class PoolAllocator
{
public:
    PoolAllocator(const std::size_t /*count*/, std::initializer_list<std::size_t> /*sizes*/)
    {
    }

    void * allocate(const std::size_t /*n*/)
    {
        throw std::bad_alloc{};
    }

    void deallocate(const void * /*ptr*/)
    {
    }
};
