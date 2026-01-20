# Cryptanalysis Laboratory

This repository contains code I wrote during laboratory sessions of my cryptography course in sixth semester at university to break some ciphers (for example, Affine Cipher and Substitution Cipher). 
Of course, it won't happen automatically and magically - a little human intervention is required. 
You have to monitor the output at each step.

> **Note:** The code is written for the sole purpose of understanding, and there are various scopes for improvement. I'm trying to improve the performance with each commit.

> **Why so much effort for such a simple project?** This project is intentionally over-engineered as practice for improving my design and documentation skills.

## Structure

The repository is organized into cipher-specific directories:
- `affine-cipher/` - Implementation and breaking of Affine Cipher
- `substitution-cipher/` - Implementation and breaking of Substitution Cipher

## 1. Affine Cipher

[This file](./affine-cipher/main.cpp) implements the following core functionalities:

- **`encrypt`** - Encrypts a plaintext using a key
- **`decrypt`** - Decrypts a ciphertext using a key

The code also implements a `ModularArithmetic` utility class to perform all operations in Group $\mathbb{Z}_{26}$. It has basic functions like **`add`**, **`subtract`**, and **`multiply`**, as well as **`findModularInverse`** to find multiplicative inverse of a number in the Group. 

The `AffineCryptanalysis` class is at the highest abstraction level. 
- **`solveAffineParameters`**: Tries to deduce the key based on a given `KnownPlaintextPair`
- **`frequencyAttack`**: Performs [Known-plaintext attack](https://en.wikipedia.org/wiki/Known-plaintext_attack) over a given ciphertext


## 2. Substitution Cipher

[This file](./substitution-cipher/main.cpp) implements the `SubstitutionCipher` class. There wasn't much to automate here, and the cryptanalyst is requested to use their own brain. It implements the following:

- **`addKey`** - Adds keys to the permutation on an incremental basis
- **`encrypt`** - Encrypts plaintext based on the key (won't work if the key isn't set for a character in the plaintext)
- **`decrypt`** - Decrypts the ciphertext (will decrypt as `-` if the key isn't present for a character)

## Usage

Each cipher directory contains a `main.cpp` file. Compile and run the code to interact with the cipher implementations.

```bash
# Example for Affine Cipher
cd affine-cipher
g++ main.cpp -o affine
./affine
```

## Contribute

If you want to improve the code so I can improve my coding style or enhance performance, just make a pull request (obviously in a forked repo). I have no specific guidelines as of now. Feel free to criticize the code.

I don't think anyone will ever use this code, but if you do, you can use it in any way. I don't have much understanding of licenses as of now. If you manage to make some money using my code, I don't have any problem with it, nor do I want a share. (:
