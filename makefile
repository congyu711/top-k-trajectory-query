CCC = g++
GMP = -lgmpxx -lgmp
CRYPTOPP = -lcryptopp

hilbert: hilbertcurve.cc
	$(CCC) hilbertcurve.cc -o hilbertcurve -D __HILBERTCURVE_TEST__ $(GMP)

SHE: SHE_encrypt.cpp
	$(CCC) SHE_encrypt.cpp -g  $(GMP) -o SHE_encrypt -D __SHE_encrypt__

Proxy_ReEncryption: Proxy_ReEncryption.cc
	$(CCC) Proxy_ReEncryption.cc -o Proxy_ReEncryption $(CRYPTOPP) -D __Proxy_ReEncryption_test__

main: main.cc hilbertcurve.cc SHE_encrypt.cpp Proxy_ReEncryption.cc DSED.cc
	$(CCC) main.cc -o main $(GMP) $(CRYPTOPP)