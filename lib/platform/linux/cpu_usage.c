#include "hawktracer/cpu_usage.h"
#include "hawktracer/alloc.h"
#include "hawktracer/monotonic_clock.h"

#include <pthread.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>

struct _HT_CPUUsageContext
{
    HT_TimestampNs last_probe;
    unsigned long last_total_time;
    int pid;
    float value;
};

static pthread_once_t _clock_ticks_per_sec_once_control = PTHREAD_ONCE_INIT;
static long _clock_ticks_per_sec = -1;

static void
_init_clock_ticks_per_sec(void)
{
    _clock_ticks_per_sec = sysconf(_SC_CLK_TCK);
}

static HT_Boolean
_get_process_time(unsigned long* total_time, int pid)
{
    char state;
    int int_data[5];
    unsigned int flags;
    unsigned long int long_data[3];
    unsigned long int stime;
    long int cutime;
    char buf[1024];

    snprintf(buf, 32, "/proc/%d/stat", pid);
    FILE* fp;
    if ((fp = fopen(buf, "r")) == NULL)
    {
        return HT_FALSE;
    }

    int num = fread(buf, 1, sizeof(buf) - 1, fp);
    fclose(fp);

    if (num < 80)
    {
        return HT_FALSE;
    }
    buf[num] = '\0';

    /* Read stat file: http://man7.org/linux/man-pages/man5/proc.5.html */
    sscanf(strrchr(buf, ')') + 2, /* pid (comm) */
           "%c " /* state */
           "%d %d %d %d %d " /* ppid pgrp session tty_nr tpgid */
           "%u " /* flags */
           "%lu %lu %lu %lu " /* minflt cminflt cmajflt utime */
           "%lu " /* stime */
           "%ld", /* cutime */
           &state,
           &int_data[0], &int_data[1], &int_data[2], &int_data[3], &int_data[4],
            &flags,
            &long_data[0], &long_data[1], &long_data[2], &long_data[3],
            &stime,
            &cutime);

    *total_time = stime + cutime;

    return HT_TRUE;
}


HT_CPUUsageContext*
ht_cpu_usage_context_create(void* process_id)
{
    HT_CPUUsageContext* context = NULL;

    int status = pthread_once(&_clock_ticks_per_sec_once_control, _init_clock_ticks_per_sec);

    if (status != 0)
    {
        return NULL;
    }

    context = HT_CREATE_TYPE(HT_CPUUsageContext);

    if (context == NULL)
    {
        return NULL;
    }

    context->last_probe = 0;
    context->pid = process_id == NULL ? getpid() : *(int*)process_id;
    context->last_total_time = 0;

    ht_cpu_usage_get_percentage(context);
    context->value = 0.0f;

    return context;
}

void
ht_cpu_usage_context_destroy(HT_CPUUsageContext* context)
{
    ht_free(context);
}

float
ht_cpu_usage_get_percentage(HT_CPUUsageContext* context)
{
    HT_TimestampNs current_ts = ht_monotonic_clock_get_timestamp();
    unsigned long total_time;

    long tics;
    float elapsed;

    if (_get_process_time(&total_time, context->pid) == HT_FALSE)
    {
        return -1.0f;
    }

    tics = total_time - context->last_total_time;
    elapsed = (current_ts - context->last_probe) / 1000000000.0f;

    context->value = tics * 100.0f / (_clock_ticks_per_sec * elapsed);
    context->last_total_time = total_time;
    context->last_probe = current_ts;

    return context->value;
}
