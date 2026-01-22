// This file implements Kasiski's test to decipher a given ciphertext that was enciphered using vigenere cipher.

#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <print> // Using C++ 23 (:
#include <optional>

class VigenereCipher {
public:
	// Maps to store key and inverse key. 
	std::unordered_map<char, char> key;
	std::unordered_map<char, char> inverseKey;

	SubstitutionCipher() {
		// Initialize all keys to `-`.
		for (char ch = 'A'; ch <= 'Z'; ++ch) {
			inverseKey[ch] = '-';
		}
	}

	// Function to add keys in the map in increamental order. 
	// One can add the key into the map as soon as he becomes sure about it. 
	// `p`: plaintext character
	// `c`: ciphertext character which the cryptanalyst thinks `p` maps to
	void addKey(char p, char c) {
		if (p < 'a' || p > 'z') {
			std::println(stderr, "Error: Plaintext should consist of only lowercase english letters.");
			return;
		}

		if (c < 'A' || c > 'Z') {
			std::println(stderr, "Error: Ciphertext should consist of only lowercase english letters.");
			return;
		}

		key[p] = c;
		inverseKey[c] = p;
	}

	std::optional<std::string> decrypt(std::string ciphertext) {
		std::string decrypted;
		decrypted.reserve(ciphertext.length());

		for (char ch : ciphertext) {
			decrypted.push_back(inverseKey[ch]);
		}

		return decrypted;
	}

	std::optional<std::string> encrypt(std::string plaintext) {
		std::string encrypted;
		encrypted.reserve(plaintext.length());

		for (char ch : plaintext) {
			if (key.find(ch) == key.end()) {
				std::println(stderr, "Error: Cannot encrypt character: {0} Please provide a key first.", ch);
				return std::nullopt;
			}
			encrypted.push_back(key[ch]);
		}

		return encrypted;
	}

	void decryptAndPrint(std::string ciphertext, int cols = 100) {
		auto decrypted = decrypt(ciphertext);
		if (decrypted == std::nullopt) return;

		int ind = 0;
		while (ind < ciphertext.length() - cols) {
			std::println(stdout, "{}", ciphertext.substr(ind, cols));
			std::println(stdout, "{}", (*decrypted).substr(ind, cols));
			ind += cols;
		}

		std::println(stdout, "{}", ciphertext.substr(ind, std::min(cols, int(ciphertext.length()) - ind)));
		std::println(stdout, "{}", (*decrypted).substr(ind, std::min(cols, int(ciphertext.length()) - ind)));
		std::println();
	}

	void addDecryptAndPrint(char p, char c, std::string ciphertext) {
		addKey(p, c);
		decryptAndPrint(ciphertext);
	}
};

void printFrequenciesSorted(std::unordered_map<std::string, int> freqMap, int cols) {
	std::vector<std::pair<int, std::string>> freqArray;
	for (auto [k, v] : freqMap) {
		freqArray.push_back({v, k});
	}

	std::sort(freqArray.begin(), freqArray.end());
	std::reverse(freqArray.begin(), freqArray.end());

	int printedCols = cols;
	for (auto [v, k] : freqArray) {
		std::print(stdout, "{0}: {1}\t", k, v);
		if (printedCols-- == 0) {
			printedCols = cols;
			std::println();
		}
	}

	std::println();
	std::println();
}

int main() {
	std::string ciphertext = "qwgbnnkywgbonsaqcjkbjbrorhjhnonzglxmlmmnxsqvrbochmqrxycyaqrfjbucxdkprqxrqaaaqzghpkojqqobnluuydawbixrvjwwozhvbnbubdqxpnufkdoadcorlmwcynodxhbewqntjjiqwgbnnkyyhopdqxpzzdrdqhujyxcbdsfxuunonzglxmlmppqqfsqlyniewqxjbqowhljbyzszowubqorryqqevdfwwtyrmxzlbmllqkkumxslxjxzfgxewiexfdabjuqfqdjjfkdvyjdefziajdpdqbidstizppnhfkzkacxqudri";

	std::unordered_map<std::string, int> monogram, digram, trigram;

	// Populate monogram, digram, and trigram frequencies for frequency analysis.
	for (int i = 0; i < ciphertext.length(); ++i) monogram[ciphertext.substr(i, 1)]++;
	for (int i = 0; i < ciphertext.length() - 1; ++i) digram[ciphertext.substr(i, 2)]++;
	for (int i = 0; i < ciphertext.length() - 2; ++i) trigram[ciphertext.substr(i, 3)]++;

	printFrequenciesSorted(monogram, 13);
	printFrequenciesSorted(digram, 11);
	printFrequenciesSorted(trigram, 9);
	
	return 0;
}
