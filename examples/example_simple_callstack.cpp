#include <hawktracer.h>

static void fnc_bar()
{
    HT_TP_GLOBAL_SCOPED_STRING("bar");
}

static void fnc_foo()
{
    HT_TP_GLOBAL_SCOPED_STRING("foo");

    for (int i = 0; i < 16; i++)
    {
        fnc_bar();
    }
}

static void fnc_start()
{
    HT_TP_GLOBAL_SCOPED_STRING("start");

    for (int i = 0; i < 32; i++)
    {
        fnc_foo();
    }
}

int main(int argc, char** argv)
{
    ht_init(argc, argv);

    HT_FileDumpListener file_dump_listener;
    if (ht_file_dump_listener_init(&file_dump_listener, "test_output.htdump", 4096u) != HT_FALSE)
    {
        ht_timeline_register_listener(ht_global_timeline_get(), ht_file_dump_listener_callback, &file_dump_listener);
    }

    ht_registry_push_all_klass_info_events(ht_global_timeline_get());

    fnc_start();

    ht_timeline_flush(ht_global_timeline_get());
    ht_timeline_unregister_all_listeners(ht_global_timeline_get());
    ht_file_dump_listener_deinit(&file_dump_listener);
    ht_deinit();
}
