# linux_compile
all the setup required for our riscv compile

~/bashrc entires
export RISCV=/home/vithurson/buildroot-2022.02.3/output/host                                                                                
export PATH=$PATH:$RISCV/bin                                                   
export ARCH=riscv
export CROSS_COMPILE=riscv64-buildroot-linux-uclibc- 
