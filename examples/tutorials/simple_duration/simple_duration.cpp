#include <hawktracer.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    ht_init(argc, argv);

    HT_FileDumpListener* file_dump_listener = ht_file_dump_listener_create("test_output.htdump", 4096u, NULL);
    if (file_dump_listener != NULL)
    {
        ht_timeline_register_listener(ht_global_timeline_get(), ht_file_dump_listener_callback, file_dump_listener);
    }

    ht_feature_duration_enable(ht_global_timeline_get());

    const int NUM_MEASUREMENTS = 10;
    HT_DurationId ids[NUM_MEASUREMENTS];
    char names[NUM_MEASUREMENTS][128];

    printf ("Gathering %d measurements\n", NUM_MEASUREMENTS);
    for (int i = 0; i < NUM_MEASUREMENTS; i++) 
    {
        sprintf(names[i], "Measurement_name_%d", i);
        printf("Starting measurement %d, named %s\n", i, names[i]);
        ids[i] = ht_feature_duration_start_string(ht_global_timeline_get(), names[i]);
        sleep(1);
    }

    for (int i = 0; i < NUM_MEASUREMENTS; i++) 
    {
        HT_DurationNs duration = ht_feature_duration_stop(ht_global_timeline_get(), ids[i]);
        printf("ID: %llu, Duration: %llu nanoseconds.\n", ids[i], duration);
    }

    ht_timeline_flush(ht_global_timeline_get());
    ht_timeline_unregister_all_listeners(ht_global_timeline_get());
    ht_file_dump_listener_destroy(file_dump_listener);
    ht_feature_duration_disable(ht_global_timeline_get());

    ht_deinit();

    return 0;
}
