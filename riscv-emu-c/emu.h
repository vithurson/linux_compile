#ifndef _EMU_H_
#define _EMU_H_

#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <bits/stdc++.h>
#include <math.h>
#include <chrono>
#include <thread>
#include <algorithm> 
#include <map>
#include <curses.h>
#include <memory.h>
#include <stdexcept>
#include <sys/time.h>

#ifdef TEMU

extern "C" {
#include "temu/temu.h"
#include "temu/cutils.h"
#include "temu/iomem.h"
#include "temu/virtio.h"
#include "temu/machine.h"
#ifdef CONFIG_FS_NET
#include "temu/fs_utils.h"
#include "temu/fs_wget.h"
#endif
#ifdef CONFIG_SLIRP
#include "temu/slirp/libslirp.h"
#endif
}

#endif

#include <stdarg.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#ifndef _WIN32
#include <termios.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_tun.h>
#endif
#include <sys/stat.h>
#include <signal.h>

#include <queue>

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
using namespace std;






int emu_main();
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
using namespace std;



typedef uint64_t uint_t;
typedef uint64_t data_t;

#define MEM_SIZE 28
#define XLEN     64
#define FIFO_ADDR_RX 0xe000102c
#define FIFO_ADDR_TX 0xe0001030
// #define FIFO_ADDR_RX 0xe000102c
// #define FIFO_ADDR_TX 0xe0001030

#define DRAM_BASE 0x80000000

#define DRAM_SIZE (((1llu)<<(MEM_SIZE))*8)

#define MASK64 0xFFFFFFFFFFFFFFFFllu
#define MASK32 0xFFFFFFFFllu

enum plevel_t {
    MMODE = 0b11,
    HMODE = 0b10,
    SMODE = 0b01,
    UMODE = 0b00
};

vector<uint_t> memory(1<<MEM_SIZE); // main memory

//uint8_t memory[DRAM_SIZE];

vector<uint_t> reg_file(32);       // register file

plevel_t cp     = (plevel_t)MMODE;

bool csr_read_success = false;



#define CLINT_BASE 0x2000000
#define CLINT_SIZE 0x00c0000
#define PLIC_BASE 0x40100000
#define PLIC_SIZE 0x400000
#define VIRTIO_BASE 0x40010000
#define VIRTIO_SIZE 0x1000

#define MTIME_ADDR (CLINT_BASE+0xbff8)
#define MTIMECMP_ADDR (CLINT_BASE+0x4000)

uint_t &mtime = memory.at(MTIME_ADDR/8);
uint_t &mtimecmp = memory.at(MTIMECMP_ADDR/8);


#define CAUSE_MISALIGNED_FETCH 0x0
#define CAUSE_FETCH_ACCESS 0x1
#define CAUSE_ILLEGAL_INSTRUCTION 0x2
#define CAUSE_BREAKPOINT 0x3
#define CAUSE_MISALIGNED_LOAD 0x4
#define CAUSE_LOAD_ACCESS 0x5
#define CAUSE_MISALIGNED_STORE 0x6
#define CAUSE_STORE_ACCESS 0x7
#define CAUSE_USER_ECALL 0x8
#define CAUSE_SUPERVISOR_ECALL 0x9
#define CAUSE_HYPERVISOR_ECALL 0xa
#define CAUSE_MACHINE_ECALL 0xb
#define CAUSE_FETCH_PAGE_FAULT 0xc
#define CAUSE_LOAD_PAGE_FAULT 0xd
#define CAUSE_STORE_PAGE_FAULT 0xf

#define CAUSE_USER_SOFT_INT 0x0
#define CAUSE_SUPERVISOR_SOFT_INT 0x1

#define CAUSE_MACHINE_SOFT_INT 0x3
#define CAUSE_USER_TIMER_INT 0x4
#define CAUSE_SUPERVISOR_TIMER_INT 0x5

#define CAUSE_MACHINE_TIMER_INT 0x7
#define CAUSE_USER_EXT_INT 0x8
#define CAUSE_SUPERVISOR_EXT_INT 0x9

#define CAUSE_MACHINE_EXT_INT 0xb



bool INS_ADDR_MISSALIG   = false; //instruction address miss
bool INS_ACC_FAULT       = false; //instruction access fault
bool ILL_INS             = false; //illegal instruction
bool EBREAK              = false; //break point
bool LD_ADDR_MISSALIG    = false; //load address misalignmen
bool LD_ACC_FAULT        = false; //load access fault
bool STORE_ADDR_MISSALIG = false; //store/amo address misali
bool STORE_ACC_FAULT     = false; //store/amo access fault
bool INS_PAGE_FAULT      = false; //instructioin page fault
bool LD_PAGE_FAULT       = false; //load page fault
bool STORE_PAGE_FAULT    = false; //store/amo page fault
bool ECALL               = false;





// type defs
enum opcode_t {
                lui    = 0b0110111,
                auipc  = 0b0010111,
                jump   = 0b1101111,
                jumpr  = 0b1100111,
                cjump  = 0b1100011,
                load   = 0b0000011,
                store  = 0b0100011,
                iops   = 0b0010011,
                iops64 = 0b0011011,
                rops   = 0b0110011,
                rops64 = 0b0111011,
                amo    = 0b0101111,
                fence  = 0b0001111,
                systm  = 0b1110011,
                fd1    = 0b0000111,
                fd2    = 0b0100111,
                fd3    = 0b1000011,
                fd4    = 0b1000111,
                fd5    = 0b1001011,
                fd6    = 0b1001111,
                fd7    = 0b1010011
            };




bool store_word(uint_t store_addr, uint_t load_data, uint_t value, uint_t &wb_data){
    switch(store_addr%8){
        case 0 :
            wb_data = ( load_data & 0xFFFFFFFF00000000) + ((value & 0xFFFFFFFF) << 0 );
            break;
        case 1 :
            wb_data = ( load_data & 0xFFFFFF00000000FF) + ((value & 0xFFFFFFFF) << 8 );
            break;
        case 2 :
            wb_data = ( load_data & 0xFFFF00000000FFFF) + ((value & 0xFFFFFFFF) << 16);
            break;
        case 3 :
            wb_data = ( load_data & 0xFF00000000FFFFFF) + ((value & 0xFFFFFFFF) << 24);
            break;
        case 4 :
            wb_data = ( load_data & 0x00000000FFFFFFFF) + ((value & 0xFFFFFFFF) << 32);
            break;
        default :
            //cout<<"######## Mis-aligned memory store #########\n";
            wb_data = -1;
            return false;
            break;
    }
    return true;
}



bool store_halfw(uint_t store_addr, uint_t load_data, uint_t value, uint_t &wb_data){
    switch(store_addr%8){
        case 0 :
            wb_data = ( load_data & 0xFFFFFFFFFFFF0000) + ((value & 0xFFFF) << 0 );
            break;
        case 1 :
            wb_data = ( load_data & 0xFFFFFFFFFF0000FF) + ((value & 0xFFFF) << 8 );
            break;
        case 2 :
            wb_data = ( load_data & 0xFFFFFFFF0000FFFF) + ((value & 0xFFFF) << 16);
            break;
        case 3 :
            wb_data = ( load_data & 0xFFFFFF0000FFFFFF) + ((value & 0xFFFF) << 24);
            break;
        case 4 :
            wb_data = ( load_data & 0xFFFF0000FFFFFFFF) + ((value & 0xFFFF) << 32);
            break;
        case 5 :
            wb_data = ( load_data & 0xFF0000FFFFFFFFFF) + ((value & 0xFFFF) << 40);
            break;
        case 6 :
            wb_data = ( load_data & 0x0000FFFFFFFFFFFF) + ((value & 0xFFFF) << 48);
            break;
        default :
            //cout<<"######## Mis-aligned memory store #########\n";
            wb_data = -1;
            return false;
            break;
    }
    return true;
}

bool store_byte(uint_t store_addr, uint_t load_data, uint_t value, uint_t &wb_data){
    switch(store_addr%8){
        case 0 :
            wb_data = ( load_data & ((0xFFFFFFFFFFFFFFull)<<8)) + (value & 0xFF);
            break;
        case 1 :
            wb_data = ( load_data & ((0xFFFFFFFFFFFFull)<<16)) + ((value & 0xFF)<<8) + (load_data & 0xFF);
            break;
        case 2 :
            wb_data = ( load_data & ((0xFFFFFFFFFFull)<<24)) + ((value & 0xFF)<<16) + (load_data & 0xFFFF);
            break;
        case 3 :
            wb_data = ( load_data & ((0xFFFFFFFFull)<<32)) + ((value & 0xFF)<<24) + (load_data & 0xFFFFFF);
            break;
        case 4 :
            wb_data = ( load_data & ((0xFFFFFFull)<<40)) + ((value & 0xFF)<<32) + (load_data & 0xFFFFFFFF);
            break;
        case 5 :
            wb_data = ( load_data & ((0xFFFFull)<<48)) + ((value & 0xFF)<<40) + (load_data & 0xFFFFFFFFFF);
            break;
        case 6 :
            wb_data = ( load_data & ((0xFFull)<<56)) + ((value & 0xFF)<<48) + (load_data & 0xFFFFFFFFFFFF);
            break;
        case 7 :
            wb_data = ((value & 0xFF)<<56) + (load_data & 0xFFFFFFFFFFFFFF);
            break;
        default :
            //cout<<"######## Mis-aligned memory store #########\n";
            wb_data = -1;
            return false;
            break;
    }
    return true;
}


bool load_word(uint_t load_addr, uint_t load_data, uint_t &wb_data){
    switch(load_addr%8){
        case 0 :
            wb_data = ((load_data >> 0 ) & 0xFFFFFFFF);
            break;
        case 1 :
            wb_data = ((load_data >> 8 ) & 0xFFFFFFFF);
            break;
        case 2 :
            wb_data = ((load_data >> 16) & 0xFFFFFFFF);
            break;
        case 3 :
            wb_data = ((load_data >> 24) & 0xFFFFFFFF);
            break;
        case 4 :
            wb_data = ((load_data >> 32) & 0xFFFFFFFF);
            break;
        default :
            wb_data = -1;
            return false;
            break;   
    }
    return true;
}

bool load_halfw(uint_t load_addr, uint_t load_data, uint_t &wb_data){
    switch(load_addr%8){
        case 0 :
            wb_data = ((load_data >> 0 ) & 0xFFFF) ;
            break;
        case 1 :
            wb_data = ((load_data >> 8 ) & 0xFFFF) ;
            break;
        case 2 :
            wb_data = ((load_data >> 16) & 0xFFFF) ;
            break;
        case 3 :
            wb_data = ((load_data >> 24) & 0xFFFF) ;
            break;
        case 4 :
            wb_data = ((load_data >> 32) & 0xFFFF) ;
            break;
        case 5 :
            wb_data = ((load_data >> 40) & 0xFFFF) ;
            break;
        case 6 :
            wb_data = ((load_data >> 48) & 0xFFFF) ;
            break;
        default :
            //cout<<"######## Mis-aligned memory load #########\n";
            wb_data = -1;
            return false;
            break;
    }
    return true;
}


bool load_byte(uint_t load_addr, uint_t load_data, uint_t &wb_data){
    switch(load_addr%8){
        case 0 :
            wb_data = (load_data & 0xFF) ;
            break;
        case 1 :
            wb_data = (((load_data)>>8) & 0xFF) ;
            break;
        case 2 :
            wb_data = (((load_data)>>16) & 0xFF) ;
            break;
        case 3 :
            wb_data = (((load_data)>>24) & 0xFF) ;
            break;
        case 4 :
            wb_data = (((load_data)>>32) & 0xFF) ;
            break;
        case 5 :
            wb_data = (((load_data)>>40) & 0xFF) ;
            break;
        case 6 :
            wb_data = (((load_data)>>48) & 0xFF) ;
            break;
        case 7 :
            wb_data = (((load_data)>>56) & 0xFF) ;
            break;
        default :
            //cout<<"######## Mis-aligned memory load #########\n";
            wb_data = -1;
            return false;
            break;
    }
    return true;
}

std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

bool plic_served_irq = false; 
bool plic_pending_irq = false;

//#define DEBUG

template<class T>
T sign_extend(T x, const int bits) {
    T m = 1;
    m <<= bits - 1;
    return (x ^ m) - m;
}

template<class T>
T divi(T num1, T num2,int s) {
    if (num2==0){
        switch(s){
            case(0)  : return (T)(-1); break;
            case(1) : return (T)MASK64; break;
            case(2)  : return num1; break;
            case(3) : return num1; break;
        }
    } else if(num1==(-pow(2ull,63)) && num2==-1){
        if (s==0 || s==2){
            switch(s){
                case(0)  : return -pow(2ull,63); break;
                case(2)  : return 0; break;
            }
        }
    } else {
        ldiv_t div_result;
        switch(s){
            case(0)  :
                div_result = div((int64_t)num1,(int64_t)num2);
                return div_result.quot;
                break;
            case(1) :
                return num1/num2;
                break;
            case(2)  :
                div_result = div((int64_t)num1,(int64_t)num2);
                return div_result.rem;
                break;
            case(3) :
                return num1%num2;
                break;
        }
    }
}

template<class T>
T divi32(T num1, T num2,int s) {
    if (num2==0){
        switch(s){
            case(0)  : return (T)(-1); break;
            case(1) : return (T)MASK32; break;
            case(2)  : return num1; break;
            case(3) : return num1; break;
        }
    } else if(num1==(-pow(2,31)) && num2==-1){
        if (s==0 || s==2){
            switch(s){
                case(0)  : return -pow(2ull,31); break;
                case(2)  : return 0; break;
            }
        }
    } else {
        div_t div_result;
        switch(s){
            case(0)  :
                div_result = div((int32_t)num1,(int32_t)num2);
                return div_result.quot;
                break;
            case(1) :
                return num1/num2;
                break;
            case(2)  :
                div_result = div((int32_t)num1,(int32_t)num2);
                return div_result.rem;
                break;
            case(3) :
                return num1%num2;
                break;
        }
    }
}

int64_t signed_value(uint_t x){
  if (((x>>63) & 0b1) == 1)
      return (x ^ (1llu<<63)) - (1llu<<63);
  else
      return x;
}

int32_t signed_value32(uint_t x){
    uint32_t y = x & MASK32;
  if (((y>>31) & 0b1) == 1)
      return (y ^ (1lu<<31)) - (1lu<<31);
  else
      return y;
}

string reg_file_names [] = {"zero","ra","sp","gp","tp","t0","t1","t2","s0","s1","a0","a1","a2","a3","a4","a5","a6",\
                              "a7","s2","s3","s4","s5","s6","s7","s8","s9","s10","s11","t3","t4","t5","t6"};

void print_reg_file(vector<uint_t> reg_file){
    
    for (int i=0;i<32;i++){
        printf("%s : %lu\n",reg_file_names[i].c_str(),reg_file[i]);
    }
}

uint_t getINST(uint_t PC,vector<uint_t> * memory){
    if(PC%2==0)
        return ((MASK32) & (memory->at(PC/2)));
    else
        return ((memory->at(PC/2))>>32);
}


#endif
