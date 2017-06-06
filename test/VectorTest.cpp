#include <hawktracer/Vector.h>

#include <gtest/gtest.h>

using hawktracer::Vector;

TEST(VectorTest, FindNonExistingValue_ShouldFail)
{
    // Arrange
    Vector<int> vec;
    vec.emplace_back(1);
    vec.emplace_back(2);

    // Act & Assert
    ASSERT_EQ(vec.invalid_size, vec.find(3));
}

TEST(VectorTest, FindNonExistingValue_ShouldPass)
{
    // Arrange
    Vector<int> vec;
    vec.emplace_back(1);
    vec.emplace_back(2);

    // Act & Assert
    ASSERT_EQ(1u, vec.find(2));
}

TEST(VectorTest, ArrayOperatorShouldReturnValidObject)
{
    // Arrange
    Vector<int> vec;
    vec.emplace_back(1);
    vec.emplace_back(2);
    vec.emplace_back(3);

    // Act & Assert
    ASSERT_EQ(2, vec[1]);
}

TEST(VectorTest, EraseMethodShouldRemoveObject)
{
    // Arrange
    Vector<int> vec;
    vec.emplace_back(1);
    vec.emplace_back(2);
    vec.emplace_back(3);

    // Act
    vec.erase(1);

    // Assert
    ASSERT_EQ(vec.invalid_size, vec.find(2));
}

TEST(VectorTest, ShouldResizeIfNeeded)
{
    // Arrange
    Vector<int> vec(8);

    // Act
    for (int i = 0; i < 20; i++)
    {
        vec.emplace_back(i);
    }

    // Assert
    ASSERT_EQ(19, vec[19]);
}

TEST(VectorTest, ShouldNotFailOnDecreasingSize)
{
    // Arrange
    Vector<int> vec(8);

    for (int i = 0; i < 64; i++)
    {
        vec.emplace_back(i);
    }

    // Act & Assert
    for (int i = 63; i >= 0; i--)
    {
        vec.erase(i);
    }
}

TEST(VectorTest, ShouldNotFailOnDecreasingAndIncreasingTheSize)
{
    // Arrange
    Vector<int> vec(8);

    // Act & Assert
    for (int i = 0; i < 64; i++)
    {
        vec.emplace_back(i);
    }

    for (int i = 63; i >= 0; i--)
    {
        vec.erase(i);
    }

    for (int i = 0; i < 64; i++)
    {
        vec.emplace_back(i);
    }
}
