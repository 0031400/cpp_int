#include <vector>
#include <string>
#include <tuple>
class BigInt
{
public:
    // digits must have items
    std::vector<int> digits = {};
    bool is_positive = true;
    void pop_zero();

    BigInt();
    explicit BigInt(int);
    explicit BigInt(std::string);

    BigInt mul_small(int) const;
    BigInt mul_ten(int) const;
    std::tuple<int, BigInt> div_ten();
    bool is_zero() const;
    BigInt cut(int, int) const;

    BigInt operator-() const;
    friend BigInt operator+(const BigInt &, const BigInt &);
    friend BigInt operator-(const BigInt &, const BigInt &);
    friend BigInt operator*(const BigInt &, const BigInt &);
    friend BigInt operator/(const BigInt &, const BigInt &);
    friend BigInt operator%(const BigInt &, const BigInt &);
    friend std::istream &operator>>(std::istream &, BigInt &);
    friend std::ostream &operator<<(std::ostream &, const BigInt &);
    friend bool operator==(const BigInt &x, const BigInt &y);
    friend bool operator!=(const BigInt &x, const BigInt &y);
    friend bool operator>(const BigInt &x, const BigInt &y);
    friend bool operator>=(const BigInt &x, const BigInt &y);
    friend bool operator<(const BigInt &x, const BigInt &y);
    friend bool operator<=(const BigInt &x, const BigInt &y);
};