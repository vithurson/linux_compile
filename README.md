# linux_compile

all the setup required for our riscv compile

./submodule_update

add these to the ~/.bashrc
export RISCV=/home/vithurson/buildroot-2022.02.3/output/host                                                                                
export PATH=$PATH:$RISCV/bin                                                   
export ARCH=riscv
export CROSS_COMPILE=riscv64-buildroot-linux-uclibc- 

generic
./apply_configs_and_patches

buildroot 
make -j16

linux
make -j16

riscv-pk
mkdir build
cd build
../configure --prefix=$RISCV --host=riscv64-buildroot-linux-uclibc --with-arch=rv64ima --with-payload=../../linux/vmlinux --enable-print-device-tree --with-dts=../../device_tree/sample.dts --with-abi=lp64 --with-mem-start=0x10000000

riscv-emu-c
make
./emu ../riscv-pk/build/bbl
