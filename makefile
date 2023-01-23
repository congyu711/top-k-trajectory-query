CCC = g++
CRYPTOPP = -lcryptopp

hilbert: hilbertcurve.cc
	$(CCC) hilbertcurve.cc -o hilbertcurve -D __HILBERTCURVE_TEST__ $(CRYPTOPP)

SHE_Encryption: SHE_Encryption.cc
	$(CCC) SHE_Encryption.cc -g  $(CRYPTOPP) -o SHE_Encryption -D __SHE_encrypt__

Proxy_ReEncryption: Proxy_ReEncryption.cc
	$(CCC) Proxy_ReEncryption.cc -o Proxy_ReEncryption $(CRYPTOPP) -D __Proxy_ReEncryption_test__

extend: extend.cc
	$(CCC) extend.cc -o extend $(CRYPTOPP)

Preliminarie: Preliminarie.cc Douglas_Peucker.cc SHE_Encryption.cc hilbertcurve.cc
	$(CCC) Preliminarie.cc -o Preliminarie $(CRYPTOPP) -D __Preliminarie__test__

Cryption_Code: Cryption_Code.cc Preliminarie.cc Proxy_ReEncryption.cc
	$(CCC) Cryption_Code.cc -o Cryption_Code $(CRYPTOPP)

sort: sort.cc Cryption_Code.cc DSED.cc
	$(CCC) sort.cc -o sort $(CRYPTOPP) 

main: main.cc sort.cc
	$(CCC) main.cc -o main $(CRYPTOPP)

Preliminarie: Preliminarie.cc extend.cc SHE_Encryption.cc 
	$(CCC) Preliminarie.cc -o Preliminarie $(CRYPTOPP)