#include <hawktracer/timeline.h>

#include <hawktracer/init.h>

/* For thread-local timelines there might not be
 * a way to deinit it before ht_deinit(). Therefore
 * we need to make sure that they can safely be
 * destroyed after ht_deinit().
 */
int main(int argc, char** argv)
{
    ht_init(argc, argv);

    HT_Timeline timeline;
    ht_timeline_init(&timeline, 10, HT_FALSE, HT_FALSE, NULL);

    ht_deinit();

    ht_timeline_deinit(&timeline);

    return 0;
}
