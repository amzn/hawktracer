#include "internal/thread.h"

#include <atomic>

struct ThreadIdGenerator
{
    ThreadIdGenerator()
    {
        _current_thread_id = ++_thread_id;
    }

    inline HT_ThreadId get_id() { return _thread_id; }

private:
    static std::atomic<HT_ThreadId> _thread_id;
    HT_ThreadId _current_thread_id;
};

std::atomic<HT_ThreadId> ThreadIdGenerator::_thread_id =  ATOMIC_VAR_INIT(0);

HT_ThreadId
ht_thread_get_current_thread_id(void)
{
    static thread_local ThreadIdGenerator generator;

    return generator.get_id();
}
