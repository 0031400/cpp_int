#include "big_int.hpp"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>

const std::uint32_t cpp_int::BASE;
const int cpp_int::BASE_DIGITS;

cpp_int::cpp_int() : limbs(1, 0), negative(false)
{
}

cpp_int::cpp_int(const char *value) : cpp_int()
{
    if (value == 0)
    {
        throw std::invalid_argument("cpp_int cannot be constructed from a null string");
    }
    read_decimal(value);
}

cpp_int::cpp_int(const std::string &value) : cpp_int()
{
    read_decimal(value);
}

bool cpp_int::is_zero() const
{
    return limbs.size() == 1 && limbs[0] == 0;
}

int cpp_int::sign() const
{
    if (is_zero())
    {
        return 0;
    }
    return negative ? -1 : 1;
}

std::string cpp_int::str() const
{
    std::ostringstream out;
    out << *this;
    return out.str();
}

cpp_int &cpp_int::operator=(const char *value)
{
    if (value == 0)
    {
        throw std::invalid_argument("cpp_int cannot be assigned from a null string");
    }
    return *this = std::string(value);
}

cpp_int &cpp_int::operator=(const std::string &value)
{
    read_decimal(value);
    return *this;
}

cpp_int cpp_int::operator+() const
{
    return *this;
}

cpp_int cpp_int::operator-() const
{
    cpp_int result = *this;
    if (!result.is_zero())
    {
        result.negative = !result.negative;
    }
    return result;
}

cpp_int &cpp_int::operator+=(const cpp_int &rhs)
{
    *this = *this + rhs;
    return *this;
}

cpp_int &cpp_int::operator-=(const cpp_int &rhs)
{
    *this = *this - rhs;
    return *this;
}

cpp_int &cpp_int::operator*=(const cpp_int &rhs)
{
    *this = *this * rhs;
    return *this;
}

cpp_int &cpp_int::operator/=(const cpp_int &rhs)
{
    *this = *this / rhs;
    return *this;
}

cpp_int &cpp_int::operator%=(const cpp_int &rhs)
{
    *this = *this % rhs;
    return *this;
}

cpp_int &cpp_int::operator<<=(unsigned int shift)
{
    bool was_negative = negative;
    while (shift > 0)
    {
        unsigned int step = std::min(shift, 29u);
        *this = mul_abs_small(*this, static_cast<std::uint32_t>(1u << step));
        shift -= step;
    }
    negative = was_negative && !is_zero();
    return *this;
}

cpp_int &cpp_int::operator>>=(unsigned int shift)
{
    bool was_negative = negative;
    while (shift > 0)
    {
        unsigned int step = std::min(shift, 29u);
        *this = div_abs_small(*this, static_cast<std::uint32_t>(1u << step));
        shift -= step;
    }
    negative = was_negative && !is_zero();
    return *this;
}

cpp_int &cpp_int::operator++()
{
    *this += 1;
    return *this;
}

cpp_int cpp_int::operator++(int)
{
    cpp_int old = *this;
    ++(*this);
    return old;
}

cpp_int &cpp_int::operator--()
{
    *this -= 1;
    return *this;
}

cpp_int cpp_int::operator--(int)
{
    cpp_int old = *this;
    --(*this);
    return old;
}

void cpp_int::normalize()
{
    while (limbs.size() > 1 && limbs.back() == 0)
    {
        limbs.pop_back();
    }

    if (is_zero())
    {
        negative = false;
    }
}

void cpp_int::read_decimal(const std::string &value)
{
    limbs.assign(1, 0);
    negative = false;

    std::size_t pos = 0;
    while (pos < value.size() && std::isspace(static_cast<unsigned char>(value[pos])))
    {
        ++pos;
    }

    bool is_negative = false;
    if (pos < value.size() && (value[pos] == '+' || value[pos] == '-'))
    {
        is_negative = value[pos] == '-';
        ++pos;
    }

    while (pos < value.size() && value[pos] == '0')
    {
        ++pos;
    }

    if (pos == value.size())
    {
        return;
    }

    limbs.clear();

    for (std::size_t end = value.size(); end > pos;)
    {
        std::size_t begin = end >= pos + BASE_DIGITS ? end - BASE_DIGITS : pos;
        std::uint32_t block = 0;

        for (std::size_t i = begin; i < end; ++i)
        {
            if (!std::isdigit(static_cast<unsigned char>(value[i])))
            {
                throw std::invalid_argument("invalid cpp_int decimal string");
            }
            block = block * 10 + static_cast<std::uint32_t>(value[i] - '0');
        }

        limbs.push_back(block);
        end = begin;
    }

    negative = is_negative;
    normalize();
}

void cpp_int::shift_base_add(std::uint32_t block)
{
    if (is_zero())
    {
        limbs[0] = block;
    }
    else
    {
        limbs.insert(limbs.begin(), block);
    }
    normalize();
}

int cpp_int::compare_abs(const cpp_int &lhs, const cpp_int &rhs)
{
    if (lhs.limbs.size() != rhs.limbs.size())
    {
        return lhs.limbs.size() < rhs.limbs.size() ? -1 : 1;
    }

    for (std::size_t i = lhs.limbs.size(); i > 0; --i)
    {
        if (lhs.limbs[i - 1] != rhs.limbs[i - 1])
        {
            return lhs.limbs[i - 1] < rhs.limbs[i - 1] ? -1 : 1;
        }
    }

    return 0;
}

cpp_int cpp_int::add_abs(const cpp_int &lhs, const cpp_int &rhs)
{
    cpp_int result;
    result.limbs.clear();

    std::uint64_t carry = 0;
    const std::size_t count = std::max(lhs.limbs.size(), rhs.limbs.size());

    for (std::size_t i = 0; i < count || carry; ++i)
    {
        std::uint64_t sum = carry;
        if (i < lhs.limbs.size())
        {
            sum += lhs.limbs[i];
        }
        if (i < rhs.limbs.size())
        {
            sum += rhs.limbs[i];
        }

        result.limbs.push_back(static_cast<std::uint32_t>(sum % BASE));
        carry = sum / BASE;
    }

    result.normalize();
    return result;
}

cpp_int cpp_int::sub_abs(const cpp_int &lhs, const cpp_int &rhs)
{
    cpp_int result;
    result.limbs.clear();

    std::int64_t borrow = 0;

    for (std::size_t i = 0; i < lhs.limbs.size(); ++i)
    {
        std::int64_t cur = static_cast<std::int64_t>(lhs.limbs[i]) - borrow;
        if (i < rhs.limbs.size())
        {
            cur -= rhs.limbs[i];
        }

        if (cur < 0)
        {
            cur += BASE;
            borrow = 1;
        }
        else
        {
            borrow = 0;
        }

        result.limbs.push_back(static_cast<std::uint32_t>(cur));
    }

    result.normalize();
    return result;
}

cpp_int cpp_int::mul_abs_small(const cpp_int &value, std::uint32_t multiplier)
{
    if (multiplier == 0 || value.is_zero())
    {
        return cpp_int(0);
    }

    cpp_int result;
    result.limbs.clear();
    result.negative = false;

    std::uint64_t carry = 0;
    for (std::size_t i = 0; i < value.limbs.size() || carry; ++i)
    {
        std::uint64_t cur = carry;
        if (i < value.limbs.size())
        {
            cur += static_cast<std::uint64_t>(value.limbs[i]) * multiplier;
        }

        result.limbs.push_back(static_cast<std::uint32_t>(cur % BASE));
        carry = cur / BASE;
    }

    result.normalize();
    return result;
}

cpp_int cpp_int::div_abs_small(const cpp_int &value, std::uint32_t divisor, std::uint32_t *remainder)
{
    if (divisor == 0)
    {
        throw std::runtime_error("division by zero");
    }

    cpp_int result;
    result.limbs.assign(value.limbs.size(), 0);
    result.negative = false;

    std::uint64_t rem = 0;

    for (std::size_t i = value.limbs.size(); i > 0; --i)
    {
        std::uint64_t cur = value.limbs[i - 1] + rem * BASE;
        result.limbs[i - 1] = static_cast<std::uint32_t>(cur / divisor);
        rem = cur % divisor;
    }

    if (remainder)
    {
        *remainder = static_cast<std::uint32_t>(rem);
    }

    result.normalize();
    return result;
}

void cpp_int::div_mod_abs(const cpp_int &lhs, const cpp_int &rhs, cpp_int &quotient, cpp_int &remainder)
{
    if (rhs.is_zero())
    {
        throw std::runtime_error("division by zero");
    }

    if (compare_abs(lhs, rhs) < 0)
    {
        quotient = 0;
        remainder = lhs;
        remainder.negative = false;
        return;
    }

    std::uint32_t norm = BASE / (rhs.limbs.back() + 1u);
    cpp_int a = mul_abs_small(lhs, norm);
    cpp_int b = mul_abs_small(rhs, norm);

    quotient.limbs.assign(a.limbs.size(), 0);
    quotient.negative = false;
    remainder = 0;

    for (std::size_t i = a.limbs.size(); i > 0; --i)
    {
        remainder.shift_base_add(a.limbs[i - 1]);

        const std::size_t b_size = b.limbs.size();
        const std::uint64_t high = remainder.limbs.size() > b_size ? remainder.limbs[b_size] : 0;
        const std::uint64_t next = remainder.limbs.size() > b_size - 1 ? remainder.limbs[b_size - 1] : 0;

        std::uint64_t guess = (high * BASE + next) / b.limbs.back();
        if (guess >= BASE)
        {
            guess = BASE - 1;
        }

        cpp_int product = mul_abs_small(b, static_cast<std::uint32_t>(guess));
        while (compare_abs(product, remainder) > 0)
        {
            --guess;
            product = sub_abs(product, b);
        }

        remainder = sub_abs(remainder, product);
        quotient.limbs[i - 1] = static_cast<std::uint32_t>(guess);
    }

    quotient.normalize();
    remainder = div_abs_small(remainder, norm);
    remainder.normalize();
}

cpp_int operator+(const cpp_int &lhs, const cpp_int &rhs)
{
    cpp_int result;

    if (lhs.negative == rhs.negative)
    {
        result = cpp_int::add_abs(lhs, rhs);
        result.negative = lhs.negative;
    }
    else
    {
        int cmp = cpp_int::compare_abs(lhs, rhs);
        if (cmp >= 0)
        {
            result = cpp_int::sub_abs(lhs, rhs);
            result.negative = lhs.negative;
        }
        else
        {
            result = cpp_int::sub_abs(rhs, lhs);
            result.negative = rhs.negative;
        }
    }

    result.normalize();
    return result;
}

cpp_int operator-(const cpp_int &lhs, const cpp_int &rhs)
{
    return lhs + (-rhs);
}

cpp_int operator*(const cpp_int &lhs, const cpp_int &rhs)
{
    if (lhs.is_zero() || rhs.is_zero())
    {
        return cpp_int(0);
    }

    cpp_int result;
    result.limbs.assign(lhs.limbs.size() + rhs.limbs.size(), 0);
    result.negative = lhs.negative != rhs.negative;

    for (std::size_t i = 0; i < lhs.limbs.size(); ++i)
    {
        std::uint64_t carry = 0;

        for (std::size_t j = 0; j < rhs.limbs.size() || carry; ++j)
        {
            std::uint64_t cur = result.limbs[i + j] + carry;
            if (j < rhs.limbs.size())
            {
                cur += static_cast<std::uint64_t>(lhs.limbs[i]) * rhs.limbs[j];
            }

            result.limbs[i + j] = static_cast<std::uint32_t>(cur % cpp_int::BASE);
            carry = cur / cpp_int::BASE;
        }
    }

    result.normalize();
    return result;
}

cpp_int operator/(const cpp_int &lhs, const cpp_int &rhs)
{
    cpp_int abs_lhs = lhs;
    cpp_int abs_rhs = rhs;
    cpp_int quotient;
    cpp_int remainder;

    abs_lhs.negative = false;
    abs_rhs.negative = false;

    cpp_int::div_mod_abs(abs_lhs, abs_rhs, quotient, remainder);

    quotient.negative = lhs.negative != rhs.negative;
    quotient.normalize();
    return quotient;
}

cpp_int operator%(const cpp_int &lhs, const cpp_int &rhs)
{
    cpp_int abs_lhs = lhs;
    cpp_int abs_rhs = rhs;
    cpp_int quotient;
    cpp_int remainder;

    abs_lhs.negative = false;
    abs_rhs.negative = false;

    cpp_int::div_mod_abs(abs_lhs, abs_rhs, quotient, remainder);

    remainder.negative = lhs.negative;
    remainder.normalize();
    return remainder;
}

cpp_int operator<<(cpp_int lhs, unsigned int shift)
{
    lhs <<= shift;
    return lhs;
}

cpp_int operator>>(cpp_int lhs, unsigned int shift)
{
    lhs >>= shift;
    return lhs;
}

bool operator==(const cpp_int &lhs, const cpp_int &rhs)
{
    return lhs.negative == rhs.negative && lhs.limbs == rhs.limbs;
}

bool operator!=(const cpp_int &lhs, const cpp_int &rhs)
{
    return !(lhs == rhs);
}

bool operator<(const cpp_int &lhs, const cpp_int &rhs)
{
    if (lhs.negative != rhs.negative)
    {
        return lhs.negative;
    }

    int cmp = cpp_int::compare_abs(lhs, rhs);
    return lhs.negative ? cmp > 0 : cmp < 0;
}

bool operator>(const cpp_int &lhs, const cpp_int &rhs)
{
    return rhs < lhs;
}

bool operator<=(const cpp_int &lhs, const cpp_int &rhs)
{
    return !(rhs < lhs);
}

bool operator>=(const cpp_int &lhs, const cpp_int &rhs)
{
    return !(lhs < rhs);
}

std::istream &operator>>(std::istream &in, cpp_int &value)
{
    std::string text;
    in >> text;
    value = text;
    return in;
}

std::ostream &operator<<(std::ostream &out, const cpp_int &value)
{
    if (value.negative && !value.is_zero())
    {
        out << '-';
    }

    out << value.limbs.back();

    for (std::size_t i = value.limbs.size() - 1; i > 0; --i)
    {
        out << std::setw(cpp_int::BASE_DIGITS) << std::setfill('0') << value.limbs[i - 1];
    }

    return out;
}

cpp_int abs(const cpp_int &value)
{
    return value < 0 ? -value : value;
}
