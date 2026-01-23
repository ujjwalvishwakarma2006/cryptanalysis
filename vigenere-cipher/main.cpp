// This file implements Kasiski's test to decipher a given ciphertext that was enciphered using vigenere cipher.
// Step 1: Try to find the keyLength(m)
// Step 2: Try to verify using Index of Coincidence.
// Step 3: Try to find out each character of the key by observingi the Mg values of each bin

#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <print>			// Using C++ 23 (:
#include <optional>
#include <numeric>			// for std::gcd
#include <fstream>			// to process probability file

// Utility function to print frequencies of most frequent {mono,bi,tri,four}grams.
void printFrequenciesSorted(std::unordered_map<std::string, int> freqMap, int cols, std::string label, int top = 15) {
	std::println(" * {} Frequencies", label);
	std::print("\t");
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
			std::print("\t");
		}
		if (--top == 0) break;
	}

	std::println();
	std::println();
}

class VigenereCipher {
	std::optional<std::vector<double>> prob;		// expected probability of each character
	std::optional<std::string> key, freqFile = "frequencies.txt";

	bool addProb(std::string chAndProb, std::vector<double>& tempProb) {
		char ch = chAndProb[0];
		if (ch < 'a' || ch > 'z') {
			std::println(stderr, "Error: {} should consist only lowercase english alphabets.", *freqFile);
			return false;
		}
		double chProb = std::stod(chAndProb.substr(2, chAndProb.length() - 2));
		tempProb[ch - 'a'] = chProb;
		return true;
	}

public:
	VigenereCipher() {
		std::ifstream inFile(*freqFile);
		std::string chAndProb;
		std::vector<double> tempProb(26, 0);
		while (getline(inFile, chAndProb)) {
			if (!addProb(chAndProb, tempProb)) {
				std::println("Warning: Object initialized, but {} is not in the required format.", *freqFile);
				return;
			}
		}
		
		prob = tempProb;
		// double ioc = 0;
		// for (auto p : *prob) ioc += p*p;
		// std::println("{}", ioc);
	}

	void setKey(std::string key) {
		this->key = key;
		std::println("Key set to: {}", *(this->key));
		std::println();
	}

	std::optional<std::string> decrypt(std::string ciphertext) {
		if (!key) {
			std::println(stderr, "Error: Unable to decrypt, please set the key first.");
			return std::nullopt;
		}

		std::string decrypted;
		decrypted.reserve(ciphertext.length());
		int keyLength = (*key).length();
		for (int i = 0; i < ciphertext.length(); ++i) {
			int cti = ciphertext[i] - 'a';
			int ki = (*key)[i % keyLength] - 'a';
			char pi = 'a' + (cti - ki + 26) % 26;
			decrypted.push_back(pi);
		}

		return decrypted;
	}

	std::optional<std::string> encrypt(std::string plaintext) {
		if (!key) {
			std::println(stderr, "Error: Unable to encrypt, please set the key first.");
			return std::nullopt;
		}

		std::string encrypted;
		encrypted.reserve(plaintext.length());
		int keyLength = (*key).length();
		for (int i = 0; i < plaintext.length(); ++i) {
			int pi = plaintext[i] - 'a';
			int ki = (*key)[i % keyLength] - 'a';
			char cti = 'a' + (pi + ki) % 26;
			encrypted.push_back(cti);
		}

		return encrypted;
	}

	static std::optional<std::vector<int>> getDeltas(
		std::string&				ciphertext, // ciphertext to search in
		std::vector<std::string>	phrases		// phrases to search in the ciphertext
	) {
		std::vector<int> deltas;
		deltas.reserve(phrases.size());

		for (auto phrase : phrases) {
			auto firstOccurrence = ciphertext.find(phrase, 0);
			if (firstOccurrence == std::string::npos) {
				std::println(stderr, "Error: target phrase {} not found in ciphertext", phrase);
				return std::nullopt;
			}
			auto secondOccurrence = ciphertext.find(phrase, firstOccurrence + 1);
			if (secondOccurrence == std::string::npos) {
				std::println(stderr, "Error: target phrase {} not found in ciphertext", phrase);
				return std::nullopt;
			}
			deltas.push_back(secondOccurrence - firstOccurrence);
		}

		return deltas;
	}
	
	static std::optional<std::vector<int>> printAndGetDeltas(
		std::string&				ciphertext, // ciphertext to search in
		std::vector<std::string>	phrases,	// phrases to search in the ciphertext
		int							cols = 7	// number of columns in one chAndProb
	) {
		auto deltas = getDeltas(ciphertext, phrases);
		if (!deltas) return std::nullopt;

		std::println(" * Deltas for repeated phrases");
		std::print("\t");
		int printed = cols;
		for (int i = 0; i < phrases.size(); ++i) {
			std::print("{}: {}\t", phrases[i], (*deltas)[i]);
			if (--printed == 0) {
				std::println();
				printed = cols;
				std::print("\t");
			}
		}
		std::println();
		std::println();
		return deltas;
	}

	static std::optional<int> deduceKeyLength(std::vector<int> deltas, int top) {
		if (deltas.size() == 0) return std::nullopt;
		int m = deltas[0];
		for (int i = 1; i < std::min(static_cast<int>(deltas.size()), top); ++i) {
			m = std::gcd(m, deltas[i]); 
		}
		return m;
	}
		
	void calculateMgs(std::vector<int> bin, int binNumber, int cols = 9) {
		// This function calculates and prints difference values of Mg's for a given bin
		// By observing those values of those Mg's, we can find the character of the key
		// corresponding to the provided bin. 
		if (bin.size() != 26) {
			std::println(stderr, "Error: bin size must be equal to 26");
			return;
		}
		if (!prob) {
			std::println(stderr, "Error: expected frequencies not set.");
			return;
		}
		
		std::println("Mg values for bin {}", binNumber);
		int totalChars = std::accumulate(bin.begin(), bin.end(), 0);
		// Looping over possible values of binNumber'th character of the key
		// And for each, calculating Mg's
		int printed = cols;
		for (int i = 0; i < 26; ++i) {
			char ch = 'a' + i;
			double mg = 0;
			for (int j = 0; j < 26; ++j) {
				mg += (*prob)[j] * bin[(j + i) % 26];
			}
			mg = mg / totalChars;
			std::print("{}: {:.3f}\t", ch, mg);
			if (--printed == 0) {
				printed = cols;
				std::println();
			} 
		}

		std::println();
		std::println();
	}
	
	std::vector<double> findKey(std::string ciphertext, int keyLength) {
		std::vector<std::vector<int>> binFreq(keyLength, std::vector<int>(26, 0));
		// binFreq[i][j] denotes the frequency of j'th character in i'th bin
		std::vector<double> iocs(keyLength);
		int ctLength = ciphertext.size();
		for (int i = 0; i < std::min(keyLength, ctLength); ++i) {
			for (int k = i; k < ctLength - keyLength; k += keyLength) {
				binFreq[i][ciphertext[k] - 'a'] += 1;
			}
		}

		for (int i = 0; i < keyLength; ++i) {
			int totalBinChars = std::accumulate(binFreq[i].begin(), binFreq[i].end(), 0);
			iocs[i] = 0;
			for (int j = 0; j < 26; ++j) {
				iocs[i] += binFreq[i][j] * binFreq[i][j];
			}
			iocs[i] /= totalBinChars * totalBinChars;
			calculateMgs(binFreq[i], i);
		}

		return iocs;
	}

};

int main() {
	std::println("============================== VIGENERE CIPHER DECRYPTER ==============================");
	std::println();
	std::string ciphertext = "qwgbnnkywgbonsaqcjkbjbrorhjhnonzglxmlmmnxsqvrbochmqrxycyaqrfjbucxdkprqxrqaaaqzghpkojqqobnluuydawbixrvjwwozhvbnbubdqxpnufkdoadcorlmwcynodxhbewqntjjiqwgbnnkyyhopdqxpzzdrdqhujyxcbdsfxuunonzglxmlmppqqfsqlyniewqxjbqowhljbyzszowubqorryqqevdfwwtyrmxzlbmllqkkumxslxjxzfgxewiexfdabjuqfqdjjfkdvyjdefziajdpdqbidstizppnhfkzkacxqudri";

	std::unordered_map<std::string, int> monogram, digram, trigram, fourgram;

	// Populate trigram and fourgram frequencies for Kasiski's Test.
	for (int i = 0; i < ciphertext.length() - 2; ++i) trigram[ciphertext.substr(i, 3)]++;
	for (int i = 0; i < ciphertext.length() - 3; ++i) fourgram[ciphertext.substr(i, 4)]++;

	printFrequenciesSorted(trigram, 11, "Trigram");
	printFrequenciesSorted(fourgram, 10, "Fourgram");
	// We get the following trigrams.
	// wgb: 3	zgl: 2	xml: 2	qxp: 2	qwg: 2	pdq: 2	onz: 2	nzg: 2	non: 2	nnk: 2	nky: 2	mlm: 2
	// We get the following fourgrams.
	// zglx: 2	xmlm: 2	wgbn: 2	qwgb: 2	onzg: 2	nzgl: 2	nonz: 2	nnky: 2	lxml: 2	glxm: 2	gbnn: 2
	std::vector<std::string> repeatedTrigrams = {
		"wgb", "zgl", "xml", "qxp", "qwg", "pdq", "onz", "nzg", "non", "nnk", "nky", "mlm"
	};
	std::vector<std::string> repeatedFourgrams = {
		"zglx", "xmlm", "wgbn", "qwgb", "onzg", "nzgl", "nonz", "nnky", "lxml", "glxm", "gbnn"
	};

	// Since fourgrams are more reliable, we use them. 
	// If there were no fourgrams, we had to intelligently guess which trigrams need to be used
	// to deduce the keyLength using gcd.

	VigenereCipher vc;
	// vc.printDeltas(ciphertext, repeatedTrigrams, 7);
	// auto deltas3 = vc.getDeltas(ciphertext, repeatedTrigrams);
	// auto m3 = vc.deduceKeyLength(*deltas3, 5);
	auto deltas4 = vc.printAndGetDeltas(ciphertext, repeatedFourgrams);
	auto m4 = vc.deduceKeyLength(*deltas4, 5);
	std::println("Expected Key Length found: {}", *m4);

	vc.findKey(ciphertext, 7);
	vc.setKey("mdzxwjq");
	auto plaintext = vc.decrypt(ciphertext);
	if (plaintext) {
		std::println("Decrypted text: ");
		std::println("{}", *plaintext);
	}
	return 0;
}
