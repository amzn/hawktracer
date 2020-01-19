/**
 * @file hawktracer-hello-world.c
 * The file demonstrates how to use HawkTracer in C/C++ applications.
 */
#include <hawktracer.h>
#include <stdio.h>

static void hello_world(void)
{
    /* We start measuring the code */
    ht_feature_callstack_start_string(ht_global_timeline_get(), "hello_world()");

    for (int i = 0; i < 100; i++)
    {
        /* Another tracepoint, we want to measure printf() function */
        ht_feature_callstack_start_string(ht_global_timeline_get(), "printf()");
        printf("2 * %d = %d\n", i, 2*i);
        /* Stop measurements for printf() */
        ht_feature_callstack_stop(ht_global_timeline_get());
    }
    /* Stop measurements for the first tracepoint (hello_world()) */
    ht_feature_callstack_stop(ht_global_timeline_get());
}

int main(int argc, char** argv)
{
    /* initialize HawkTracer library */
    ht_init(argc, argv);

    HT_ErrorCode error_code;
    /* Create a listener and register it to a timeline, it'll handle all the HawkTracer events */
    ht_file_dump_listener_register(
                ht_global_timeline_get(), "hello-world-out.htdump", 2048, &error_code);

    /* Creating listener might fail (e.g. file can't be open),
     * so we have to check the status
     */
    if (error_code != HT_ERR_OK)
    {
        printf("Unable to create listener. Error code: %d\n", error_code);
        ht_deinit();
        return -1;
    }

    /* Run the actual code */
    hello_world();

    /* Uninitialize HawkTracer library */
    ht_deinit();

    return 0;
}

