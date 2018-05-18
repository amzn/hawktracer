#ifndef HAWKTRACER_GLOBAL_TIMELINE_H
#define HAWKTRACER_GLOBAL_TIMELINE_H

#include <hawktracer/scoped_tracepoint.h>
#include <hawktracer/feature_cached_string.h>
#include <hawktracer/string_scoped_tracepoint.h>

HT_DECLS_BEGIN

HT_API HT_Timeline* ht_global_timeline_get(void);

HT_DECLS_END

#define HT_TP_GLOBAL_SCOPED_INT(int_label) HT_TP_SCOPED_INT(ht_global_timeline_get(), int_label)

#define HT_TP_GLOBAL_SCOPED_STRING(string_label) HT_TP_SCOPED_STRING(ht_global_timeline_get(), string_label)

#define HT_TP_G_STRACEPOINT(label) HT_TP_STRACEPOINT(ht_global_timeline_get(), label)

#define HT_TP_G_FUNCTION() HT_TP_FUNCTION(ht_global_timeline_get())

#endif /* HAWKTRACER_GLOBAL_TIMELINE_H */
