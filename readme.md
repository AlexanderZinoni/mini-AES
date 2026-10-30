# Mini-AES – C Implementation

## Overview

This project implements a simplified version of the AES encryption algorithm (Mini-AES) in C, as part of TD4 – Algorithmique et programmation C (Université Paris-Saclay).

The implementation includes:
- Finite field arithmetic over GF(2⁴).
- Logarithm and exponentiation lookup tables.
- Finite field multiplication and inversion.
- Mini-AES encryption and decryption.
- ASCII-to-hexadecimal message conversion.

## Mini-AES

The algorithm operates on 16-bit blocks, represented by four 4-bit elements, over the finite field:

GF(2⁴) = GF(2)[x]/(x⁴ + x + 1).

Encryption consists of an initial `AddRoundKey`, followed by 16 rounds involving:

1. `SubBytes`: Nonlinear substitution using the inverse in GF(2⁴) and an affine transformation.
2. `ShiftRows`: Permutation of state elements.
3. `MixColumns`: Linear transformation over GF(2⁴).
4. `AddRoundKey`: XOR with the current round key.

The round keys are generated from a master key using a key schedule. The master key is 16 bits long.

## Compilation & execution example

Compile the project with `make`, then run the executable `main`. 
By default, the program encrypts the string `"Very secret message. "` under the hex key `130E`, and displays the following: 
`
Original:  Very secret message. 
Encrypted: F1E71C2DC93FE726B6223F628AD5DC256061465E7CF8
Decrypted: Very secret message. 
`
