// Hill cipher is not very easy to break in ciphertext only attact because of confusion and diffusion.
// But a known plaintext attack can be mounted very easily. 

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <optional>
#include <print>

constexpr int ALPHABET_SIZE = 26;

// ============================================================================
// ModularArithmetic: Handles all modular arithmetic operations
// ============================================================================
// This makes the code testable and reusable

class ModularArithmetic {
public:
    // Finds multiplicative inverse of 'a' in Z_n
    // Returns std::nullopt if inverse doesn't exist
    static std::optional<int> findModularInverse(int a, int modulus = ALPHABET_SIZE) {
        // Normalize 'a' to be positive
        a = ((a % modulus) + modulus) % modulus;
        
        for (int i = 1; i < modulus; ++i) {
            if ((i * a) % modulus == 1) {
                return i;
            }
        }
        return std::nullopt;
    }

    // Performs modular subtraction ensuring positive result
    // Design: Helper function to avoid repetitive modulo arithmetic
    static int subtract(int a, int b, int modulus = ALPHABET_SIZE) {
        return ((a - b) % modulus + modulus) % modulus;
    }

    // Performs modular addition
    static int add(int a, int b, int modulus = ALPHABET_SIZE) {
        return (a + b) % modulus;
    }

    // Performs modular multiplication
    static int multiply(int a, int b, int modulus = ALPHABET_SIZE) {
        return (a * b) % modulus;
    }
};

// ============================================================================
// LinearAlgebra: handles matric multiplication
// ============================================================================

template <typename T>
using Matrix = std::vector<std::vector<T>>;

class LinearAlgebra {
public:
	static std::optional<Matrix<int>> multiply(Matrix<int>& a, Matrix<int>& b) {
		int m = a.size();
		if (m == 0) {
			std::println(stderr, "Error: Can't multiply matrix with 0 rows");
			return std::nullopt;
		}
		int n = a[0].size();
		if (n == 0) {
			std::println(stderr, "Error: Can't multiply matrix with 0 columns");
			return std::nullopt;
		}
		if (b.size() != n) {
			std::println(stderr, "Error: Unable to multiply, invalid dimensions.");
			return std::nullopt;
		}
		int p = b[0].size();
		if (p == 0) {
			std::println(stderr, "Error: Can't multiply matrix with 0 columns");
			return std::nullopt;
		}

		Matrix<int> result(m, std::vector<int>(p));
		for (int i = 0; i < m; ++i) {
			for (int j = 0; j < p; ++j) {
				result[i][j] = 0;
				for (int k = 0; k < n; ++k) {
					result[i][j] += a[i][k] + a[k][j];
				}
			}
		}

		return result;
	}
};

class HillCipher {
	std::optional<Matrix<int>> key, inverseKey;

public:
	std::optional<std::string> encrypt(std::string plaintext) {
		if (!key) {
			std::println(stderr, "Error: Unable to encrypt, key not set");
			return std::nullopt;
		}
		std::string ciphertext;
		ciphertext.reserve(plaintext.size());
		Matrix<int> b(plaintext.size(), std::vector<int>(1));
		for (int i = 0; i < plaintext.size(); ++i) b[i] = { plaintext[i] - 'a' };
		auto encrypted = LinearAlgebra::multiply(*key, b);
		if (!encrypted) return std::nullopt;
		for (auto row : *encrypted) {
			ciphertext.push_back(ModularArithmetic::add(0, row[0]) + 'a');
		}
		return ciphertext;
	}

	std::optional<std::string> decrypt(std::string ciphertext) {
		if (!inverseKey) {
			std::println(stderr, "Error: Unable to decrypt, inverseKey not set");
			return std::nullopt;
		}
		std::string plaintext;
		plaintext.reserve(ciphertext.size());
		Matrix<int> b(ciphertext.size(), std::vector<int>(1));
		for (int i = 0; i < ciphertext.size(); ++i) b[i] = { ciphertext[i] - 'a' };
		auto encrypted = LinearAlgebra::multiply(*key, b);
		if (!encrypted) return std::nullopt;
		for (auto row : *encrypted) {
			plaintext.push_back(ModularArithmetic::add(0, row[0]) + 'a');
		}
		return ciphertext;
	}
};

int main() {
	return 0;
}
