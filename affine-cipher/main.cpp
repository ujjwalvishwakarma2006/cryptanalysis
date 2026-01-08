// Code to decrypt affine cipher. 

#include <bits/stdc++.h>

using pii = std::pair<int, int>;

inline void print_pair(pii p) {
    std::cout << "(" << p.first << ", " << p.second << ")\n";
}

inline bool print_inverse_error(int x, int n) {
    std::cout << "Inverse of " << x << " does not exist in Z_" << n << ".\n";
    return false;
}

// Function to find inverse of x in Zn
inline int find_inverse(int x, int n) {
    for (int i = 0; i < n; ++i) {
        if ((i * x)%n == 1) return i;
    }
    return -1;
}

// Function to solve for pair (a,b) given two modular equations:
// ax1 + b = r1 mod n and ax2 + b = r2 mod n
// where a, b, x1, x2, r1, r2 belongs to Zn
// Specifically a belongs to Zn*
inline pii find_a_b(int x1, int r1, int x2, int r2, int n) {
    int x = ((((x1-x2)%n) + n)%n);
    int inverse = find_inverse(x, n);
    // Ensure inverse exists
    // assert(inverse != -1);
    assert(inverse != -1 || print_inverse_error(x, n));
    int a = (((((r1-r2)%n) + n)%n) * inverse)%n;
    int b = (((r1 - a*x1)%n) + n)%n;
    // Check if inverse of a exists in Zn or not, because we need that while decryption. 
    int a_inverse = find_inverse(a, n);
    assert(a_inverse != -1);
    return {a, b}; 
}

void run_modulo_tests() {
    auto result = find_a_b(2, 14, 7, 13, 26);
    print_pair(result);
}

void decrypt_and_print(std::string ciphertext, pii a_b, int n) {
    // Check if a inverse exist or not. 
    int a = a_b.first;
    int b = a_b.second;
    int a_inverse = find_inverse(a, n);
    assert(a_inverse != -1);

    for (auto& c : ciphertext) {
        int ind = c - 'A';
        ind = ind - b;
        ind = ind * a_inverse;
        ind = ((ind%n) + n)%n;
        c = 'a' + ind;
    }

    std::cout << ciphertext << "\n";
}

int main() {
    // run_modulo_tests();

    // ciphertext need to be decrypted.
    std::string ciphertext = "HYVFMRIGJXHMZBTRIZVZUZMHYOZKRURIRFUXFKHMRIGJZIRDQMFDZXXZX";
    std::unordered_map<char, int> char_freq;
    
    for (char c : ciphertext) char_freq[c]++;
    for (auto [letter, count] : char_freq) {
        std::cout << letter << ": " << count << "\n";
    }
    // Highest frequencies
    // Z: 8
    // R: 7
    // I: 5 
    // M: 5 
    // X: 5

    // Frequency analysis (assuming a -> 0 and z -> 25)

    pii a_b;
    
    // First guess (fails): suppose Z(25) is encryption of e(4) and R(17) is of t(19)
    // Plug in these values in the two equations. 
    // a_b = find_a_b(4, 25, 19, 17, 26);
    
    // Second guess (fails): suppose Z(25) is encryption of e(4) and R(17) is of a(0)
    // Plug in these values in the two equations. 
    // a_b = find_a_b(4, 25, 0, 17, 26);
    
    // Second guess (fails): suppose Z(25) is encryption of e(4) and R(17) is of o(14)
    // Plug in these values in the two equations. 
    // a_b = find_a_b(4, 25, 14, 17, 26);
    
    // Second guess (fails): suppose Z(25) is encryption of t(19) and R(17) is of e(5)
    // Plug in these values in the two equations. 
    // a_b = find_a_b(19, 25, 5, 17, 26);
    
    // Second guess (fails): suppose Z(25) is encryption of t(19) and R(17) is of a(0)
    // Plug in these values in the two equations. 
    // a_b = find_a_b(19, 25, 0, 17, 26);
    
    // Second guess (fails): suppose Z(25) is encryption of t(19) and R(17) is of o(14)
    // Plug in these values in the two equations. 
    // a_b = find_a_b(19, 25, 14, 17, 26);
    
    // Second guess (fails): suppose Z(25) is encryption of a(0) and R(17) is of e(4)
    // Plug in these values in the two equations. 
    // a_b = find_a_b(0, 25, 4, 17, 26);
    
    // Second guess (fails): suppose Z(25) is encryption of a(0) and R(17) is of t(19)
    // Plug in these values in the two equations. 
    // a_b = find_a_b(0, 25, 19, 17, 26);
    
    // Second guess (fails): suppose Z(25) is encryption of a(0) and R(17) is of o(14)
    // Plug in these values in the two equations. 
    // a_b = find_a_b(0, 25, 14, 17, 26);
    
    // Second guess (fails): suppose Z(25) is encryption of o(14) and R(17) is of e(4)
    // Plug in these values in the two equations. 
    // a_b = find_a_b(14, 25, 4, 17, 26);
    
    // Second guess (fails): suppose Z(25) is encryption of o(14) and R(17) is of t(19)
    // Plug in these values in the two equations. 
    // a_b = find_a_b(14, 25, 19, 17, 26);
    
    // Second guess (fails): suppose Z(25) is encryption of o(14) and R(17) is of a(0)
    // Plug in these values in the two equations. 
    a_b = find_a_b(14, 25, 0, 17, 26);
    
    print_pair(a_b);
    decrypt_and_print(ciphertext, a_b, 26);
    return 0;
}