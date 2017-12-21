#ifndef HT_SIMPLE_INIT_H
#define HT_SIMPLE_INIT_H

#include <hawktracer/init.h>
#include <hawktracer/global_timeline.h>

#define HT_INIT_GLOBAL_LISTENERS_P(LISTENER_MACRO, ...) LISTENER_MACRO(ht_global_timeline_get(), __VA_ARGS__)
#define HT_INIT_GLOBAL_LISTENERS_(x) HT_INIT_GLOBAL_LISTENERS_P(x)
#define HT_INIT_GLOBAL_LISTENERS(X, USER_DATA) HT_INIT_GLOBAL_LISTENERS_(MKCREFLECT_EXPAND_VA_ X)

#define HT_DEINIT_GLOBAL_LISTENERS_P(LISTENER_MACRO, ...) LISTENER_MACRO(__VA_ARGS__)
#define HT_DEINIT_GLOBAL_LISTENERS_(x) HT_DEINIT_GLOBAL_LISTENERS_P(x)
#define HT_DEINIT_GLOBAL_LISTENERS(X, USER_DATA) HT_DEINIT_GLOBAL_LISTENERS_(MKCREFLECT_EXPAND_VA_ X)

#define HT_SIMPLE_INIT(argc, argv, ...) \
    ht_init(argc, argv); \
    MKCREFLECT_FOREACH(HT_INIT_GLOBAL_LISTENERS, 0, __VA_ARGS__) \
    while (0)

#define HT_SIMPLE_DEINIT(...) \
    do { \
        ht_timeline_flush(HT_TIMELINE(ht_global_timeline_get())); \
        ht_timeline_unregister_all_listeners(HT_TIMELINE(ht_global_timeline_get())); \
        MKCREFLECT_FOREACH(HT_DEINIT_GLOBAL_LISTENERS, 0, __VA_ARGS__) \
        ht_deinit(); \
    } while (0)

#endif /* HT_SIMPLE_INIT_H */
