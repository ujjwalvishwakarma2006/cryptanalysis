#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

long long mod_exp(long long base, long long exp, long long mod) {
    long long result = 1;
    base %= mod;

    while (exp > 0) {
        if (exp % 2)
            result = (__int128)result * base % mod;
        base = (__int128)base * base % mod;
        exp /= 2;
    }
    return result;
}

bool is_prime(long long n) {
    cout << "n = " << n;
    for (long long i = 1; i < n; ++i) {
        // if (__gcd(i, n) != 1LL) continue;
        // Check flt's converse
        auto mod = mod_exp(i, n-1, n);
        if ((mod % n) != 1) {
            cout 
                << " Failed at a = " << i 
                << " a^n-1 = " << mod << "\n";
            return false;
        }
    }
    cout << " is prime\n";
    return true;
}

int main() {
    is_prime(1731);
    is_prime(1729);
    is_prime(561);
}