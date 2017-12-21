#ifndef HAWKTRACER_LISTENERS_FILE_DUMP_LISTENER_H
#define HAWKTRACER_LISTENERS_FILE_DUMP_LISTENER_H

#include <hawktracer/base_types.h>

#include <stdio.h>

#define HT_FILE_DUMP_LISTENER_BUFFER_SIZE 4096u /* TODO make it configurable */

HT_DECLS_BEGIN

typedef struct
{
    HT_Byte buffer[HT_FILE_DUMP_LISTENER_BUFFER_SIZE];
    FILE* p_file;
    size_t buffer_usage;
} HT_FileDumpListener;

HT_Boolean ht_file_dump_listener_init(HT_FileDumpListener* listener, const char* filename);

void ht_file_dump_listener_deinit(HT_FileDumpListener* listener);

void ht_file_dump_listener_callback(TEventPtr events, size_t size, HT_Boolean serialized, void* user_data);

HT_DECLS_END


#define HT_FILE_DUMP_LISTENER_INIT(timeline, identifier, file_name) \
    HT_FileDumpListener identifier; \
    if (ht_file_dump_listener_init(&identifier, file_name) != HT_FALSE) \
    { \
        ht_timeline_register_listener(timeline, ht_file_dump_listener_callback, &identifier); \
    }

#define HT_FILE_DUMP_LISTENER_DEINIT(identifier) \
    ht_file_dump_listener_deinit(&identifier);

#endif /* HAWKTRACER_LISTENERS_FILE_DUMP_LISTENER_H */
