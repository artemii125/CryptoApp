all: librc4.so libelgamal.so libpermutation.so main

librc4.so: lib/rc4.cpp lib/utils.cpp
	g++ -Wall -fPIC -std=c++17 -Iinclude -shared -o librc4.so lib/rc4.cpp lib/utils.cpp

libelgamal.so: lib/elgamal.cpp lib/utils.cpp
	g++ -Wall -fPIC -std=c++17 -Iinclude -shared -o libelgamal.so lib/elgamal.cpp lib/utils.cpp -ldl -lgmp -lgmpxx

libpermutation.so: lib/permutation.cpp lib/utils.cpp
	g++ -Wall -fPIC -std=c++17 -Iinclude -shared -o libpermutation.so lib/permutation.cpp lib/utils.cpp

main: src/main.cpp src/rc4_menu.cpp src/elgamal_menu.cpp src/permutation_menu.cpp lib/utils.cpp
	g++ -Wall -std=c++17 -Iinclude -o CryptoApp src/main.cpp src/rc4_menu.cpp src/elgamal_menu.cpp src/permutation_menu.cpp lib/utils.cpp -ldl -lgmp -lgmpxx

clean:
	rm -f *.so main
