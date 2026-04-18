#include <algorithm>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

class ModularArithmetic {
public:
    static int subtract(int a, int b, int modulus) {
        return ((a - b) % modulus + modulus) % modulus;
    }

    static int add(int a, int b, int modulus) {
        return (a + b) % modulus;
    }

    static int multiply(int a, int b, int modulus) {
        return static_cast<int>((1LL * a * b) % modulus);
    }
};

class Number {
private:
    // Decimal digits in little-endian order: 123 is stored as {3, 2, 1}.
    std::vector<int> digits;

    void trim() {
        while (digits.size() > 1 && digits.back() == 0) {
            digits.pop_back();
        }
    }

    static int compare(const Number& a, const Number& b) {
        if (a.digits.size() != b.digits.size()) {
            return (a.digits.size() < b.digits.size()) ? -1 : 1;
        }

        for (int i = static_cast<int>(a.digits.size()) - 1; i >= 0; --i) {
            if (a.digits[i] != b.digits[i]) {
                return (a.digits[i] < b.digits[i]) ? -1 : 1;
            }
        }
        return 0;
    }

    void multiplyBy10() {
        if (isZero()) {
            return;
        }
        digits.insert(digits.begin(), 0);
    }

    void addSmall(int value) {
        if (value < 0 || value > 9) {
            throw std::invalid_argument("addSmall expects a digit in [0, 9]");
        }

        int carry = value;
        std::size_t i = 0;
        while (carry > 0) {
            if (i == digits.size()) {
                digits.push_back(0);
            }
            int sum = digits[i] + carry;
            digits[i] = sum % 10;
            carry = sum / 10;
            ++i;
        }
    }

    static Number multiplyByDigit(const Number& n, int digit) {
        if (digit < 0 || digit > 9) {
            throw std::invalid_argument("multiplyByDigit expects a digit in [0, 9]");
        }
        if (digit == 0 || n.isZero()) {
            return Number(0);
        }

        Number result;
        result.digits.assign(n.digits.size() + 1, 0);
        int carry = 0;
        for (std::size_t i = 0; i < n.digits.size(); ++i) {
            int prod = n.digits[i] * digit + carry;
            result.digits[i] = prod % 10;
            carry = prod / 10;
        }
        if (carry > 0) {
            result.digits[n.digits.size()] = carry;
        }
        result.trim();
        return result;
    }

    static std::pair<Number, Number> divmod(const Number& dividend, const Number& divisor) {
        if (divisor.isZero()) {
            throw std::invalid_argument("division by zero");
        }
        if (dividend < divisor) {
            return {Number(0), dividend};
        }

        Number remainder(0);
        std::vector<int> quotientMsdFirst;
        quotientMsdFirst.reserve(dividend.digits.size());

        for (int i = static_cast<int>(dividend.digits.size()) - 1; i >= 0; --i) {
            remainder.multiplyBy10();
            remainder.addSmall(dividend.digits[i]);

            int low = 0;
            int high = 9;
            int qDigit = 0;

            while (low <= high) {
                int mid = (low + high) / 2;
                Number guess = multiplyByDigit(divisor, mid);
                if (guess <= remainder) {
                    qDigit = mid;
                    low = mid + 1;
                } else {
                    high = mid - 1;
                }
            }

            quotientMsdFirst.push_back(qDigit);
            if (qDigit > 0) {
                remainder = remainder - multiplyByDigit(divisor, qDigit);
            }
        }

        Number quotient;
        quotient.digits.clear();
        int firstNonZero = 0;
        while (firstNonZero < static_cast<int>(quotientMsdFirst.size()) - 1 && quotientMsdFirst[firstNonZero] == 0) {
            ++firstNonZero;
        }
        for (int i = static_cast<int>(quotientMsdFirst.size()) - 1; i >= firstNonZero; --i) {
            quotient.digits.push_back(quotientMsdFirst[i]);
        }
        quotient.trim();
        remainder.trim();

        return {quotient, remainder};
    }

public:
    Number() : digits(1, 0) {}

    Number(unsigned long long value) {
        if (value == 0) {
            digits = {0};
            return;
        }

        while (value > 0) {
            digits.push_back(static_cast<int>(value % 10ULL));
            value /= 10ULL;
        }
    }

    explicit Number(const std::string& numStr) {
        if (numStr.empty()) {
            throw std::invalid_argument("number string cannot be empty");
        }

        for (char ch : numStr) {
            if (!std::isdigit(static_cast<unsigned char>(ch))) {
                throw std::invalid_argument("number string must contain only digits");
            }
        }

        int i = 0;
        while (i < static_cast<int>(numStr.size()) - 1 && numStr[i] == '0') {
            ++i;
        }

        for (int j = static_cast<int>(numStr.size()) - 1; j >= i; --j) {
            digits.push_back(numStr[j] - '0');
        }
        trim();
    }

    static Number rand(const std::string& scale) {
        if (scale.empty()) {
            throw std::invalid_argument("scale cannot be empty");
        }
        for (char ch : scale) {
            if (!std::isdigit(static_cast<unsigned char>(ch))) {
                throw std::invalid_argument("scale must contain only digits");
            }
        }

        Number upper(scale);
        if (upper < Number(1)) {
            throw std::invalid_argument("scale must be >= 1");
        }

        int len = static_cast<int>(upper.toString().size());
        static thread_local std::mt19937_64 rng(std::random_device{}());
        std::uniform_int_distribution<int> digitDist(0, 9);

        while (true) {
            std::string s(len, '0');
            for (int i = 0; i < len; ++i) {
                s[i] = static_cast<char>('0' + digitDist(rng));
            }

            Number candidate(s);
            if (candidate >= Number(1) && candidate <= upper) {
                return candidate;
            }
        }
    }

    std::string toString() const {
        std::string result;
        result.reserve(digits.size());
        for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
            result.push_back(static_cast<char>('0' + digits[i]));
        }
        return result;
    }

    bool isZero() const {
        return digits.size() == 1 && digits[0] == 0;
    }

    bool isEven() const {
        return (digits[0] % 2) == 0;
    }

    int modSmall(int m) const {
        if (m <= 0) {
            throw std::invalid_argument("modulus must be positive");
        }

        int remainder = 0;
        for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
            remainder = (remainder * 10 + digits[i]) % m;
        }
        return remainder;
    }

    Number half() const {
        Number result;
        result.digits.assign(digits.size(), 0);

        int carry = 0;
        for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
            int cur = carry * 10 + digits[i];
            result.digits[i] = cur / 2;
            carry = cur % 2;
        }
        result.trim();
        return result;
    }

    friend bool operator<(const Number& a, const Number& b) {
        return compare(a, b) < 0;
    }

    friend bool operator>(const Number& a, const Number& b) {
        return compare(a, b) > 0;
    }

    friend bool operator<=(const Number& a, const Number& b) {
        return compare(a, b) <= 0;
    }

    friend bool operator>=(const Number& a, const Number& b) {
        return compare(a, b) >= 0;
    }

    friend bool operator==(const Number& a, const Number& b) {
        return compare(a, b) == 0;
    }

    friend bool operator!=(const Number& a, const Number& b) {
        return compare(a, b) != 0;
    }

    Number operator+(const Number& other) const {
        Number result;
        result.digits.assign(std::max(digits.size(), other.digits.size()) + 1, 0);

        int carry = 0;
        for (std::size_t i = 0; i < result.digits.size(); ++i) {
            int a = (i < digits.size()) ? digits[i] : 0;
            int b = (i < other.digits.size()) ? other.digits[i] : 0;
            int sum = a + b + carry;
            result.digits[i] = sum % 10;
            carry = sum / 10;
        }
        result.trim();
        return result;
    }

    Number operator-(const Number& other) const {
        if (*this < other) {
            throw std::invalid_argument("negative result is not supported");
        }

        Number result;
        result.digits.assign(digits.size(), 0);

        int borrow = 0;
        for (std::size_t i = 0; i < digits.size(); ++i) {
            int a = digits[i] - borrow;
            int b = (i < other.digits.size()) ? other.digits[i] : 0;
            if (a < b) {
                a += 10;
                borrow = 1;
            } else {
                borrow = 0;
            }
            result.digits[i] = a - b;
        }
        result.trim();
        return result;
    }

    Number operator*(const Number& other) const {
        if (isZero() || other.isZero()) {
            return Number(0);
        }

        Number result;
        result.digits.assign(digits.size() + other.digits.size(), 0);

        for (std::size_t i = 0; i < digits.size(); ++i) {
            int carry = 0;
            for (std::size_t j = 0; j < other.digits.size(); ++j) {
                int idx = static_cast<int>(i + j);
                int val = result.digits[idx] + digits[i] * other.digits[j] + carry;
                result.digits[idx] = val % 10;
                carry = val / 10;
            }

            std::size_t idx = i + other.digits.size();
            while (carry > 0) {
                int val = result.digits[idx] + carry;
                result.digits[idx] = val % 10;
                carry = val / 10;
                ++idx;
                if (idx == result.digits.size() && carry > 0) {
                    result.digits.push_back(0);
                }
            }
        }
        result.trim();
        return result;
    }

    Number operator/(const Number& other) const {
        return divmod(*this, other).first;
    }

    Number operator%(const Number& other) const {
        return divmod(*this, other).second;
    }
};

class MillerRabin {
private:
    static std::string residueLabel(const Number& x, const Number& nMinusOne) {
        if (x == Number(1)) {
            return "+1";
        }
        if (x == nMinusOne) {
            return "-1";
        }
        return "not +/-1";
    }

    static std::string toBinary(Number n) {
        if (n.isZero()) {
            return "0";
        }

        std::string bits;
        while (!n.isZero()) {
            bits.push_back(n.isEven() ? '0' : '1');
            n = n.half();
        }
        std::reverse(bits.begin(), bits.end());
        return bits;
    }

    static Number randomInRange(const Number& low, const Number& high, std::mt19937_64& rng) {
        if (low > high) {
            throw std::invalid_argument("invalid random range");
        }
        if (low == high) {
            return low;
        }

        Number span = high - low + Number(1);
        int len = static_cast<int>(span.toString().size());
        std::uniform_int_distribution<int> digitDist(0, 9);

        while (true) {
            std::string candidateStr(len, '0');
            for (int i = 0; i < len; ++i) {
                candidateStr[i] = static_cast<char>('0' + digitDist(rng));
            }

            Number candidate(candidateStr);
            if (candidate < span) {
                return low + candidate;
            }
        }
    }

    static Number modPow(Number base, Number exp, const Number& modulus) {
        Number result(1);
        base = base % modulus;

        while (!exp.isZero()) {
            if (!exp.isEven()) {
                result = (result * base) % modulus;
            }
            exp = exp.half();
            base = (base * base) % modulus;
        }
        return result;
    }

    static Number modPowVerbose(Number base, const Number& exp, const Number& modulus, const Number& nMinusOne, std::ostream& out) {
        Number result(1);
        base = base % modulus;

        std::string bits = toBinary(exp);
        out << "  binary(m) = " << bits << "\n";
        out << "  Start: result class = +1\n";

        for (std::size_t i = 0; i < bits.size(); ++i) {
            char bit = bits[i];

            result = (result * result) % modulus;
            out
                << "    Step " << (i + 1)
                << " square => " << residueLabel(result, nMinusOne) << "\n";

            if (bit == '1') {
                result = (result * base) % modulus;
                out
                    << "           multiply by a => " << residueLabel(result, nMinusOne) << "\n";
            }
        }

        return result;
    }

public:
    static bool isProbablePrime(const Number& n, int rounds = 8) {
        static const int smallPrimes[] = {
            2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37,
            41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97
        };

        std::cout << "n = " << n.toString() << "\n";

        if (n < Number(2)) {
            std::cout << "n < 2 => composite\n";
            std::cout << "Prime probability (Miller-Rabin bound) = 0\n\n";
            return false;
        }

        if (n == Number(2) || n == Number(3)) {
            std::cout << "n is a small prime by definition\n";
            std::cout << "Prime probability = 1\n\n";
            return true;
        }

        if (n.isEven()) {
            std::cout << "n is even and > 2 => composite\n";
            std::cout << "Prime probability (Miller-Rabin bound) = 0\n\n";
            return false;
        }

        bool divisibleBySmallPrime = false;
        int foundSmallFactor = -1;
        for (int p : smallPrimes) {
            Number prime(static_cast<unsigned long long>(p));
            if (n == prime) {
                std::cout << "n equals small prime " << p << "\n";
                std::cout << "Prime probability = 1\n\n";
                return true;
            }
            if (n.modSmall(p) == 0) {
                divisibleBySmallPrime = true;
                foundSmallFactor = p;
                break;
            }
        }

        Number d = n - Number(1);
        int s = 0;
        while (d.isEven()) {
            d = d.half();
            ++s;
        }

        Number nMinusOne = n - Number(1);
        Number nMinusTwo = n - Number(2);

        std::cout << "n - 1 = 2^k * m\n";
        std::cout << "k = " << s << "\n";
        std::cout << "m = " << d.toString() << "\n\n";

        if (divisibleBySmallPrime) {
            std::cout << "Pre-check: n is divisible by small prime " << foundSmallFactor << " => composite for sure\n";
            std::cout << "Continuing with Miller-Rabin rounds for a full trace.\n\n";
        }

        std::random_device rd;
        std::mt19937_64 rng(rd());

        for (int round = 1; round <= rounds; ++round) {
            Number a = randomInRange(Number(2), nMinusTwo, rng);
            std::cout << "Round " << round << ": a = " << a.toString() << "\n";
            std::cout << "  Compute x = a^m mod n via square-and-multiply\n";

            Number x = modPowVerbose(a, d, n, nMinusOne, std::cout);
            std::cout
                << "  Final x class for a^m mod n => " << residueLabel(x, nMinusOne) << "\n";

            if (x == Number(1) || x == nMinusOne) {
                std::cout << "  Round result: inconclusive (candidate survives this round)\n\n";
                continue;
            }

            bool reachedMinusOne = false;
            for (int r = 1; r <= s - 1; ++r) {
                x = (x * x) % n;
                std::cout
                    << "  r = " << r
                    << " : x = x^2 mod n => " << residueLabel(x, nMinusOne) << "\n";

                if (x == nMinusOne) {
                    reachedMinusOne = true;
                    break;
                }

                // If x becomes +1 before hitting -1, n is definitely composite.
                if (x == Number(1)) {
                    break;
                }
            }

            if (!reachedMinusOne) {
                std::cout << "  Round result: witness found => composite\n";
                std::cout << "Prime probability (Miller-Rabin bound) = 0\n\n";
                return false;
            }

            std::cout << "  Round result: inconclusive after squaring chain\n\n";
        }

        if (divisibleBySmallPrime) {
            std::cout << "All rounds inconclusive, but small-prime divisibility already proved composite.\n";
            std::cout << "Prime probability (Miller-Rabin bound) = 0\n\n";
            return false;
        }

        long double falsePrimeUpperBound = std::pow(0.25L, static_cast<long double>(rounds));
        long double confidence = 1.0L - falsePrimeUpperBound;
        std::cout << "All rounds inconclusive.\n";
        std::cout << "False-prime upper bound <= 4^-" << rounds << " = "
                  << std::setprecision(18) << falsePrimeUpperBound << "\n";
        std::cout << "Prime probability lower bound >= "
                  << std::setprecision(18) << confidence << "\n\n";

        return true;
    }
};

int main() {
    struct TestCase {
        std::string label;
        std::string value;
    };

    const std::vector<TestCase> tests = {
        {"Number 0", "1731"},
        {"Number 1", "1729"},
        {"Number 2", "56125680981752282333498088313568935051383833838594899821664631784577337171193624243181360054669678410455329112434552942717084003541384594864129940145043086760031292483340068923506115878221189886491132772739661669044958531131327771"},
        {"Number 3", "56125680981752282333498088313568935051383833838594899821664631784577337171193624243181360054669678410455329112434552942717084003541384594864129940145043086760031292483340068923506115878221189886491132772739661669044958531131327773"}
    };

    std::cout << "Miller-Rabin primality test report (random bases)\n\n";

    for (const TestCase& test : tests) {
        Number n(test.value);
        std::cout << "============================================================\n";
        std::cout << test.label << "\n";
        bool prime = MillerRabin::isProbablePrime(n, 6);
        std::cout << "Summary: " << (prime ? "probably prime" : "composite") << "\n";
        std::cout << "============================================================\n\n";
    }

    return 0;
}