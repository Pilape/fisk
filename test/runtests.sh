clang -o testbin *.c lib/CuTest.c -Wall -g

./testbin

rm testbin
