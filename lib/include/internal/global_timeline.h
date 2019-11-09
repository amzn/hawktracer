#ifndef HAWKTRACER_INTERNAL_GLOBAL_TIMELINE_H
#define HAWKTRACER_INTERNAL_GLOBAL_TIMELINE_H

#include <hawktracer/macros.h>

#include <stddef.h>

HT_DECLS_BEGIN

void ht_global_timeline_set_buffer_size(size_t buffer_size);

size_t ht_global_timeline_get_buffer_size(void);

HT_DECLS_END

#endif /* HAWKTRACER_INTERNAL_GLOBAL_TIMELINE_H */
