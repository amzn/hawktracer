#ifndef HAWKTRACER_POSIX_MAPPED_TRACEPOINT_H
#define HAWKTRACER_POSIX_MAPPED_TRACEPOINT_H

#include <hawktracer/ht_config.h>

#ifdef HT_USE_PTHREADS

#include <hawktracer/timeline.h>

HT_DECLS_BEGIN

HT_API void ht_pthread_mapped_tracepoint_enter(HT_Timeline* timeline, const char* label);

HT_API void ht_pthread_mapped_tracepoint_leave(void);

HT_API void ht_pthread_mapped_tracepoint_add_mapping(void);

void _ht_posix_mapped_tracepoint_init(void);

void _ht_posix_mapped_tracepoint_deinit(void);

HT_DECLS_END

#endif /* HT_USE_PTHREADS */

#endif /* HAWKTRACER_POSIX_MAPPED_TRACEPOINT_H */
