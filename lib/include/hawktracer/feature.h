#ifndef HAWKTRACER_FEATURE_H
#define HAWKTRACER_FEATURE_H

#include <hawktracer/macros.h>
#include <hawktracer/base_types.h>
#include <hawktracer/alloc.h>

HT_DECLS_BEGIN

/**
 * @see _HT_Feature
 */
typedef struct _HT_Feature HT_Feature;

/**
 * A type of the destroy feature callback.
 *
 * @param feature a feature to destory.
 */
typedef void (*HT_FeatureDestroy)(HT_Feature* feature);

/**
 * The struct represents a feature class.
 *
 * There should only be one global instance of this struct. It
 * contains a knowledge about destroying the object.
 */
typedef struct
{
    /**
     * Destroy calback. It's used for automatic feature destruction.
     * It should release all the resources allocated for the feature
     * and the memory used for the object itself.
     */
    HT_FeatureDestroy destroy;

    /* <private readonly> */
    /**
     * Stores identifier of the feature class.
     * The identifier is used in the registry to identify the feature class.
     * It should not be modified and accessed by user, the field is
     * used for internal purposes only.
     */
    uint32_t id;
} HT_FeatureKlass;

/**
 * The struct is a base class for all the user-defined features.
 *
 * Each user-defined feature must have a member of this type as
 * a first field.
 */
struct _HT_Feature
{
    /** A pointer to a class of the feature. For internal purposes only. */
    HT_FeatureKlass* klass;
};

/**
 * Defines a set of methods that help defining timeline feature.
 *
 * @param TYPE_NAME a name of the struct that represents the new feature.
 * @param DESTROY_FNC a functor for destroying the feature object (of type #HT_FeatureDestroy).
 *
 * The struct defines following functions
 *  - TYPE_NAME_get_class() - returns class of the feature
 *  - TYPE_NAME_alloc() - allocates memory (using ht_alloc()) and initializes the feature object
 *  - TYPE_NAME_from_timeline() - gets the feature object from the timeline. Returns NULL if the feature is not enabled for the timeline.
 *  - TYPE_NAME_register() - registers a feature in HawkTracer.
 */
#define HT_FEATURE_DEFINE(TYPE_NAME, DESTROY_FNC) \
    static HT_FeatureKlass* TYPE_NAME ## _get_class(void) { \
        static HT_FeatureKlass feature_klass = { DESTROY_FNC, HT_INVALID_FEATURE_ID }; \
        return &feature_klass; \
    } \
    static TYPE_NAME* TYPE_NAME ## _alloc(void) { \
        TYPE_NAME* feature = HT_CREATE_TYPE(TYPE_NAME); \
        if (feature) { \
            ((HT_Feature*)feature)->klass = TYPE_NAME ## _get_class(); \
        } \
        return feature; \
    } \
    static HT_INLINE TYPE_NAME* TYPE_NAME ## _from_timeline(HT_Timeline* timeline) { \
        return ((TYPE_NAME*)ht_timeline_get_feature(timeline, TYPE_NAME ## _get_class())); \
    } \
    HT_ErrorCode TYPE_NAME ## _register(void) { \
        return ht_registry_register_feature(TYPE_NAME ## _get_class()); \
    }

HT_DECLS_END

#endif /* HAWKTRACER_FEATURE_H */
