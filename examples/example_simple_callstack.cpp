#include <hawktracer/listeners/file_dump_listener.h>
#include <hawktracer/simple_init.h>

static void fnc_bar()
{
    HT_TP_GLOBAL_SCOPED_STRING("bar");
}

static void fnc_foo()
{
    HT_TP_GLOBAL_SCOPED_STRING("foo");

    for (int i = 0; i < 2048; i++)
    {
        fnc_bar();
    }
}

static void fnc_start()
{
    HT_TP_GLOBAL_SCOPED_STRING("start");

    for (int i = 0; i < 4096; i++)
    {
        fnc_foo();
    }
}

int main(int argc, char** argv)
{
    HT_SIMPLE_INIT(argc, argv,
                   (HT_FILE_DUMP_LISTENER_INIT, file_dump_listener, "test_output.htdump"));

    fnc_start();

    HT_SIMPLE_DEINIT(
                (HT_FILE_DUMP_LISTENER_DEINIT, file_dump_listener));
}
