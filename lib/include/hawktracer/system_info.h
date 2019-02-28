#ifndef HAWKTRACER_SYSTEM_INFO_H
#define HAWKTRACER_SYSTEM_INFO_H

#include <hawktracer/core_events.h>

HT_DECLS_BEGIN
/**
 * Gets the endianness of the system.
 *
 * @return endianness of the system.
 */
HT_API HT_Endianness ht_system_info_get_endianness(void);

HT_DECLS_END

#endif /* HAWKTRACER_SYSTEM_INFO_H */
