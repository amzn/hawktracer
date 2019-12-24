#ifndef HAWKTRACER_INTERNAL_FEATURE_H
#define HAWKTRACER_INTERNAL_FEATURE_H

#include <hawktracer/macros.h>

HT_DECLS_BEGIN

void ht_feature_register_core_features(void);

HT_ErrorCode HT_FeatureCachedString_register(void);
HT_ErrorCode HT_FeatureCallstack_register(void);

HT_DECLS_END

#endif /* HAWKTRACER_INTERNAL_FEATURE_H */
