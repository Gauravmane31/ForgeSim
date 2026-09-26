#include "ForgeSim/execution/CacheManager.hpp"

#include <stdexcept>


void CacheManager::store(
    int nodeId,
    const Result& result
)
{
    cache[nodeId] = result;
}


bool CacheManager::contains(
    int nodeId
) const
{
    auto it = cache.find(nodeId);

    if (it == cache.end())
    {
        return false;
    }

    return it->second.isValid();
}


const Result& CacheManager::get(
    int nodeId
) const
{
    auto it = cache.find(nodeId);

    if (it == cache.end())
    {
        throw std::runtime_error(
            "No cached result exists for node: " +
            std::to_string(nodeId)
        );
    }

    if (!it->second.isValid())
    {
        throw std::runtime_error(
            "Cached result is invalid for node: " +
            std::to_string(nodeId)
        );
    }

    return it->second;
}


void CacheManager::invalidate(
    int nodeId
)
{
    auto it = cache.find(nodeId);

    if (it != cache.end())
    {
        it->second.invalidate();
    }
}


void CacheManager::clear()
{
    cache.clear();
}


std::size_t CacheManager::size() const
{
    return cache.size();
}