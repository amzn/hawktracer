/**
 * @file push-counter-feature.h
 * Example timeline feature definition.
 */

#include "push-counter-feature.h"

#include <stdio.h>

/* feature object. This should contain all fields that will be used
 * by the feature methods */
typedef struct
{
    /* required field - the feature must inherit from HT_Feature class */
    HT_Feature base;
    /* other feature members go here. we'll be using count field for
     * counting number of events being pushed to the timeline
     */
    int counter;
} PushCounterFeature;

static void push_counter_feature_destroy(HT_Feature* feature);

/* the macro defines a few helper methods for the feature.
 * All the methods are prefixed with PushCounterFeature_
*/
HT_FEATURE_DEFINE(PushCounterFeature, push_counter_feature_destroy)

/* A helper method for attaching the feature to the timeline.
 * User can simply call this function with her timeline object
 * as a parameter to enable the feature for that timeline
 */
HT_ErrorCode push_counter_feature_enable(HT_Timeline* timeline)
{
    /* Allocate memory for the feature. This method should be used for
     * allocating the object, as it not only allocates memory, but
     * initializes base HT_Feature member.
     */
    PushCounterFeature* feature = PushCounterFeature_alloc();

    if (feature == NULL)
    {
        return HT_ERR_OUT_OF_MEMORY;
    }

    /* Initialize all the fields of the feature. In this case, we reset the counter */
    feature->counter = 0;

    /* The function attaches the feature to a timeline */
    return ht_timeline_set_feature(timeline, (HT_Feature*)feature);
}

void push_counter_feature_push_event(HT_Timeline* timeline, HT_Event* event)
{
    /* Obtain the feature from the timeline */
    PushCounterFeature* feature = PushCounterFeature_from_timeline(timeline);

    /* The feature might not be enabled for the pipeline, so this needs to be checked. */
    if (!feature)
    {
        printf("Feature has not been enabled for the pipeline\n");
        return;
    }

    /* Perform the feature's logic */
    ht_timeline_push_event(timeline, event);
    feature->counter++;
}

int push_counter_feature_get_count(HT_Timeline* timeline)
{
    /* Obtain the feature from the timeline */
    PushCounterFeature* feature = PushCounterFeature_from_timeline(timeline);

    /* The feature might not be enabled for the pipeline, so this needs to be checked. */
    if (!feature)
    {
        return -1;
    }

    return feature->counter;
}

/* Destroys the feature object */
static void push_counter_feature_destroy(HT_Feature* feature)
{
    PushCounterFeature* push_counter_feature = (PushCounterFeature*)feature;

    /* Here all the resources of the feature should be released */

    ht_free(push_counter_feature);
}
