#ifndef HAWKTRACER_HASH_H
#define HAWKTRACER_HASH_H

#include <hawktracer/macros.h>

#include <inttypes.h>

HT_DECLS_BEGIN

static uint32_t djb2_hash(const char *str)
{
    uint32_t hash = 5381;
    int c;

    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

HT_DECLS_END

#endif /* HAWKTRACER_HASH_H */
