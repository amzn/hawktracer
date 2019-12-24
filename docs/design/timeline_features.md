# Timeline features {#design_timeline_features}
HawkTracer doesn't allow creating subclasses of the HT_Timeline class, but the timeline object might be extended by *Timeline Features*.

Timeline feature is an additional context that can be attached to the timeline to store extra state of the object. For example, if the user needs to keep track of the total number of events pushed to a timeline, she can implement a timeline feature that stores integer and increment the value every time a new event is being pushed to the timeline.

Each timeline can have multiple features enabled, however, each feature can be enabled only once for the timeline. For example, if there are 2 features X and Y, and two timelines A and B, timeline A can have both X and Y features enabled, and timeline B can have feature X and Y enabled, but timeline A can't have feature X enabled twice.

Because of performance reasons, the features must be registered before they're enabled for timelines.

## Example
Before defining new features, it's highly recommended to go through the @ref tutorial_define_timeline_feature

## Thread safety
Registering a new feature in the HawkTracer system is thread safe.

Enabling a feature to the timeline is thread safe as long as user doesn't try to enable feature of the same type from different threads. In that case, the operation is not thread safe and might result with memory leak.

## Existing features
HawkTracer already defines two features that can be used with timelines. Both features are automatically registered in HawkTracer (in ht_init()), and they're also automatically attached to the global timeline.

* [callstack feature](@ref feature_callstack.h) - the feature keeps track of the duration of the specific scope in the code
* [cached string feature](@ref feature_cached_string.h) - the feature manages cache for strings, so the listeners receive string only once, and later only hashes of the strings are used (to save memory and bandwidth)
