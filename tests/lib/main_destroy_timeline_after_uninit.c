#include <hawktracer/timeline.h>

#include <hawktracer/init.h>

/* For thread-local timelines there might not be
 * a way to deinit it before ht_deinit(). Therefore
 * we need to make sure that they can safely be
 * destroyed after ht_deinit().
 */
int main(int argc, char** argv)
{
    if (ht_is_initialized())
    {
        return 1;
    }

    ht_init(argc, argv);

    if (!ht_is_initialized())
    {
        return 1;
    }

    HT_Timeline* timeline = ht_timeline_create(10, HT_FALSE, HT_FALSE, NULL, NULL);

    ht_deinit();

    if (ht_is_initialized())
    {
        return 1;
    }

    ht_timeline_destroy(timeline);

    return 0;
}
