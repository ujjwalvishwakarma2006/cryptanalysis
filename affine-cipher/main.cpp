// Affine Cipher Cryptanalysis Tool
// Implements encryption, decryption, and frequency-based attack on affine ciphers

#include <iostream>
#include <unordered_map>
#include <optional>
#include <string>
#include <vector>
#include <algorithm>

// ============================================================================
// Constants
// ============================================================================

constexpr int ALPHABET_SIZE = 26;

// ============================================================================
// ModularArithmetic: Handles all modular arithmetic operations
// ============================================================================
// Design Decision: Separate math operations from cipher logic
// This makes the code testable and reusable

class ModularArithmetic {
public:
    // Finds multiplicative inverse of 'a' in Z_n
    // Returns std::nullopt if inverse doesn't exist
    // Design: std::optional clearly signals "might not exist"
    static std::optional<int> findModularInverse(int a, int modulus) {
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
    static int subtract(int a, int b, int modulus) {
        return ((a - b) % modulus + modulus) % modulus;
    }

    // Performs modular addition
    static int add(int a, int b, int modulus) {
        return (a + b) % modulus;
    }

    // Performs modular multiplication
    static int multiply(int a, int b, int modulus) {
        return (a * b) % modulus;
    }
};

// ============================================================================
// AffineKey: Represents the key pair (a, b) for affine cipher
// ============================================================================
// Design Decision: Encapsulate key validation and inverse calculation
// This ensures keys are always in a valid state

struct AffineKey {
    int a;          // Multiplicative component (must be coprime with 26)
    int b;          // Additive component
    int a_inverse;  // Cached inverse of 'a' for decryption
	bool valid;		// `true` if key is set

    AffineKey(int a_val, int b_val, int a_inv) 
        : a(a_val), b(b_val), a_inverse(a_inv) {}

    // Factory method: Creates key only if valid
    // Design: Static factory pattern ensures object invariants
    static std::optional<AffineKey> create(int a, int b) {
        auto inverse = ModularArithmetic::findModularInverse(a, ALPHABET_SIZE);
        
        if (!inverse) {
            return std::nullopt;  // 'a' has no inverse
        }
        
        return AffineKey(a, b, *inverse);
    }

    void print() const {
        std::cout << "Key: a=" << a << ", b=" << b 
                  << " (a^-1=" << a_inverse << ")\n";
    }
};

// ============================================================================
// KnownPlaintextPair: Represents two known plaintext-ciphertext mappings
// ============================================================================
// Design Decision: Clear name shows this is for known-plaintext attack
// Used to solve the system of linear equations for affine parameters

struct KnownPlaintextPair {
    char plaintext1, ciphertext1;
    char plaintext2, ciphertext2;

    KnownPlaintextPair(char p1, char c1, char p2, char c2)
        : plaintext1(p1), ciphertext1(c1), plaintext2(p2), ciphertext2(c2) {}

    void print() const {
        std::cout << "Known pairs: " 
                  << plaintext1 << "->" << ciphertext1 << ", "
                  << plaintext2 << "->" << ciphertext2 << "\n";
    }

    // Validates that the pair provides useful information
    bool isValid() const {
        return plaintext1 != plaintext2;  // Must be distinct
    }
};

// ============================================================================
// AffineCipher: Core cipher implementation
// ============================================================================
// Design Decision: Focuses solely on encryption/decryption mechanics
// Cryptanalysis is handled by separate class

class AffineCipher {
private:
    std::optional<AffineKey> key_;

    // Helper: Encrypts single character
    // Design: Private helper keeps public API clean
    std::optional<char> encryptChar(char ch) const {
        if (!key_) {
            return std::nullopt;
        }

        if (ch < 'a' || ch > 'z') {
            return std::nullopt;  // Invalid input
        }

        int index = ch - 'a';
        int encrypted_index = ModularArithmetic::add(
            ModularArithmetic::multiply(key_->a, index, ALPHABET_SIZE),
            key_->b,
            ALPHABET_SIZE
        );
        
        return static_cast<char>('A' + encrypted_index);
    }

    // Helper: Decrypts single character
    std::optional<char> decryptChar(char ch) const {
        if (!key_) {
            return std::nullopt;
        }

        if (ch < 'A' || ch > 'Z') {
            return std::nullopt;
        }

        int index = ch - 'A';
        int shifted = ModularArithmetic::subtract(index, key_->b, ALPHABET_SIZE);
        int decrypted_index = ModularArithmetic::multiply(
            key_->a_inverse,
            shifted,
            ALPHABET_SIZE
        );
        
        return static_cast<char>('a' + decrypted_index);
    }

public:
    AffineCipher() = default;

    // Constructor with key
    explicit AffineCipher(const AffineKey& key) : key_(key) {}

    // Sets encryption/decryption key
    bool setKey(int a, int b) {
        key_ = AffineKey::create(a, b);
        return key_.has_value();
    }

    bool hasKey() const {
        return key_.has_value();
    }

    const AffineKey& getKey() const {
        return *key_;
    }

    // Encrypts plaintext (lowercase) to ciphertext (uppercase)
    std::optional<std::string> encrypt(const std::string& plaintext) const {
        if (!key_) {
            std::cerr << "Error: No key set for encryption\n";
            return std::nullopt;
        }

        std::string ciphertext;
        ciphertext.reserve(plaintext.length());

        for (char ch : plaintext) {
            auto encrypted = encryptChar(ch);
            if (!encrypted) {
                std::cerr << "Error: Invalid character '" << ch 
                          << "' in plaintext\n";
                return std::nullopt;
            }
            ciphertext.push_back(*encrypted);
        }

        return ciphertext;
    }

    // Decrypts ciphertext (uppercase) to plaintext (lowercase)
    std::optional<std::string> decrypt(const std::string& ciphertext) const {
        if (!key_) {
            std::cerr << "Error: No key set for decryption\n";
            return std::nullopt;
        }

        std::string plaintext;
        plaintext.reserve(ciphertext.length());

        for (char ch : ciphertext) {
            auto decrypted = decryptChar(ch);
            if (!decrypted) {
                std::cerr << "Error: Invalid character '" << ch 
                          << "' in ciphertext\n";
                return std::nullopt;
            }
            plaintext.push_back(*decrypted);
        }

        return plaintext;
    }
};

// ============================================================================
// AffineCryptanalysis: Handles breaking the cipher
// ============================================================================
// Design Decision: Separate cryptanalysis from cipher implementation
// This follows SRP and makes the code easier to extend

class AffineCryptanalysis {
public:
    // Solves for affine parameters (a, b) given two known plaintext-ciphertext pairs
    // Solves the system: a*x1 + b ≡ y1 (mod 26) and a*x2 + b ≡ y2 (mod 26)
    // Returns std::nullopt if no valid solution exists
    static std::optional<AffineKey> solveAffineParameters(
        const KnownPlaintextPair& pair
    ) {
        if (!pair.isValid()) {
            return std::nullopt;
        }

        // Convert characters to numeric indices
        int x1 = pair.plaintext1 - 'a';
        int x2 = pair.plaintext2 - 'a';
        int y1 = pair.ciphertext1 - 'A';
        int y2 = pair.ciphertext2 - 'A';

        // Solve: a*(x1-x2) ≡ (y1-y2) (mod 26)
        int x_diff = ModularArithmetic::subtract(x1, x2, ALPHABET_SIZE);
        auto x_diff_inverse = ModularArithmetic::findModularInverse(
            x_diff, ALPHABET_SIZE
        );

        if (!x_diff_inverse) {
            return std::nullopt;  // No solution exists
        }

        int y_diff = ModularArithmetic::subtract(y1, y2, ALPHABET_SIZE);
        int a = ModularArithmetic::multiply(y_diff, *x_diff_inverse, ALPHABET_SIZE);

        // Solve: b ≡ y1 - a*x1 (mod 26)
        int b = ModularArithmetic::subtract(
            y1,
            ModularArithmetic::multiply(a, x1, ALPHABET_SIZE),
            ALPHABET_SIZE
        );

        return AffineKey::create(a, b);
    }

    // Performs frequency analysis attack on ciphertext
    // Tries different mappings of frequent ciphertext letters to frequent plaintext letters
    static std::vector<std::string> frequencyAttack(
        const std::string& ciphertext,
        const std::vector<char>& likely_plaintext_chars = {'e', 't', 'a', 'o'},
        int max_results = 5
    ) {
        // Step 1: Frequency analysis
        auto frequent_ciphertext = getFrequentCharacters(ciphertext, likely_plaintext_chars.size());

        std::cout << "Frequent ciphertext characters: ";
        for (char c : frequent_ciphertext) std::cout << c << " ";
        std::cout << "\n\n";

        // Step 2: Try different mappings
        std::vector<std::string> candidates;
        AffineCipher cipher;

        for (char c1 : frequent_ciphertext) {
            for (char c2 : frequent_ciphertext) {
                if (c1 == c2) continue;

                for (char p1 : likely_plaintext_chars) {
                    for (char p2 : likely_plaintext_chars) {
                        if (p1 == p2) continue;

                        KnownPlaintextPair pair(p1, c1, p2, c2);
                        auto key = solveAffineParameters(pair);

                        if (!key) continue;

                        cipher = AffineCipher(*key);
                        auto decrypted = cipher.decrypt(ciphertext);

                        if (decrypted) {
                            candidates.push_back(*decrypted);
                            
                            if (candidates.size() >= max_results) {
                                return candidates;
                            }
                        }
                    }
                }
            }
        }

        return candidates;
    }

private:
    // Analyzes character frequency in text
    static std::vector<char> getFrequentCharacters(
        const std::string& text,
        size_t count
    ) {
        std::unordered_map<char, int> freq;
        
        for (char c : text) {
            freq[c]++;
        }

        // Sort by frequency
        std::vector<std::pair<char, int>> freq_vec(freq.begin(), freq.end());
        std::sort(freq_vec.begin(), freq_vec.end(),
            [](const auto& a, const auto& b) {
                return a.second > b.second;  // Descending order
            });

        // Extract top characters
        std::vector<char> result;
        for (size_t i = 0; i < std::min(count, freq_vec.size()); ++i) {
            result.push_back(freq_vec[i].first);
        }

        return result;
    }
};

// ============================================================================
// Main: Demonstrates usage
// ============================================================================

int main() {
    std::cout << "===== Affine Cipher Cryptanalysis =====\n\n";

    const std::string ciphertext = 
        "HYVFMRIGJXHMZBTRIZVZUZMHYOZKRURIRFUXFKHMRIGJZIRDQMFDZXXZX";

    std::cout << "Ciphertext: " << ciphertext << "\n\n";

    // Perform frequency attack
    std::cout << "Performing frequency analysis attack...\n";
    auto candidates = AffineCryptanalysis::frequencyAttack(ciphertext, {'e', 't', 'a', 'o'}, 10);

    std::cout << "\nPossible plaintexts:\n";
    std::cout << "----------------------------------------\n";
    for (size_t i = 0; i < candidates.size(); ++i) {
        std::cout << i + 1 << ". " << candidates[i] << "\n";
    }

    return 0;
}