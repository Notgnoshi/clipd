#pragma once

#include "common.h"

#include <cmath>

namespace Math
{
/**
 * @brief A class to represent Complex numbers and mathematical operation s.
 */
class Complex
{
  public:
    /**
     * @brief Default constructor, creates the number (1, 0)
     */
    Complex();

    /**
     * @brief Constructs a new complex number from given rectangular coordinates.
     *
     * @param x The x coordinate.
     * @param y The y coordinate.
     */
    Complex( long double x, long double y );

    /**
     * @brief Compare two Complex numbers for equality.
     *
     * @param other The other complex number to compare against.
     * @returns true if the two complex numbers are equal.
     * @returns false if the two complex numbers are not equal.
     */
    bool operator==( const Complex& other ) const;
    /**
     * @brief Compare two Complex numbers for inequality.
     *
     * @param other The other complex number to compare against.
     * @returns true if the two complex numbers are not equal.
     * @returns false if the two complex numbers are equal
     */
    bool operator!=( const Complex& other ) const;

    /**
     * @brief Adds two Complex numbers together.
     *
     * @param other The other Complex number to add.
     * @return Complex& The result of the specified addition.
     */
    Complex& operator+( const Complex& other );

  private:
    // Rectangular coordinates
    long double x;
    long double y;

    // Polar coordinates
    long double r;
    long double arg;
};
} // namespace Math
