#include "hawktracer/memory_usage.h"
#include "hawktracer/alloc.h"

#include <Windows.h>
#include <Psapi.h>

struct _HT_MemoryUsageContext
{
    HANDLE process;
};

HT_MemoryUsageContext*
ht_memory_usage_context_create(void* process_id)
{
    HT_MemoryUsageContext* context = HT_CREATE_TYPE(HT_MemoryUsageContext);

    if (context == NULL)
    {
        return NULL;
    }

    context->process = process_id == NULL ? GetCurrentProcess() : process_id;
    return context;
}

void
ht_memory_usage_context_destroy(HT_MemoryUsageContext* context)
{
    ht_free(context);
}

HT_ErrorCode
ht_memory_usage_get_usage(HT_MemoryUsageContext* context,
                          size_t* virtual_memory_bytes,
                          size_t* shared_memory_bytes,
                          size_t* resident_memory_bytes)
{
    PROCESS_MEMORY_COUNTERS_EX pmc;

    if (!context || !context->process)
    {
        return HT_ERR_INVALID_ARGUMENT;
    }

    if (!virtual_memory_bytes && !shared_memory_bytes && !resident_memory_bytes)
    {
        return HT_ERR_OK;
    }

    if (GetProcessMemoryInfo(context->process, &pmc, sizeof(pmc)))
    {
        if (virtual_memory_bytes)
        {
            *virtual_memory_bytes = 0;
        }
        if (shared_memory_bytes)
        {
            *shared_memory_bytes = 0;
        }
        if (resident_memory_bytes)
        {
            *resident_memory_bytes = pmc.WorkingSetSize;
        }
        return HT_ERR_OK;
    }
    else
    {
        return HT_ERR_UNKNOWN;
    }
}
