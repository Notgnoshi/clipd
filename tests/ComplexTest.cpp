#include "math/complex.h"

#include <gtest/gtest.h>

/**
 * @brief Tests the implementation of Complex equality.
 */
TEST( ComplexTestSuite, TestComplexEquality )
{
    const auto c1 = Math::Complex( 1, 1 );
    const auto c2 = Math::Complex( 1, 1 );
    const auto c3 = Math::Complex( 2, 2 );

    EXPECT_EQ( c1, c2 );
    EXPECT_NE( c1, c3 );
}

/**
 * @brief Tests the implementation of scalar multiplication.
 */
TEST( ComplexTestSuite, TestScalarMultiplication )
{
    ASSERT_TRUE( false ) << "Not yet implemented.";
}

/**
 * @brief Tests the implementation of complex addition.
 */
TEST( ComplexTestSuite, TestComplexAddition )
{
    ASSERT_TRUE( false ) << "Not yet implemented.";
}

/**
 * @brief Tests the implementation of complex multiplication.
 */
TEST( ComplexTestSuite, TestComplexMultiplication )
{
    ASSERT_TRUE( false ) << "Not yet implemented.";
}
