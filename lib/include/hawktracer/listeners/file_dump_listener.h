/** @file
 * The listener creates a new file and stores all the incomming
 * events in it. The listener can handle both serialized and
 * unserialized event streams.
 */

#ifndef HAWKTRACER_LISTENERS_FILE_DUMP_LISTENER_H
#define HAWKTRACER_LISTENERS_FILE_DUMP_LISTENER_H

#include <hawktracer/timeline.h>

#include <stdio.h>

HT_DECLS_BEGIN

typedef struct _HT_FileDumpListener HT_FileDumpListener;

/**
 * Creates a file dump listener and registers it to a timeline.
 *
 * This is a helper function that wrapps ht_file_dump_listener_create() and
 * ht_timeline_register_listener_full(). The user can achieve the same result
 * by explicitly creating a listener and registering it to a timeline:
 *
 * @code
 * HT_FileDumpListener* listener = ht_file_dump_listener_create("file.htdump", 2048, &error_code);
 * ht_timeline_register_listener_full(
 *             timeline,
 *             ht_file_dump_listener_callback,
 *             listener,
 *             (HT_DestroyCallback)ht_file_dump_listener_destroy);
 * @endcode
 *
 * Please note the example above doesn't implement error handling, which would make the code
 * even more complicated.
 *
 * @param timeline the timeline where the listener will be attached to.
 * @param filename a name of the file to store the data in.
 * @param buffer_size a size of the internal buffer.
 * @param out_err a pointer to an error code variable where the error will be stored if the operation fails.
 *
 * @return a pointer to a new instance of the listener.
 */
HT_API HT_FileDumpListener* ht_file_dump_listener_register(
        HT_Timeline* timeline, const char* filename, size_t buffer_size, HT_ErrorCode *out_err);

/**
 * Creates an instance of a file dump listener.
 *
 * Internal buffer can be disabled by setting @a buffer_size parameter to 0. However,
 * the internal buffer must not be disabled if event stream is not serialized - this setting
 * causes undefined behavior.
 *
 * @param filename a name of the file to store the data in.
 * @param buffer_size a size of the internal buffer.
 * @param out_err a pointer to an error code variable where the error will be stored if the operation fails.
 *
 * @return a pointer to a new instance of the listener.
 */
HT_API HT_FileDumpListener* ht_file_dump_listener_create(const char* filename, size_t buffer_size, HT_ErrorCode *out_err);

/**
 * Destroys an instance of the listener.
 *
 * @param listener a pointer to the listener to be destroyed.
 */
HT_API void ht_file_dump_listener_destroy(HT_FileDumpListener* listener);

/**
 * A listener callback.
 *
 * This callback should be used for the ht_timeline_register_listener() function.
 */
HT_API void ht_file_dump_listener_callback(TEventPtr events, size_t size, HT_Boolean serialized, void* user_data);

/**
 * Flushes internal listener buffer.
 *
 * The listener is normally flushed when the internal buffer is full
 * or when ht_file_dump_listener_destroy() is called. However, user
 * might want to forcefully flush the data into a file if needed using
 * this function.
 *
 * @param listener a pointer to the listener.
 * @param flush_stream #HT_TRUE if the function should also flush internal FILE buffer; if not, set to #HT_FALSE
 *
 * @return #HT_ERR_OK if the operation completed successfully; otherwise, appropriate error code is returned.
 * This function can potentially fail only if @a flush_stream parameter is set to #HT_TRUE.
 */
HT_API HT_ErrorCode ht_file_dump_listener_flush(HT_FileDumpListener* listener, HT_Boolean flush_stream);

/**
 * Stops listening to new events.
 *
 * The function is very similar to ht_file_dump_listener_destroy() except it does not
 * release the memory allocated for @a listener object.
 * After stopping the listener, there's no way to resume it. User needs to
 * create another listener and register it for the timeline instead.
 *
 * @param listener the listener.
 */
HT_API void ht_file_dump_listener_stop(HT_FileDumpListener* listener);

HT_DECLS_END

#endif /* HAWKTRACER_LISTENERS_FILE_DUMP_LISTENER_H */
