#ifndef HAWKTRACER_INTERNAL_FEATURE_H
#define HAWKTRACER_INTERNAL_FEATURE_H

#include <hawktracer/timeline.h>

HT_DECLS_BEGIN

void ht_feature_disable(HT_Timeline* timeline, uint32_t id);

void ht_feature_register_core_features(void);

HT_DECLS_END

#endif /* HAWKTRACER_INTERNAL_FEATURE_H */
