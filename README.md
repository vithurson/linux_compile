# linux_compile

clone submodules
----------------

./submodule_update


add these to the ~/.bashrc
--------------------------
export RISCV=/home/vithurson/buildroot-2022.02.3/output/host                                                                                
export PATH=$PATH:$RISCV/bin                                                   
export ARCH=riscv
export CROSS_COMPILE=riscv64-buildroot-linux-uclibc- 

generic

./apply_configs_and_patches

buildroot 
---------
make -j16

linux
-----
make -j16

riscv-pk
-------
mkdir build

cd build

../../config_build_bbl
riscv-emu-c
-----------
make

./emu ../riscv-pk/build/bbl
