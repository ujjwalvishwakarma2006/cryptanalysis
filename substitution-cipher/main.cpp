#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>

class SubstitutionCipher {
public:
	// Maps to store key and inverse key. 
	std::unordered_map<char, char> key;
	std::unordered_map<char, char> invKey;

	SubstitutionCipher() {
		// Initialize all keys to `-`.
		for (char ch = 'A'; ch <= 'Z'; ++ch) {
			invKey[ch] = '-';
		}
	}

	// Function to add keys in the map in increamental order. 
	// One can add the key into the map as soon as he becomes sure about it. 
	// `p`: plaintext character
	// `c`: ciphertext character which the cryptanalyst thinks `p` maps to
	void addKey(char p, char c) {
		if (p < 'a' || p > 'z') {
			std::cout << "Plaintext should consist of only lowercase english letters.\n";
			return;
		}

		if (c < 'A' || c > 'Z') {
			std::cout << "Ciphertext should consist of only uppercase english letters.\n";
			return;
		}

		key[p] = c;
		invKey[c] = p;
	}

	std::string decrypt(std::string ciphertext) {
		std::string decrypted;
		decrypted.reserve(ciphertext.length());

		for (char ch : ciphertext) {
			if (key[ch] == '-') {
				std::cout << "Cannot decrypt character: " << ch << ". Please provide a key first.\n";
				return "";
			}
			decrypted.push_back(invKey[ch]);
		}

		return decrypted;
	}

	std::string encrypt(std::string plaintext) {
		std::string encrypted;
		encrypted.reserve(plaintext.length());

		for (char ch : plaintext) {
			if (key[ch] == '-') {
				std::cout << "Cannot encrypt character: " << ch << ". Please provide a key first.\n";
				return "";
			}
			encrypted.push_back(key[ch]);
		}

		return encrypted;
	}

	void decryptAndPrint(std::string ciphertext, int cols = 100) {
		std::string decrypted = decrypt(ciphertext);
		if (decrypted == "") return;

		int ind = 0;
		while (ind < ciphertext.length() - cols) {
			std::cout << ciphertext.substr(ind, cols) << "\n";
			std::cout << decrypted.substr(ind, cols) << "\n";
			ind += cols;
		}

		std::cout << ciphertext.substr(ind, std::min(cols, int(ciphertext.length()) - ind)) << "\n";
		std::cout << decrypted.substr(ind, std::min(cols, int(ciphertext.length()) - ind)) << "\n";
		std::cout << "\n";
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
		std::cout << k << ": " << v << "\t";
		if (printedCols-- == 0) {
			printedCols = cols;
			std::cout << "\n";
		}
	}

	std::cout << "\n\n";
}

int main() {
	std::string ciphertext = "RABXDPSTJXQSFPPFQEJVSXPGSMCMPSLPGSFPPFQESXJXFWVSXMFXCPXRSMFIIHJMMRBISESCMDAPRIRPTRAWMPGSQJXXSQPESCPGSFPPFQESXMSISQPMPGSESCPJPXCXFAWJLICFMMDLRANPGFPPGSFPPFQESXQFAWRMPRANDRMGPGSQJXXSQPFAWPGSRAQJXXSQPESCFTPSXPXRFIMJASPGRANRYJDIWIRESPJLSAPRJARMPGFPPXCRANHFMMYJXWMJTMJLSJASMFQQJDAPRMAJPFBXDPSTJXQSFPPFQEPGFPRMUDMPGRPPXRFIRAPGSBXDPSTJXQSFPPFQEYSTJQDMJAPGSESCNSASXFPSWTJXPGSSAQXCHPRJAFINJXRPGL";

	SubstitutionCipher sc;
	std::unordered_map<std::string, int> monogram, digram, trigram;

	// Populate monogram, digram, and trigram frequencies for frequency analysis.
	for (int i = 0; i < ciphertext.length(); ++i) monogram[ciphertext.substr(i, 1)]++;
	for (int i = 0; i < ciphertext.length() - 1; ++i) digram[ciphertext.substr(i, 2)]++;
	for (int i = 0; i < ciphertext.length() - 2; ++i) trigram[ciphertext.substr(i, 3)]++;

	printFrequenciesSorted(monogram, 13);
	printFrequenciesSorted(digram, 11);
	printFrequenciesSorted(trigram, 9);
	
	// Performing Frequency Analysis based on statistical data of the English Language.
	// Also, seeing the incremental changes after deciphering. 
	sc.addDecryptAndPrint('t', 'P', ciphertext);
	sc.addDecryptAndPrint('e', 'S', ciphertext);
	sc.addDecryptAndPrint('h', 'G', ciphertext);
	sc.addDecryptAndPrint('a', 'F', ciphertext);
	sc.addDecryptAndPrint('c', 'Q', ciphertext);
	sc.addDecryptAndPrint('k', 'E', ciphertext);
	sc.addDecryptAndPrint('r', 'X', ciphertext);
	sc.addDecryptAndPrint('o', 'J', ciphertext);
	sc.addDecryptAndPrint('b', 'B', ciphertext);
	sc.addDecryptAndPrint('u', 'D', ciphertext);
	sc.addDecryptAndPrint('i', 'R', ciphertext);
	sc.addDecryptAndPrint('n', 'A', ciphertext);
	sc.addDecryptAndPrint('f', 'T', ciphertext);
	sc.addDecryptAndPrint('d', 'W', ciphertext);
	sc.addDecryptAndPrint('v', 'V', ciphertext);
	sc.addDecryptAndPrint('s', 'M', ciphertext);
	sc.addDecryptAndPrint('y', 'C', ciphertext);
	sc.addDecryptAndPrint('m', 'L', ciphertext);
	sc.addDecryptAndPrint('l', 'I', ciphertext);
	sc.addDecryptAndPrint('p', 'H', ciphertext);
	sc.addDecryptAndPrint('g', 'N', ciphertext);
	sc.addDecryptAndPrint('w', 'Y', ciphertext);
	sc.addDecryptAndPrint('j', 'U', ciphertext);

	return 0;
}
