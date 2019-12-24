/**
 * @file push-counter-feature.h
 * Example timeline feature declaration.
 */

#ifndef PUSH_COUNTER_FEATURE_H
#define PUSH_COUNTER_FEATURE_H

#include <hawktracer.h>

HT_DECLS_BEGIN

/*
 * Registers feature in HawkTracer system.
 * The definition of the function is automatically generated
 * by HT_FEATURE_DEFINE() macro.
 */
HT_ErrorCode PushCounterFeature_register(void);

/*
 * Helper method for enabling a feature for a specific timeline object
 */
HT_ErrorCode push_counter_feature_enable(HT_Timeline* timeline);

/*
 * Feature method - pushes an event to a timeline and increments the counter.
 * The method is a wrapper for ht_timeline_push_event()
 */
void push_counter_feature_push_event(HT_Timeline* timeline, HT_Event* event);

/*
 * Feature method - returns count of events pushed to the timeline
 */
int push_counter_feature_get_count(HT_Timeline* timeline);

HT_DECLS_END

#endif /* PUSH_COUNTER_FEATURE_H */
