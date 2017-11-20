#include <gtest/gtest.h>

#include <hawktracer/event_id_provider.h>

TEST(EventIdProviderTest, BaseTest)
{
    // Arrange
    HT_EventIdProvider* provider = ht_event_id_provider_create();

    // Act & Assert
    ASSERT_EQ(0u, ht_event_id_provider_next(provider));
    ASSERT_EQ(1u, ht_event_id_provider_next(provider));
    ASSERT_EQ(2u, ht_event_id_provider_next(provider));

    ht_event_id_provider_destroy(provider);
}

TEST(EventIdProviderTest, DefaultProvider)
{
    // Arrange
    HT_EventIdProvider* provider = ht_event_id_provider_get_default();

    // Act & Assert
    auto first = ht_event_id_provider_next(provider);
    ASSERT_EQ(first + 1, ht_event_id_provider_next(provider));
    ASSERT_EQ(first + 2, ht_event_id_provider_next(provider));
    ASSERT_EQ(first + 3, ht_event_id_provider_next(provider));
}
