#include "pool.h"

#include <cstddef>
#include <new>
#include <set>
#include <vector>

using std::size_t;

namespace pool {
class Pool
{
public:
    Pool(const size_t obj_size, const size_t obj_count)
        : m_obj_size(obj_size)
        , m_storage(obj_size * obj_count)
        , m_used_map()
    {
        for (size_t x = 0; x < obj_count; x++) {
            m_used_map.insert(x);
        }
    }

    void * allocate();

    void deallocate(const void * ptr);

    bool contains(const void * ptr);

private:
    const size_t m_obj_size;
    std::vector<std::byte> m_storage;
    std::set<std::size_t> m_used_map;
};

void * Pool::allocate()
{
    if (!m_used_map.empty()) {
        const size_t pos = *m_used_map.begin();
        m_used_map.erase(m_used_map.begin());
        return &m_storage[pos * m_obj_size];
    }
    throw std::bad_alloc{};
}

void Pool::deallocate(const void * ptr)
{
    auto b_ptr = static_cast<const std::byte *>(ptr);
    const auto begin = &m_storage[0];
    if (b_ptr >= begin) {
        m_used_map.insert((b_ptr - begin) / m_obj_size);
    }
}

bool Pool::contains(const void * ptr)
{
    auto b_ptr = static_cast<const std::byte *>(ptr);
    return &m_storage[0] <= b_ptr && &m_storage[m_storage.size() - 1] >= b_ptr;
}

Pool * create_pool(const size_t obj_size, const size_t obj_count)
{
    return new Pool(obj_size, obj_count);
}

void * allocate(Pool & pool)
{
    return pool.allocate();
}

void deallocate(Pool & pool, const void * ptr)
{
    pool.deallocate(ptr);
}

bool contains(Pool & pool, const void * ptr)
{
    return pool.contains(ptr);
}

void destroy_pool(Pool * pool)
{
    delete pool;
}

} // namespace pool