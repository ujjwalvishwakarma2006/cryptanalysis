// Code to decrypt affine cipher. 

#include <bits/stdc++.h>
using pii = std::pair<int, int>;

int n = 26;

struct mapping {
public:
    char from, to;

	mapping () {}

    mapping(char _from, char _to) {
        from = _from;
        to = _to;
    }
};

struct points {
public:
    mapping first, second;

	points() {}

    points(char c1, char p1, char c2, char p2) : first(p1, c1), second(p2, c2) {} 

    void print() {
		std::cout << first.from << "->" << first.to << 
            "\t" << second.from << "->" << second.to
            << "\n";
    }
};

class Group {
	int n = 26;
public:

	Group() {}
	Group(int _n) : n(_n) {}

	// Function to find inverse of x in Zn
	inline int inverse(int x) {
		for (int i = 1; i < n; ++i) {
			if ((i * x)%n == 1) return i;
		}
		return -1;
	}
	
	// Function to solve for pair (a,b) given two modular equations:
	// ax1 + b = r1 mod n and ax2 + b = r2 mod n
	// where a, b, x1, x2, r1, r2 belongs to Zn
	// Specifically a belongs to Zn*
	inline pii find_a_b(points map) {
		int x1 = map.first.from - 'a';
		int x2 = map.second.from - 'a';
		int r1 = map.first.to - 'A';
		int r2 = map.second.to - 'A';
		int x = ((((x1-x2)%n) + n)%n);
		int x_inverse = inverse(x);

		// Ensure inverse exists
		if (x_inverse == -1) {
			return {-1, -1}; 
		}

		int a = (((((r1-r2)%n) + n)%n) * x_inverse)%n;
		int b = (((r1 - a*x1)%n) + n)%n;

		return {a, b};
	}
};

class AffineCipher {
	char encrypt_char(char ch) {
		if (ch > 'z' || ch < 'a') {
			std::cout << "Unable to encrypt character: " << ch << "\n\tExpects only lowercase english alphabets\n";
			return '0';
		}	

		if (!key.exists()) {
			std::cout << "Key required to encrypt the plaintext, please set a key first.\n";
			return '0';
		}

		int char_ind = ch - 'a';
		char encrypted = (key.a * char_ind + key.b) % n;
		encrypted = encrypted + 'A';

		return encrypted;
	}

	char decrypt_char(char ch) {
		if (ch > 'Z' || ch < 'A') {
			std::cout << "Unable to decrypt character: " << ch << "\n\tExpects only lowercase english alphabets\n";
			return '0';
		}

		if (!key.exists()) {
			std::cout << "Key required to encrypt the plaintext, please set a key first.\n";
			return '0';
		}

		int char_ind = ch - 'A';
		char decrypted = (key.a_inv * (n + ((char_ind - key.b) % n))) % n;
		decrypted = decrypted + 'a';

		return decrypted;
	}

public:
	struct Key {
		Group* G = new Group();
		int a;
		int b;
		int a_inv;

		Key() {}

		Key(int _a, int _b) {
			set(_a, _b);
		}
		
		bool exists() {
			return keyExists;
		}

		bool set(int _a, int _b) {
			int _a_inv = G->inverse(_a);

			if (_a_inv == -1) {
				std::cout << "\tCannot set key.a = " << _a << "\n\tInverse won't exist in Z_26\n";
				return false;
			}

			a = _a;
			b = _b;
			a_inv = _a_inv;
			keyExists= true;
			return true;
		}

		private:
			bool keyExists= false;
	};
	
	Key key;

	AffineCipher() {}

	std::string encrypt(std::string& plaintext) {
		std::string encrypted;
		encrypted.reserve(plaintext.length());	

		for (char ch : plaintext) {
			char ch_encrypted = encrypt_char(ch);
			if (ch_encrypted == '0') return "";
			encrypted.push_back(ch_encrypted);
		}

		return encrypted;
	}

	std::string decrypt(std::string& ciphertext) {
		std::string decrypted;
		decrypted.reserve(ciphertext.length());

		for (char ch : ciphertext) {
			char ch_decrypted = decrypt_char(ch);
			if (ch_decrypted == '0') return "";
			decrypted.push_back(ch_decrypted);
		}

		return decrypted;
	}

	bool deduceKey(std::string plaintext, std::string ciphertext) {
		int string_length = plaintext.length();

		if (ciphertext.length() != plaintext.length()) {
			std::cout << "Can't deduce the key. Lenghts of plaintext and ciphertext are different.\n";
			return false;
		}

		points pts;
		bool keysFound = false;
		for (int i = 1; i < plaintext.length(); ++i) {
			if (plaintext[i] == plaintext[i - 1]) continue;
			keysFound = true;
			pts = points(ciphertext[i], plaintext[i], ciphertext[i - 1], plaintext[i - 1]);
		}

		if (!keysFound) {
			std::cout << "Can't deduce the key. Insufficient information from the given plaintext-ciphertext pair.\n";
			return false;
		}

		auto p = key.G->find_a_b(pts);
		if (key.set(p.first, p.second) == false) {
			// std::cout << "\tCan't deduce the key. Inverse doesn't exist.\n";
			return false;
		}

		return true;
	}
};

int main() {
	AffineCipher af;

    // ciphertext need to be decrypted.
    std::string ciphertext = "HYVFMRIGJXHMZBTRIZVZUZMHYOZKRURIRFUXFKHMRIGJZIRDQMFDZXXZX";
    std::unordered_map<char, int> char_freq;
    
    for (char c : ciphertext) char_freq[c]++;
    for (auto [letter, count] : char_freq) std::cout << letter << ": " << count << "\n";
    // Highest frequencies
    // Z: 8
    // R: 7
    // I: 5 
    // M: 5 
    // X: 5

    // Frequency analysis (assuming a -> 0 and z -> 25)
    char most_frequent[4] = {'e', 't', 'a', 'o'};
    char most_frequent_ct[4] = {'Z', 'R', 'I', 'M'};

    for (char c1 : most_frequent_ct) {
        for (char c2 : most_frequent_ct) {
			if (c1 == c2) continue;
            for (char p1 : most_frequent) {
                for (char p2 : most_frequent) {
                    if (p1 == p2) continue;
					std::string from_plaintext = std::string(1, p1) + p2;
					std::string to_ciphertext = std::string(1, c1) + c2;
					std::cout << from_plaintext << " -> " << to_ciphertext << "\n";
					if (!af.deduceKey(from_plaintext, to_ciphertext)) continue;
					std::string decrypted = af.decrypt(ciphertext);
					std::cout << decrypted << "\n"; 
                }
            }
        }
    }

    return 0;
}
