#ifndef CPP_INT_HPP
#define CPP_INT_HPP

#include <cstdint>
#include <cstddef>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

class cpp_int
{
public:
    cpp_int();
    cpp_int(bool value) : limbs(1, value ? 1u : 0u), negative(false)
    {
    }
    cpp_int(std::nullptr_t) = delete;
    cpp_int(const char *value);
    cpp_int(const std::string &value);

    template <class Integer, typename std::enable_if<std::is_integral<Integer>::value, int>::type = 0>
    cpp_int(Integer value) : limbs(1, 0), negative(false)
    {
        typedef typename std::make_unsigned<Integer>::type Unsigned;

        Unsigned magnitude;
        if (std::is_signed<Integer>::value && value < 0)
        {
            negative = true;
            magnitude = static_cast<Unsigned>(-(value + 1)) + 1;
        }
        else
        {
            magnitude = static_cast<Unsigned>(value);
        }

        limbs.clear();
        if (magnitude == 0)
        {
            limbs.push_back(0);
        }
        else
        {
            while (magnitude > 0)
            {
                limbs.push_back(static_cast<std::uint32_t>(magnitude % BASE));
                magnitude /= BASE;
            }
        }
    }

    bool is_zero() const;
    int sign() const;
    std::string str() const;

    template <class T>
    T convert_to() const
    {
        T result = 0;
        for (std::size_t i = limbs.size(); i > 0; --i)
        {
            result = static_cast<T>(result * static_cast<T>(BASE) + static_cast<T>(limbs[i - 1]));
        }
        return negative ? static_cast<T>(-result) : result;
    }

    cpp_int &operator=(const char *value);
    cpp_int &operator=(std::nullptr_t) = delete;
    cpp_int &operator=(const std::string &value);

    template <class Integer, typename std::enable_if<std::is_integral<Integer>::value, int>::type = 0>
    cpp_int &operator=(Integer value)
    {
        *this = cpp_int(value);
        return *this;
    }

    cpp_int operator+() const;
    cpp_int operator-() const;

    cpp_int &operator+=(const cpp_int &rhs);
    cpp_int &operator-=(const cpp_int &rhs);
    cpp_int &operator*=(const cpp_int &rhs);
    cpp_int &operator/=(const cpp_int &rhs);
    cpp_int &operator%=(const cpp_int &rhs);
    cpp_int &operator<<=(unsigned int shift);
    cpp_int &operator>>=(unsigned int shift);

    cpp_int &operator++();
    cpp_int operator++(int);
    cpp_int &operator--();
    cpp_int operator--(int);

    friend cpp_int operator+(const cpp_int &lhs, const cpp_int &rhs);
    friend cpp_int operator-(const cpp_int &lhs, const cpp_int &rhs);
    friend cpp_int operator*(const cpp_int &lhs, const cpp_int &rhs);
    friend cpp_int operator/(const cpp_int &lhs, const cpp_int &rhs);
    friend cpp_int operator%(const cpp_int &lhs, const cpp_int &rhs);
    friend cpp_int operator<<(cpp_int lhs, unsigned int shift);
    friend cpp_int operator>>(cpp_int lhs, unsigned int shift);

    friend bool operator==(const cpp_int &lhs, const cpp_int &rhs);
    friend bool operator!=(const cpp_int &lhs, const cpp_int &rhs);
    friend bool operator<(const cpp_int &lhs, const cpp_int &rhs);
    friend bool operator>(const cpp_int &lhs, const cpp_int &rhs);
    friend bool operator<=(const cpp_int &lhs, const cpp_int &rhs);
    friend bool operator>=(const cpp_int &lhs, const cpp_int &rhs);

    friend std::istream &operator>>(std::istream &in, cpp_int &value);
    friend std::ostream &operator<<(std::ostream &out, const cpp_int &value);

private:
    static const std::uint32_t BASE = 1000000000u;
    static const int BASE_DIGITS = 9;

    std::vector<std::uint32_t> limbs;
    bool negative;

    void normalize();
    void read_decimal(const std::string &value);
    void shift_base_add(std::uint32_t block);

    static int compare_abs(const cpp_int &lhs, const cpp_int &rhs);
    static cpp_int add_abs(const cpp_int &lhs, const cpp_int &rhs);
    static cpp_int sub_abs(const cpp_int &lhs, const cpp_int &rhs);
    static cpp_int mul_abs_small(const cpp_int &value, std::uint32_t multiplier);
    static cpp_int div_abs_small(const cpp_int &value, std::uint32_t divisor, std::uint32_t *remainder = 0);
    static void div_mod_abs(const cpp_int &lhs, const cpp_int &rhs, cpp_int &quotient, cpp_int &remainder);
};

cpp_int abs(const cpp_int &value);

#endif
