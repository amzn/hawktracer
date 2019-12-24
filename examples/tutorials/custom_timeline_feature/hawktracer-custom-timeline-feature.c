/**
 * @file hawktracer-custom-timeline-feature.c
 * The file demonstrates how to use custom timeline feature.
 */

/* Contains declaration of custome feature's API */
#include "push-counter-feature.h"

#include <hawktracer.h>
#include <stdio.h>

int main(int argc, char** argv)
{
    /* initialize HawkTracer library */
    ht_init(argc, argv);

    /* register our custom feature */
    PushCounterFeature_register();

    HT_ErrorCode error_code;
    /* create timeline object. Instead, global timeline could be used (ht_global_timeline_get()) */
    HT_Timeline* timeline = ht_timeline_create(1024, HT_FALSE, HT_FALSE, NULL, &error_code);
    if (!timeline)
    {
        printf("Failed to create timeline object: %d\n", error_code);
        return 1;
    }

    /* attach the feature to the timeline object */
    push_counter_feature_enable(timeline);

    /* push 10 events to the timeline through the feature method */
    for (int i = 0; i < 10; i++)
    {
        HT_DECL_EVENT(HT_Event, event)
        push_counter_feature_push_event(timeline, &event);
    }

    printf("Number of events pushed to the timeline: %d\n",
           push_counter_feature_get_count(timeline));

    /* destroy the timeline. It will also destroy the feature */
    ht_timeline_destroy(timeline);

    return 0;
}
