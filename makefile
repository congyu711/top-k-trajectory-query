
hilbert: hilbertcurve.cc
	g++ hilbertcurve.cc -o hilbertcurve -D __HILBERTCURVE_TEST__ -lgmpxx -lgmp

SHE: SHE_encrypt.cpp
	g++ SHE_encrypt.cpp -g  -lgmpxx -lgmp -o SHE_encrypt -D __SHE_encrypt__

generate: generate_key.cc
	g++ generate_key.cc -o generate_key -lcryptopp