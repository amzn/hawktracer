#ifndef HAWKTRACER_LISTENERS_FILE_DUMP_LISTENER_H
#define HAWKTRACER_LISTENERS_FILE_DUMP_LISTENER_H

#include <hawktracer/listener_buffer.h>

#include <stdio.h>

HT_DECLS_BEGIN

typedef struct _HT_FileDumpListener HT_FileDumpListener;

HT_API HT_FileDumpListener* ht_file_dump_listener_create(const char* filename, size_t buffer_size, HT_ErrorCode *out_err);

HT_API void ht_file_dump_listener_destroy(HT_FileDumpListener* listener);

HT_API void ht_file_dump_listener_callback(TEventPtr events, size_t size, HT_Boolean serialized, void* user_data);

HT_DECLS_END

#endif /* HAWKTRACER_LISTENERS_FILE_DUMP_LISTENER_H */
