#include "hawktracer/memory_usage.h"
#include "hawktracer/alloc.h"

#include <unistd.h>
#include <stdio.h>

struct _HT_MemoryUsageContext
{
    int pid;
};

HT_MemoryUsageContext*
ht_memory_usage_context_create(void* process_id)
{
    HT_MemoryUsageContext* context = HT_CREATE_TYPE(HT_MemoryUsageContext);

    if (context == NULL)
    {
        return NULL;
    }

    context->pid = process_id == NULL ? getpid() : *(int*)process_id;
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
    char buf[64];
    long page_size;
    unsigned long long virt_mem;
    unsigned long long resident_mem;
    unsigned long long shared_mem;
    FILE* fp;

    if (!virtual_memory_bytes && !shared_memory_bytes && !resident_memory_bytes)
    {
        return HT_ERR_OK;
    }

    snprintf(buf, 32, "/proc/%d/statm", context->pid);

    if ((fp = fopen(buf, "r")) == NULL)
    {
        return HT_ERR_CANT_OPEN_FILE;
    }

    int num = fread(buf, 1, sizeof(buf) - 1, fp);
    fclose(fp);

    if (num < 16)
    {
        return HT_ERR_INVALID_FORMAT;
    }

    buf[num] = '\0';

    if (sscanf(buf, "%llu %llu %llu", &virt_mem, &resident_mem, &shared_mem) != 3)
    {
        return HT_ERR_INVALID_FORMAT;
    }

    page_size = sysconf(_SC_PAGESIZE);

    if (virtual_memory_bytes)
    {
        *virtual_memory_bytes = virt_mem * page_size;
    }
    if (virtual_memory_bytes)
    {
        *resident_memory_bytes = resident_mem * page_size;
    }
    if (virtual_memory_bytes)
    {
        *shared_memory_bytes = shared_mem * page_size;
    }

    return HT_ERR_OK;
}
