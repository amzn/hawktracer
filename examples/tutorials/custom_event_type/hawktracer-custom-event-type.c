/**
 * @file hawktracer-custom-event-type.c
 * The file demonstrates how to define custom event types in HawkTracer system.
 */
#include <hawktracer.h>

/* The header must be included for defining events */
#include <hawktracer/event_macros_impl.h>

#include <stdio.h>

/* Define base file operation event class */
HT_DECLARE_EVENT_KLASS(FileOperationEvent, HT_Event,
                       (INTEGER, uint32_t, opened_files_count),
                       (INTEGER, HT_Boolean, success))

/* Define event for OpenFile operation */
HT_DECLARE_EVENT_KLASS(OpenFileEvent, FileOperationEvent,
                       (STRING, const char*, name),
                       (STRING, const char*, mode))

/* Define event for CloseFile operation */
HT_DECLARE_EVENT_KLASS(CloseFileEvent, FileOperationEvent,
                       (STRING, const char*, label))

/* Global variable for holding the number of files currently open by the program */
static uint32_t opened_files_count = 0;

static FILE* my_fopen(const char* file_name, const char* mode)
{
    FILE* f_ptr;
    HT_Boolean success = HT_FALSE;

    f_ptr = fopen(file_name, mode);

    if (f_ptr)
    {
        opened_files_count++;
        success = HT_TRUE;
    }

    /* Push OpenFileEvent to a global timeline */
    HT_TIMELINE_PUSH_EVENT(ht_global_timeline_get(), OpenFileEvent, opened_files_count, success, file_name, mode);

    return f_ptr;
}

static void my_fclose(FILE* f_ptr, const char* marker)
{
    HT_Boolean success = f_ptr != NULL;

    /* Push CloseFileEvent to a global timeline */
    HT_TIMELINE_PUSH_EVENT(ht_global_timeline_get(), CloseFileEvent, opened_files_count, success, marker);


    if (success)
    {
        opened_files_count--;
        fclose(f_ptr);
    }
}

static void tutorial_run(void)
{
    FILE* fp1 = my_fopen("existing_file", "r");
    FILE* fp2 = my_fopen("create-me.txt", "w");
    my_fclose(fp1, "fp1");
    FILE* fp3 = my_fopen("non_existing_file", "r");
    my_fclose(fp2, "fp2");
    my_fclose(fp3, "fp3");
}

int main(int argc, char** argv)
{
    /* initialize HawkTracer library */
    ht_init(argc, argv);

    /* Register all the new event types before first usage */
    HT_REGISTER_EVENT_KLASS(FileOperationEvent);
    HT_REGISTER_EVENT_KLASS(OpenFileEvent);
    HT_REGISTER_EVENT_KLASS(CloseFileEvent);

    HT_ErrorCode error_code;
    /* Create a listener, it'll handle all the HawkTracer events */
    HT_FileDumpListener* listener = ht_file_dump_listener_create("tutorial-custom-event-type.htdump", 2048, &error_code);

    /* Creating listener might fail (e.g. file can't be open),
     * so we have to check the status
     */
    if (!listener)
    {
        printf("Unable to create listener. Error code: %d\n", error_code);
        ht_deinit();
        return -1;
    }

    /* Register listener to the global timeline */
    ht_timeline_register_listener(ht_global_timeline_get(), ht_file_dump_listener_callback, listener);

    /* Run the actual code */
    tutorial_run();

    /* Flush all the buffered events in a timeline */
    ht_timeline_flush(ht_global_timeline_get());
    /* Unregister all the listeners from the global timeline, so we can safely destroy them */
    ht_timeline_unregister_all_listeners(ht_global_timeline_get());
    /* Destroy listeners */
    ht_file_dump_listener_destroy(listener);
    /* Uninitialize HawkTracer library */
    ht_deinit();
}
