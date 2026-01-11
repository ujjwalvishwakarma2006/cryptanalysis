// Code to decrypt affine cipher. 

#include <bits/stdc++.h>
using namespace std;
using pii = std::pair<int, int>;

struct mapping {
public:
    char from, to;

    mapping(char _from, char _to) {
        from = _from;
        to = _to;
    }
};

struct points {
public:
    mapping first, second;
    
    // points(mapping _first, mapping _second) {
    //     first = _first;
    //     second = _second;
    // }

    points(char c1, char p1, char c2, char p2) : first(p1, c1), second(p2, c2) {} 

    void print() {
        cout << first.from << "->" << first.to << 
            "\t" << second.from << "->" << second.to
            << "\n";
    }
};

inline void print_pair(pii p) {
    std::cout << "(" << p.first << ", " << p.second << ")\n";
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
inline pii find_a_b(points map, int n) {
    
    int x1 = map.first.from - 'a';
    int x2 = map.second.from - 'a';
    int r1 = map.first.to - 'A';
    int r2 = map.second.to - 'A';
    int x = ((((x1-x2)%n) + n)%n);
    int inverse = find_inverse(x, n);

    // Ensure inverse exists
    if (inverse == -1) {
        // std::cout << "Failed guess!!!\n";
        return {-1, -1}; 
    }

    int a = (((((r1-r2)%n) + n)%n) * inverse)%n;
    int b = (((r1 - a*x1)%n) + n)%n;

    // Check if inverse of a exists in Zn or not, because we need that while decryption. 
    int a_inverse = find_inverse(a, n);

    if (a_inverse == -1) {
        // std::cout << "Failed guess!!!\n";
        return {-1, -1}; 
    }

    return {a, b};
}

// void run_modulo_tests() {
//     auto result = find_a_b(2, 14, 7, 13, 26);
//     print_pair(result);
// }

void decrypt_and_print(std::string& ciphertext, pii a_b, int n) {

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

    char most_frequent[4] = {'e', 't', 'a', 'o'};
    char most_frequent_ct[4] = {'Z', 'R', 'I', 'M'};
    int n = 26;

    for (char c1 : most_frequent_ct) {
        for (char c2 : most_frequent_ct) {
            for (char p1 : most_frequent) {
                for (char p2 : most_frequent) {
                    if (c1 == c2) continue;
                    if (p1 == p2) continue;
                    points pts(c1, p1, c2, p2); 
                    auto a_b = find_a_b(pts, n);
                    if (a_b != make_pair(-1, -1)) {
                        pts.print();
                        print_pair(a_b);
                        decrypt_and_print(ciphertext, a_b, n);
                    }
                    else {
                        cout << "\tFAILED GUESS!!!\n";
                    }
                }
            }
        }
    }

    return 0;
}
