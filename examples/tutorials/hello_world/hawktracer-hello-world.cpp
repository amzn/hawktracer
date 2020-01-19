/**
 * @file hawktracer-hello-world.cpp
 * The file demonstrates how to use HawkTracer in GNU C/C++ applications.
 * Please note this file won't compile in for other than GNU C or C++ compilers.
 */
#include <hawktracer.h>
#include <cstdio>
#include <string>

static void hello_world()
{
    /* A function scoped tracepoint - measures a duration of the scope
     * and automatically labels itself with the function name (i.e. "hello_world")
     */
    HT_G_TRACE_FUNCTION()

    for (int i = 0; i < 100; i++)
    {
        /* Another scoped tracepoint, but we can set the label manually */
        HT_G_TRACE_OPT_STATIC("iteration")
        printf("2 * %d = %d\n", i, 2*i);

        /* Dynamic string used as a label */
        std::string txt = "Iteration (mod 10): " + std::to_string(i % 10);
        HT_G_TRACE_OPT_DYNAMIC(txt.c_str());
        printf("text: %s\n", txt.c_str());
    }
}

int main(int argc, char** argv)
{
    /* initialize HawkTracer library */
    ht_init(argc, argv);

    HT_ErrorCode error_code;
    /* Create a listener, it'll handle all the HawkTracer events */
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

