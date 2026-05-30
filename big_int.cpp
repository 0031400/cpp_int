#include "big_int.hpp"
#include <algorithm>
#include <iostream>
#include <stdexcept>
BigInt::BigInt() {}
void BigInt::pop_zero()
{
    while (digits.size() > 0 && digits.back() == 0)
    {
        digits.pop_back();
    }
}
BigInt::BigInt(int x)
{
    if (x < 0)
        is_positive = false, x = -x;
    do
    {
        // make sure there is a zero
        digits.push_back(x % 10);
        x = x / 10;
    } while (x != 0);
}
BigInt::BigInt(std::string x)
{
    if (!x.size())
    {
        throw std::invalid_argument("string to BigInt: x is \"\"");
    }
    if (x == "-0" || x == "0")
    {
        digits = {0};
        return;
    }
    if (x[0] == '-')
    {
        is_positive = false;
        x.erase(0, 1);
    }
    int n = x.size();
    digits.resize(n);
    for (int i = n - 1; i >= 0; i--)
    {
        if (x[i] >= '0' && x[i] <= '9')
        {
            digits[n - 1 - i] = (x[i] - '0');
        }
        else
        {
            throw std::invalid_argument("string to BigInt: " + x);
        }
    }
}
BigInt operator+(const BigInt &a, const BigInt &b)
{
    if (a.is_positive && !b.is_positive)
    {
        return a - (-b);
    }
    if (!a.is_positive && b.is_positive)
    {
        return -a - b;
    }
    BigInt res;
    if (!a.is_positive && !b.is_positive)
    {
        res.is_positive = false;
    }
    // two positive
    int n = std::max(a.digits.size(), b.digits.size());
    res.digits.resize(n);
    int t = 0;
    for (int i = 0; i < n; i++)
    {
        if (i < a.digits.size())
        {
            t += a.digits[i];
        }
        if (i < b.digits.size())
        {
            t += b.digits[i];
        }
        if (t >= 10)
        {
            res.digits[i] = t - 10;
            t = 1;
        }
        else
        {
            res.digits[i] = t;
            t = 0;
        }
    }
    if (t)
    {
        res.digits.push_back(t);
    }
    return res;
}
BigInt operator-(const BigInt &a, const BigInt &b)
{
    if (!a.is_positive && !b.is_positive)
    {
        return -b - (-a);
    }
    if (a.is_positive && !b.is_positive)
    {
        return a + (-b);
    }
    if (!a.is_positive && b.is_positive)
    {
        return -(-a + b);
    }
    if (a < b)
    {
        return -(b - a);
    }
    BigInt res;
    // a > b > 0
    int n = std::max(a.digits.size(), b.digits.size());
    res.digits.resize(n);
    int t = 0;
    for (int i = 0; i < n; i++)
    {
        if (i < a.digits.size())
        {
            t += a.digits[i];
        }
        if (i < b.digits.size())
        {
            t -= b.digits[i];
        }
        if (t < 0)
        {
            res.digits[i] = 10 + t;
            t = -1;
        }
        else
        {
            res.digits[i] = t;
            t = 0;
        }
    }
    res.pop_zero();
    return res;
}
BigInt BigInt::operator-() const
{
    BigInt res = *this;
    res.is_positive = !res.is_positive;
    return res;
}
BigInt operator*(const BigInt &a, const BigInt &b)
{
    BigInt res;
    for (int i = 0; i < b.digits.size(); i++)
    {
        BigInt t = a.mul_small(b.digits[i]);
        t = t.mul_ten(i);
        res = res + t;
    }
    res.is_positive = a.is_positive == b.is_positive;
    return res;
}
std::istream &operator>>(std::istream &is, BigInt &a)
{
    std::string x;
    is >> x;
    a = BigInt(x);
    return is;
}
std::ostream &operator<<(std::ostream &os, const BigInt &a)
{
    int n = a.digits.size();
    if (!n)
    {
        os << 0;
        return os;
    }
    if (!a.is_positive)
    {
        os << "-";
    }
    for (int i = n - 1; i >= 0; i--)
    {
        os << a.digits[i];
    }
    return os;
}
std::tuple<int, BigInt> BigInt::div_ten()
{
    BigInt res = *this;
    int i = 0;
    if (is_zero())
    {
        return {0, res};
    }
    while (!digits[i])
    {
        i++;
    }
    res.digits.erase(res.digits.begin(), res.digits.begin() + i);
    return {i, res};
}
bool BigInt::is_zero() const
{
    return digits.size() == 1 && digits[0] == 0;
}
BigInt BigInt::mul_small(int x) const
{
    BigInt res;
    int n = digits.size();
    res.digits.resize(n);
    int t = 0;
    for (int i = 0; i < n; i++)
    {
        t += digits[i] * x;
        res.digits[i] = t % 10;
        t /= 10;
    }
    if (t)
    {
        res.digits.push_back(t);
    }
    return res;
}
BigInt BigInt::mul_ten(int x) const
{
    BigInt res = *this;
    if (is_zero())
        return res;
    res.digits.insert(res.digits.begin(), x, 0);
    return res;
}
bool operator==(const BigInt &x, const BigInt &y)
{
    if (x.is_zero() == true && y.is_zero() == true)
    {
        return true;
    }
    if (x.digits.size() != y.digits.size())
    {
        return false;
    }
    if (x.is_positive != y.is_positive)
    {
        return false;
    }
    for (int i = 0; i < x.digits.size(); i++)
    {
        if (x.digits[i] != y.digits[i])
        {
            return false;
        }
    }
    return true;
}
bool operator!=(const BigInt &x, const BigInt &y)
{
    return !(x == y);
}
bool operator>(const BigInt &x, const BigInt &y)
{
    if (x.is_zero())
    {
        return !(y.is_zero() || y.is_positive);
    }
    if (y.is_zero())
    {
        return !x.is_zero() || x.is_positive;
    }
    if (x.is_positive != y.is_positive)
    {
        return x.is_positive;
    }
    // x and y is ++ or --
    if (x.digits.size() > y.digits.size())
    {
        return x.is_positive;
    }
    if (x.digits.size() < y.digits.size())
    {
        return !x.is_positive;
    }
    for (int i = x.digits.size() - 1; i >= 0; i--)
    {
        if (x.digits[i] > y.digits[i])
        {
            return x.is_positive;
        }
        else if (x.digits[i] < y.digits[i])
        {
            return !x.is_positive;
        }
    }
    return false;
}
bool operator>=(const BigInt &x, const BigInt &y)
{
    return x > y || x == y;
}
bool operator<(const BigInt &x, const BigInt &y)
{
    return y > x;
}
bool operator<=(const BigInt &x, const BigInt &y)
{
    return y >= x;
}
BigInt operator%(const BigInt &x, const BigInt &y)
{
    BigInt res = x - x / y * y;
    res.is_positive = x.is_positive;
    return res;
}
BigInt operator/(const BigInt &x, const BigInt &y)
{
    BigInt res;
    res.is_positive = x.is_positive == y.is_positive;
    BigInt xx = x;
    BigInt yy = y;
    xx.is_positive = true;
    yy.is_positive = true;
    if (y.is_zero())
    {
        throw std::invalid_argument("divisor is zero");
    }
    if (xx < yy)
    {
        return BigInt(0);
    }
    int lx = xx.digits.size();
    int ly = yy.digits.size();
    res.digits.resize(lx - ly + 1);
    for (int i = lx - ly; i >= 0; i--)
    {
        BigInt t;
        if (i < xx.digits.size())
        {
            t = xx.cut(i, xx.digits.size());
        }
        else
        {
            t = BigInt(0);
        }
        int m = 0;
        while (yy.mul_small(m + 1) <= t)
        {
            m++;
        }
        res.digits[i] = m;
        if (m != 0)
        {

            xx = xx - yy.mul_small(m).mul_ten(i);
        }
    }
    res.pop_zero();
    if (res.digits.empty())
    {
        return BigInt(0);
    }
    return res;
}
BigInt BigInt::cut(int start, int end) const
{
    BigInt res;
    res.is_positive = is_positive;
    res.digits = std::vector<int>(digits.begin() + start, digits.begin() + end);
    return res;
}