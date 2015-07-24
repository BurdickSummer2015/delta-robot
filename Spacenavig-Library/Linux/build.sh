gcc -c -fPIC spacenavig_python.c -o spacenavig_python.o
gcc spacenavig_python.o -shared -o spacenavig_python.so
