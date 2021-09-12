mkdir build
cd build
../configure --prefix=$RISCV --host=riscv64-unknown-elf --with-arch=rv64ima --with-payload=vmlinux
