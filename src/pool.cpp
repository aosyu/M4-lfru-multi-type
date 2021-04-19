#include "pool.h"

#include <assert.h>
#include <cstddef>
#include <new>
#include <vector>

using std::size_t;

namespace pool {
class Pool
{
public:
    Pool(const size_t obj_size, const size_t obj_count)
        : m_obj_size(obj_size)
        , m_storage(obj_size * obj_count)
        , m_used_map(obj_count)
    {
    }

    void * allocate();

    void deallocate(const void * ptr);

    bool contains(const void * ptr);

private:
    static constexpr size_t npos = static_cast<size_t>(-1);

    size_t find_empty_place() const;

    const size_t m_obj_size;
    std::vector<std::byte> m_storage;
    std::vector<bool> m_used_map;
};

size_t Pool::find_empty_place() const
{
    for (size_t i = 0; i < m_used_map.size(); ++i) {
        if (!m_used_map[i]) {
            return i;
        }
    }
    return npos;
}

void * Pool::allocate()
{
    const size_t pos = find_empty_place();
    if (pos != npos) {
        m_used_map[pos] = true;
        return &m_storage[pos * m_obj_size];
    }
    throw std::bad_alloc{};
}

void Pool::deallocate(const void * ptr)
{
    auto b_ptr = static_cast<const std::byte *>(ptr);
    const auto begin = &m_storage[0];
    if (b_ptr >= begin) {
        const size_t offset = (b_ptr - begin) / m_obj_size;
        m_used_map[offset] = false;
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