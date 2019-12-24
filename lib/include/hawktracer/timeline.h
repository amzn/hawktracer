#ifndef HAWKTRACER_TIMELINE_H
#define HAWKTRACER_TIMELINE_H

#include <hawktracer/events.h>
#include <hawktracer/monotonic_clock.h>
#include <hawktracer/timeline_listener.h>
#include <hawktracer/event_id_provider.h>
#include <hawktracer/feature.h>

#include <stddef.h>

/**
 * Returns a pointer to a timeline's feature.
 */
#define HT_TIMELINE_FEATURE(timeline, feature_id, feature_type) \
    ((feature_type*)ht_timeline_get_feature(timeline, feature_id))

/** Defines maximum number of features that can be attached to a timeline. */
#define HT_TIMELINE_MAX_FEATURES 32

HT_DECLS_BEGIN

typedef struct _HT_Timeline HT_Timeline;

HT_API HT_Timeline* ht_timeline_create(size_t buffer_capacity,
                                       HT_Boolean thread_safe,
                                       HT_Boolean serialize_events,
                                       const char* listeners,
                                       HT_ErrorCode* out_err);

/**
 * Destroys a timeline
 *
 * @param timeline a pointer to the timeline to destroy.
 */
HT_API void ht_timeline_destroy(HT_Timeline* timeline);

/**
 * Registers new listener to a timeline.
 *
 * @param timeline the timeline.
 * @param callback a callback of the listener.
 * @param user_data additional data that will be passed to the listener's callback.
 *
 * @return #HT_ERR_OK if registration completed successfully; otherwise, error code.
 */
HT_API HT_ErrorCode ht_timeline_register_listener(
        HT_Timeline* timeline,
        HT_TimelineListenerCallback callback,
        void* user_data);

/**
 * Removes all the listeners from a timeline
 *
 * @param timeline the timeline.
 */
HT_API void ht_timeline_unregister_all_listeners(HT_Timeline* timeline);

/**
 * Initializes event according to timeline's parameters.
 *
 * The method sets a timestamp and event_id for the event.
 *
 * @param timeline the timeline.
 * @param event the event to initialize.
 */
HT_API void ht_timeline_init_event(HT_Timeline* timeline, HT_Event* event);

/**
 * Pushes an event to a timeline.
 *
 * @param timeline the timeline.
 * @param event the event.
 */
HT_API void ht_timeline_push_event(HT_Timeline* timeline, HT_Event* event);

/**
 * Transfers all the events from internal buffer to listeners.
 *
 * @param timeline the timeline.
 */
HT_API void ht_timeline_flush(HT_Timeline* timeline);

/**
 * Enables a specific feature in the timeline.
 *
 * The timeline takes ownership of the feature, it means it will automatically destroys
 * it during timeline's destruction.
 * The timeline can have multiple features enabled, but it's not possible to
 * enable the same feature twice (second attempt results with #HT_ERR_FEATURE_ALREADY_REGISTERED
 * error code).
 * Before enabling the feature for the timeline, the feature must be registered. If it's not,
 * the function returns #HT_ERR_FEATURE_NOT_REGISTERED error code.
 *
 * @param timeline the timeline.
 * @param feature the feature.
 * @return #HT_ERR_OK if enabling the feature completed successfully; otherwise, error code.
 */
HT_API HT_ErrorCode ht_timeline_set_feature(HT_Timeline* timeline, HT_Feature* feature);

/**
 * Gets the feature object of a specific class from the pipeline.
 * @param timeline the timeline.
 * @param feature_klass the feature class.
 * @return a feature object of the specific class, if the timeline has the feature of this class enabled; otherwise, NULL.
 */
HT_API HT_Feature* ht_timeline_get_feature(HT_Timeline* timeline, HT_FeatureKlass* feature_klass);

HT_API HT_EventIdProvider* ht_timeline_get_id_provider(HT_Timeline* timeline);

/**
 * Pushes an event to the timeline.
 *
 * The macro automatically constructs the event, so user only needs to specify values
 * for event fields (excluding values for HT_Event; i.e. timestamp, event_id and klass_id).
 *
 * @param TIMELINE the timeline.
 * @param EVENT_TYPE a type of the event to push.
 * @param ... a list of parameters of the event.
 */
#define HT_TIMELINE_PUSH_EVENT(TIMELINE, EVENT_TYPE, ...) \
    HT_TIMELINE_PUSH_EVENT_PEDANTIC(TIMELINE, EVENT_TYPE, ht_base_event, __VA_ARGS__)

/**
 * Pushes an event to the timeline.
 *
 * The difference between this macro and HT_TIMELINE_PUSH_EVENT() is that
 * it requires using curly brackets for sub-events. It provides a special variable
 * @a ht_base_event that represents the #HT_Event base instance. For example, to push
 * @a SubEvent to a timeline, you should do:
 * @code
 * struct SubEvent {
 *   HT_Event base;
 *   int field1;
 *   float field2;
 * };
 *
 * HT_TIMELINE_PUSH_EVENT_PEDANTIC(timeline, SubEvent, {{ht_base_event}, 3, 9.3f});
 * @endcode
 *
 * The macro allows you to push events without generating compiler warnings
 * (unlike HT_TIMELINE_PUSH_EVENT()).
 *
 * @param TIMELINE the timeline.
 * @param EVENT_TYPE a type of the event to push.
 * @param ... a list of parameters of the event.
 */
#define HT_TIMELINE_PUSH_EVENT_PEDANTIC(TIMELINE, EVENT_TYPE, ...) \
    do { \
        HT_Event ht_base_event = { \
            ht_##EVENT_TYPE##_get_event_klass_instance(), \
            ht_monotonic_clock_get_timestamp(), \
            ht_event_id_provider_next(ht_timeline_get_id_provider(TIMELINE)) \
        }; \
        EVENT_TYPE ev = {__VA_ARGS__}; \
        ht_timeline_push_event(TIMELINE, HT_EVENT(&ev)); \
    } while (0)

HT_DECLS_END

#endif /* HAWKTRACER_TIMELINE_H */
