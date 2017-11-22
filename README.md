# crypto_example_code
Crypto code from scratch for educational purposes

Each C code file is a complete, and independently compilable program with its own main().

euclidiv.c corresponds to what is discussed here: http://entrop-x.fr/index.php/fr/science/71-arithmetic-revisited-2 (french) or here: http://entrop-x.fr/index.php/en/science/55-arithmetic-revisited (english)

testeuclidiv.c is a test case that does some random integrity tests on the operations defined in euclidiv.c

power.c is the same as euclidiv.c, but with the power operation added to it. 
prime.c is the same as power.c, but with two primality tests added (Fermat and Rabin tests), and two functions that find the next prime after a given number.  They are discussed here: http://entrop-x.fr/index.php/en/science/75-big-powers-and-prime-numbers-arithmetic (english) or here: http://entrop-x.fr/index.php/fr/science/76-big-powers-and-prime-numbers-arithmetic-2 (french).

testprime.c is a consistency check that tests whether the "next prime after n" is the same for the Rabin and Fermat test.  

rsa.c is the same as prime.c, with the added function of generating an rsa keypair, and having an rsa encryption/decryption function.  These functions are discussed here: http://entrop-x.fr/index.php/fr/cryptography-and-crypto-currencies-fr/78-the-rsa-crypto-system-2 (in french) or here: http://entrop-x.fr/index.php/en/cryptography-and-crypto-currencies/77-the-rsa-crypto-system (in english).

rsatest.c is an integrity check of the encryption and decryption of the rsa system as presented in rsa.c

diffie.c is the same as rsa.c, but with the functions that set up a Diffie-Hellman key exchange tools: setting up a Diffie-Hellman environment, generating a key pair in a given environment, and finding the common secret in a D-H key exchange.  These functions are described here: http://entrop-x.fr/index.php/en/cryptography-and-crypto-currencies/79-diffie-hellman in english, and here: http://entrop-x.fr/index.php/fr/cryptography-and-crypto-currencies-fr/80-diffie-hellman-2 in french.

testdiffie.c is a consistency check on the D-H key exchange algorithms: it generates random environments and key pairs, and tests whether the common secret key is the same, as it should.

diffiepolrho.c is the same as diffie.c but adds the Pollard-Rho attack on the standard Diffie-Hellman key exchange.  This is discussed here: http://entrop-x.fr/index.php/fr/cryptography-and-crypto-currencies-fr/83-the-pollard-rho-attack-on-diffie-hellman-2 (in french) and here: http://entrop-x.fr/index.php/en/cryptography-and-crypto-currencies/81-the-pollard-rho-attack-on-diffie-hellman (in english).

diffiepolrho_64.c is a "64 bits version" of the code, where integers are of long long type.
