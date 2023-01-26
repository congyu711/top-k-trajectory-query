This repository contains code for an anonymous trajectory similarity query system.

# Files

- `hilbertcurve.cc`... construction of a family of hilbert curves.(actually this is used as a header file)
- `Douglas_Peucker.cc`... [Douglas_Peucker algorithm](https://en.wikipedia.org/wiki/Ramer%E2%80%93Douglas%E2%80%93Peucker_algorithm) (header)
- `SHE_Encryption.cc`... implement a symmetric homomorphic encryption (SHE) scheme which is leveled homomorphic(header)
- `DSED.cc`... compute the Discrete Synchronous Euclidean Distance(DSED) using the feature that the ciphertxt is leveled homomorphic and the approximate distance between two        points based on hilbertcurve(header)
- `Proxy_ReEncryption.cc` ... implement a Proxy Re-Encryption algorithm(header)
- `base.h`... some tool functions used by main.cc
