#include "emu.h"

#define MCAUSE			0x342 	
#define MTVAL			0x343 	
#define MSCRATCH		0x340 	
#define MEPC			0x341 	
#define MIP				0x344 
#define TDATA3			0x7A3 	
#define SIE				0x104 
#define HPMCOUNTER20	0xC14 	
#define HPMCOUNTER23	0xC17 	
#define UEPC			0x041 	
#define USCRATCH		0x040 	
#define UTVAL			0x043 	
#define UCAUSE			0x042 	
#define UIP				0x044 
#define SSTATUS			0x100 
#define SEDELEG			0x102 
#define SIDELEG			0x103 
#define PMPCFG1			0x3A1 
#define PMPCFG0			0x3A0 
#define PMPCFG3			0x3A3 
#define PMPCFG2			0x3A2 
#define SATP			0x180 	
#define HPMCOUNTER15	0xC0F 	
#define HPMCOUNTER13	0xC0D 	
#define HPMCOUNTER14	0xC0E 	
#define HPMCOUNTER11	0xC0B 	
#define HPMCOUNTER12	0xC0C 	
#define HPMCOUNTER10	0xC0A 	
#define MHPMEVENT8		0x328 	
#define HPMCOUNTER15H	0xC8F 	
#define HPMCOUNTER13H	0xC8D 	
#define HPMCOUNTER14H	0xC8E 	
#define HPMCOUNTER11H	0xC8B 	
#define HPMCOUNTER12H	0xC8C 	
#define HPMCOUNTER10H	0xC8A 	
#define HPMCOUNTER25H	0xC99 	
#define HPMCOUNTER24H	0xC98 	
#define HPMCOUNTER21H	0xC95 	
#define HPMCOUNTER20H	0xC94 	
#define HPMCOUNTER23H	0xC97 	
#define HPMCOUNTER22H	0xC96 	
#define HPMCOUNTER17H	0xC91 	
#define HPMCOUNTER16H	0xC90 	
#define HPMCOUNTER19H	0xC93 	
#define HPMCOUNTER18H	0xC92 	
#define HPMCOUNTER21	0xC15 	
#define STVEC			0x105 	
#define SCOUNTEREN		0x106 	
#define HPMCOUNTER22	0xC16 	
#define HPMCOUNTER17	0xC11 	
#define HPMCOUNTER16	0xC10 	
#define HPMCOUNTER19	0xC13 	
#define HPMCOUNTER18	0xC12 	
#define HPMCOUNTER25	0xC19 	
#define HPMCOUNTER24	0xC18 	
#define HPMCOUNTER30	0xC1E 	
#define HPMCOUNTER29	0xC1D 	
#define HPMCOUNTER31	0xC1F 	
#define HPMCOUNTER26	0xC1A 	
#define HPMCOUNTER28	0xC1C 	
#define HPMCOUNTER27	0xC1B 	
#define UTVEC			0x005 	
#define UIE				0x004 
#define TDATA1			0x7A1 	
#define FFLAGS			0x001 	
#define USTATUS			0x000 
#define FCSR			0x003 	
#define FRM				0x002 
#define HPMCOUNTER30H	0xC9E 	
#define HPMCOUNTER29H	0xC9D 	
#define HPMCOUNTER31H	0xC9F 	
#define HPMCOUNTER26H	0xC9A 	
#define HPMCOUNTER28H	0xC9C 	
#define HPMCOUNTER27H	0xC9B 	
#define HPMCOUNTER8H	0xC88 	
#define HPMCOUNTER9H	0xC89 	
#define HPMCOUNTER6H	0xC86 	
#define HPMCOUNTER7H	0xC87 	
#define HPMCOUNTER4H	0xC84 	
#define HPMCOUNTER5H	0xC85 	
#define INSTRETH		0xC82 	
#define HPMCOUNTER3H	0xC83 	
#define CYCLEH			0xC80 	
#define TIMEH			0xC81 	
#define HPMCOUNTER6		0xC06 
#define HPMCOUNTER7		0xC07 
#define HPMCOUNTER4		0xC04 
#define HPMCOUNTER5		0xC05 
#define INSTRET			0xC02 
#define HPMCOUNTER3		0xC03 
#define CYCLE			0xC00 	
#define TIME			0xC01 	
#define HPMCOUNTER8		0xC08 
#define HPMCOUNTER9		0xC09 
#define MCOUNTEREN		0x306 	
#define MIE				0x304 
#define MTVEC			0x305 	
#define MEDELEG			0x302 
#define MIDELEG			0x303 
#define MSTATUS			0x300 
#define MISA			0x301 	
#define PMPADDR8		0x3B8 	
#define PMPADDR9		0x3B9 	
#define PMPADDR4		0x3B4 	
#define PMPADDR5		0x3B5 	
#define PMPADDR6		0x3B6 	
#define PMPADDR7		0x3B7 	
#define PMPADDR0		0x3B0 	
#define PMPADDR1		0x3B1 	
#define PMPADDR2		0x3B2 	
#define PMPADDR3		0x3B3 	
#define TDATA2			0x7A2 	
#define MHPMEVENT9		0x329 	
#define MHPMEVENT28		0x33C 
#define MHPMEVENT27		0x33B 
#define MHPMEVENT26		0x33A 
#define MHPMEVENT31		0x33F 
#define MHPMEVENT30		0x33E 
#define MHPMEVENT29		0x33D 
#define MHPMEVENT19		0x333 
#define MHPMEVENT18		0x332 
#define MHPMEVENT17		0x331 
#define MHPMEVENT16		0x330 
#define MHPMEVENT23		0x337 
#define MHPMEVENT22		0x336 
#define MHPMEVENT21		0x335 
#define MHPMEVENT20		0x334 
#define MHPMEVENT25		0x339 
#define MHPMEVENT24		0x338 
#define PMPADDR13		0x3BD 	
#define PMPADDR14		0x3BE 	
#define PMPADDR15		0x3BF 	
#define PMPADDR10		0x3BA 	
#define PMPADDR11		0x3BB 	
#define PMPADDR12		0x3BC 	
#define DCSR			0x7B0 	
#define DPC				0x7B1 
#define DSCRATCH		0x7B2 	
#define TSELECT			0x7A0 
#define MHPMCOUNTER26H	0xB9A 	
#define MHPMCOUNTER28H	0xB9B 	
#define MHPMCOUNTER29H	0xB9D 	
#define MHPMCOUNTER30H	0xB9E 	
#define MHPMCOUNTER31H	0xB9F 	
#define MHPMEVENT4		0x324 	
#define MHPMEVENT5		0x325 	
#define MHPMEVENT6		0x326 	
#define MHPMEVENT7		0x327 	
#define MHPMEVENT3		0x323 	
#define MHARTID			0xF14 
#define MVENDORID		0xF11 	
#define MARCHID			0xF12 
#define MIMPID			0xF13 	
#define MHPMCOUNTER26	0xB1A 	
#define MHPMCOUNTER28	0xB1B 	
#define MHPMCOUNTER29	0xB1D 	
#define MHPMCOUNTER30	0xB1E 	
#define MHPMCOUNTER31	0xB1F 	
#define MHPMCOUNTER9	0xB09 	
#define MHPMCOUNTER8	0xB08 	
#define MHPMCOUNTER7	0xB07 	
#define MHPMCOUNTER6	0xB06 	
#define MHPMCOUNTER5	0xB05 	
#define MHPMCOUNTER4	0xB04 	
#define MHPMCOUNTER3	0xB03 	
#define MINSTRET		0xB02 	
#define MCYCLE			0xB00 	
#define MHPMCOUNTER7H	0xB87 	
#define MHPMCOUNTER6H	0xB86 	
#define MHPMCOUNTER5H	0xB85 	
#define MHPMCOUNTER4H	0xB84 	
#define MHPMCOUNTER3H	0xB83 	
#define MINSTRETH		0xB82 	
#define MCYCLEH			0xB80 
#define SSCRATCH		0x140 	
#define SEPC			0x141 	
#define SCAUSE			0x142 	
#define STVAL			0x143 	
#define SIP				0x144 
#define MHPMCOUNTER9H	0xB89 	
#define MHPMCOUNTER8H	0xB88 	
#define MHPMCOUNTER15H	0xB8F 	
#define MHPMCOUNTER14H	0xB8E 	
#define MHPMCOUNTER13H	0xB8D 	
#define MHPMCOUNTER12H	0xB8B 	
#define MHPMCOUNTER10H	0xB8A 	
#define MHPMEVENT13		0x32D 
#define MHPMEVENT14		0x32E 
#define MHPMCOUNTER15	0xB0F 	
#define MHPMCOUNTER14	0xB0E 	
#define MHPMCOUNTER13	0xB0D 	
#define MHPMCOUNTER12	0xB0B 	
#define MHPMCOUNTER10	0xB0A 	
#define MHPMCOUNTER24	0xB18 	
#define MHPMCOUNTER25	0xB19 	
#define MHPMCOUNTER16	0xB10 	
#define MHPMCOUNTER17	0xB11 	
#define MHPMCOUNTER18	0xB12 	
#define MHPMCOUNTER19	0xB13 	
#define MHPMCOUNTER20	0xB14 	
#define MHPMCOUNTER21	0xB15 	
#define MHPMCOUNTER22	0xB16 	
#define MHPMCOUNTER23	0xB17 	
#define MHPMCOUNTER16H	0xB90 	
#define MHPMCOUNTER17H	0xB91 	
#define MHPMCOUNTER18H	0xB92 	
#define MHPMCOUNTER19H	0xB93 	
#define MHPMCOUNTER20H	0xB94 	
#define MHPMCOUNTER21H	0xB95 	
#define MHPMCOUNTER22H	0xB96 	
#define MHPMCOUNTER23H	0xB97 	
#define MHPMCOUNTER24H	0xB98 	
#define MHPMCOUNTER25H	0xB99 	
#define MHPMEVENT15		0x32F 
#define MHPMEVENT10		0x32A 
#define MHPMEVENT11		0x32B 
#define MHPMEVENT12		0x32C 	



struct mstatus_t{
    uint8_t uie, sie, mie, upie, spie, mpie, spp, mpp, fs, xs, mprv, sum, mxr, tvm, tw, tsr, uxl, sxl, sd;
    mstatus_t() {
        uie = 0; sie = 1; mie = 1;
        upie = 0; spie = 0; mpie = 0; 
        spp = 0; mpp = 0b0; 
        fs = 0; xs = 0; mprv = 0; sum = 0; mxr = 0; tvm = 0; tw = 0; tsr = 0; uxl = 2; sxl = 0; sd = 0; 
    }
    uint_t read_reg(){
        return (((uint_t)sd<<63)+((uint_t)sxl<<34)+((uint_t)uxl<<32)+(tsr<<22)+(tw<<21)+(tvm<<20)+(mxr<<19)+(sum<<18)+(mprv<<17)+(xs<<15)+(fs<<13)+(mpp<<11)+(spp<<8)+(mpie<<7)+(spie<<5)+(upie<<4)+(mie<<3)+(sie<<1)+uie);
    }

    void write_reg(uint_t val){
        uie = (val & 0b1); sie = ((val>>1)& 0b1); mie = ((val>>3)& 0b1);
        upie= ((val>>4)& 0b1); spie= ((val>>5)& 0b1); mpie= ((val>>7)& 0b1); 
        spp= ((val>>8)& 0b1); mpp = ((val>>11)& 0b11); //0b11; removed hard wire to 11 
        fs= ((val>>13)& 0b11); xs= ((val>>15)& 0b11); mprv= ((val>>17)& 0b1); sum= ((val>>18)& 0b1); mxr= ((val>>19)& 0b1); tvm= ((val>>20)& 0b1); tw= ((val>>21)& 0b1); tsr= ((val>>22)& 0b1); uxl= ((val>>32)& 0b11); sxl= ((val>>34)& 0b11); sd= ((val>>63)& 0b1); 
    }
} mstatus;

struct sstatus_t{
    //uint8_t uie, sie, upie, spie, spp, fs, xs, sum, mxr, uxl, sd;
    uint8_t &uie = mstatus.uie; uint8_t &sie = mstatus.sie; 
    uint8_t &upie = mstatus.upie; uint8_t &spie = mstatus.spie; 
    uint8_t &spp = mstatus.spp; 
    uint8_t &fs = mstatus.fs; uint8_t &xs = mstatus.xs; uint8_t &sum = mstatus.sum; uint8_t &mxr = mstatus.mxr; uint8_t &uxl = mstatus.uxl; uint8_t &sd = mstatus.sd; 
    sstatus_t() {

    }
    uint_t read_reg(){
        return (((uint_t)sd<<63)+((uint_t)uxl<<32)+(mxr<<19)+(sum<<18)+(xs<<15)+(fs<<13)+(spp<<8)+(spie<<5)+(upie<<4)+(sie<<1)+uie);
    }

    void write_reg(uint_t val){

		if (((val>>5) & 0b1)!=spie){
			cout << "spie write" << hex << ((val>>6) & 0b1)<<endl;
		}
        uie = (val & 0b1); sie = ((val>>1)& 0b1); 
        upie= ((val>>4)& 0b1); spie= ((val>>5)& 0b1);
        spp= ((val>>8)& 0b1); 
        fs= ((val>>13)& 0b11); xs= ((val>>15)& 0b11); sum= ((val>>18)& 0b1); mxr= ((val>>19)& 0b1); uxl= ((val>>32)& 0b11);  sd= ((val>>63)& 0b1); 
    }
} sstatus;

struct ustatus_t{
    //uint8_t uie, sie, upie, spie, spp, fs, xs, sum, mxr, uxl, sd;
    uint8_t &uie = mstatus.uie; 
    uint8_t &upie = mstatus.upie; 
    uint8_t &fs = mstatus.fs; uint8_t &xs = mstatus.xs; uint8_t &sum = mstatus.sum; uint8_t &mxr = mstatus.mxr; uint8_t &uxl = mstatus.uxl; uint8_t &sd = mstatus.sd; 
    ustatus_t() {

    }
    uint_t read_reg(){
        //uie = 0;
        //upie = 0; //usermode interrupts not supported
        return (((uint_t)sd<<63)+((uint_t)uxl<<32)+(mxr<<19)+(sum<<18)+(xs<<15)+(fs<<13)+(upie<<4)+uie);
    }

    void write_reg(uint_t val){
        uie = (val & 0b1);  
        upie= ((val>>5)& 0b1); 
        fs= ((val>>13)& 0b11); xs= ((val>>15)& 0b11); sum= ((val>>18)& 0b1); mxr= ((val>>19)& 0b1); uxl= ((val>>32)& 0b11);  sd= ((val>>63)& 0b1); 
    }
} ustatus;

uint_t mscratch=0;
uint_t sscratch=0;

uint_t medeleg = 0;
uint_t sedeleg = 0;

uint_t mideleg = 0;
uint_t sideleg = 0;

uint_t misa = 0;

uint_t mepc = 0;
uint_t sepc = 0;
uint_t uepc = 0;

uint_t cycle = 0;
uint_t time_csr = 0;
uint_t instret = 0;


uint_t mhartid = 0;
uint_t mvendorid = 0;   
uint_t marchid = 0;
uint_t mimpid = 0;

uint_t mtval = 0;
uint_t stval = 0;
uint_t utval = 0;

uint_t mcounteren = 0;
uint_t scounteren = 0;

uint_t pmpaddr8 = 0;
uint_t pmpaddr9 = 0;
uint_t pmpaddr4 = 0;
uint_t pmpaddr5 = 0;
uint_t pmpaddr6 = 0;
uint_t pmpaddr7 = 0;
uint_t pmpaddr0 = 0;
uint_t pmpaddr1 = 0;
uint_t pmpaddr2 = 0;
uint_t pmpaddr3 = 0;

uint_t pmpcfg1 = 0;
uint_t pmpcfg0 = 0;
uint_t pmpcfg3 = 0;
uint_t pmpcfg2 = 0;


struct mtvec_t{
    uint8_t mode;
    uint_t base;
    mtvec_t() {
    	mode = 0;
    	base = 0;
    }
    uint_t read_reg(){
        return ((mode & 0b11)+(base & (MASK64-0b11)));
    }

    void write_reg(uint_t val){
    	mode = val & 0b11;
    	//base = (val & (MASK64 - 0b11));  
        base = ( val & (MASK64-0b11) );  
    }
} mtvec;

struct stvec_t{
    uint8_t mode;
    uint_t base;
    stvec_t() {
    	mode = 0;
    	base = 0;
    }
    uint_t read_reg(){
        return ((mode & 0b11)+(base & (MASK64-0b11)));
    }

    void write_reg(uint_t val){
    	mode = val & 0b11;
    	//base = (val & (MASK64 - 0b11)); 
        base = ( val & (MASK64-0b11) );    
    }
} stvec;

struct utvec_t{
    uint8_t mode;
    uint_t base;
    utvec_t() {
    	mode = 0;
    	base = 0;
    }
    uint_t read_reg(){
        return ((mode & 0b11)+(base & (MASK64-0b11)));
    }

    void write_reg(uint_t val){
    	mode = val & 0b11;
    	//base = (val & (MASK64 - 0b11));  
        base = ( val & (MASK64-0b11) );   
    }
} utvec;

struct mcause_t{
    uint8_t interrupt;
    uint_t ecode;
    mcause_t() {
    	interrupt = 0;
    	ecode = 0;
    }
    uint_t read_reg(){
        return ((((1llu<<63)-1) & ecode)+((uint_t)interrupt<<63));
    }

    void write_reg(uint_t val){
    	ecode = val & ((1llu<<63)-1);
    	interrupt = (val>>63) & 0b1;    
    }
} mcause;

struct scause_t{
    uint8_t interrupt;
    uint_t ecode;
    scause_t() {
    	interrupt = 0;
    	ecode = 0;
    }
    uint_t read_reg(){
        return ((((1llu<<63)-1) & ecode)+((uint_t)interrupt<<63));
    }

    void write_reg(uint_t val){
    	ecode = val & ((1llu<<63)-1);
    	interrupt = (val>>63) & 0b1;    
    }
} scause;

struct ucause_t{
    uint8_t interrupt;
    uint_t ecode;
    ucause_t() {
    	interrupt = 0;
    	ecode = 0;
    }
    uint_t read_reg(){
        return ((((1llu<<63)-1) & ecode)+((uint_t)interrupt<<63));
    }

    void write_reg(uint_t val){
    	ecode = val & ((1llu<<63)-1);
    	interrupt = (val>>63) & 0b1;    
    }
} ucause;

struct satp_t{
    uint_t PPN; uint16_t ASID; uint8_t MODE=0;
    satp_t(){
        PPN = 0;
        ASID = 0;
        MODE = 0;
    }
    uint_t read_reg(){
        return  ( ( (uint_t)(MODE & 0xF)<<60 ) + ((uint_t)ASID<<44) + (PPN & 0xFFFFFFFFFFFull));
    }

    void write_reg(uint_t val){
        PPN = val & 0xFFFFFFFFFFFull;
        ASID = (val>>44) & 0xFFFF ;
        MODE = (val>>60) & 0xF ;
    }
} satp;

struct mip_t{
    uint8_t MEIP; uint8_t SEIP; uint8_t UEIP; uint8_t MTIP; uint8_t STIP; uint8_t UTIP; uint8_t MSIP; uint8_t SSIP; uint8_t USIP;
    mip_t(){
        MEIP = 0; SEIP = 0; UEIP = 0; MTIP = 0; STIP = 0; UTIP = 0; MSIP = 0; SSIP = 0; USIP = 0;
    }
    uint_t read_reg(){
        return  (((MEIP & 0b1)<<11)+((SEIP & 0b1)<<9)+((UEIP & 0b1)<<8)+((MTIP & 0b1)<<7)+((STIP & 0b1)<<5)+((UTIP & 0b1)<<4)+((MSIP & 0b1)<<3)+((SSIP & 0b1)<<1)+(USIP & 0b1));
    }

    void write_reg(uint_t val){
        MEIP = (val>>11) & 0b1;
        SEIP = (val>> 9) & 0b1;
        UEIP = (val>> 8) & 0b1;
        MTIP = (val>> 7) & 0b1;
        STIP = (val>> 5) & 0b1;
        UTIP = (val>> 4) & 0b1;
        MSIP = (val>> 3) & 0b1;
        SSIP = (val>> 1) & 0b1;
        USIP = val& 0b1;
    }
} mip;

struct mie_t{
    uint8_t MEIE; uint8_t SEIE; uint8_t UEIE; uint8_t MTIE; uint8_t STIE; uint8_t UTIE; uint8_t MSIE; uint8_t SSIE; uint8_t USIE;
    mie_t(){
        MEIE = 0; SEIE = 0; UEIE = 0; MTIE = 0; STIE = 0; UTIE = 0; MSIE = 0; SSIE = 0; USIE = 0;
    }
    uint_t read_reg(){
        return  (((MEIE & 0b1)<<11)+((SEIE & 0b1)<<9)+((UEIE & 0b1)<<8)+((MTIE & 0b1)<<7)+((STIE & 0b1)<<5)+((UTIE & 0b1)<<4)+((MSIE & 0b1)<<3)+((SSIE & 0b1)<<1)+(USIE & 0b1));
    }
    void write_reg(uint_t val){
        MEIE = (val>>11) & 0b1;
        SEIE = (val>> 9) & 0b1;
        UEIE = (val>> 8) & 0b1;
        MTIE = (val>> 7) & 0b1;
        STIE = (val>> 5) & 0b1;
        UTIE = (val>> 4) & 0b1;
        MSIE = (val>> 3) & 0b1;
        SSIE = (val>> 1) & 0b1;
        USIE = val& 0b1;
    }
} mie;



uint_t csr_read(uint_t csr_addr){
    csr_read_success = true;
    switch(csr_addr){
        case MSTATUS :
        if (cp==MMODE){
            csr_read_success = true;
            //cout << "read mstatus : "<<hex<<mstatus.read_reg()<<endl;
            return mstatus.read_reg();
        }
        else{
            cout << "writing to mstatus only allowed in MMODE"<<endl;
            csr_read_success = false;
            return 1;
        }
            break;

        case SSTATUS :
            if ((cp==MMODE) | (cp==SMODE)){
                csr_read_success = true;
            	
                return sstatus.read_reg();
            }else{
                cout << "writing to sstatus only allowed in MMODE and SMODE"<<endl;
                csr_read_success = false;
                return 1;
            }
            break;
        case USTATUS :
            return ustatus.read_reg();
            break;
        case MSCRATCH :
        	return mscratch;
        	break;
        case SSCRATCH :
            return sscratch;
            break;
        case MISA :
        	return misa;
        	break;
        case MEPC :
        	return mepc;
        	break;
        case SEPC :
        	return sepc;
        	break;
        case UEPC :
        	return uepc;
        	break;
        case MTVEC :
        	return mtvec.read_reg();
        	break;
        case STVEC :
        	return stvec.read_reg();
        	break;
        case UTVEC :
        	return utvec.read_reg();
        	break;
        case MCAUSE :
        	return mcause.read_reg();
        	break;
        case SCAUSE :
        	return scause.read_reg();
        	break;
        case UCAUSE :
        	return ucause.read_reg();
        	break;
        case MEDELEG :
        	return medeleg;
        	break;
        case SEDELEG :
        	return sedeleg;
        	break;
        case MIDELEG :
        	return mideleg;
        	break;
        case SIDELEG :
        	return sideleg;
        	break;
        case CYCLE :
            return cycle;
            break;
        case TIME :
            //cout << "rdtime : "<<time_csr<<endl;
            return time_csr;
            break;
        case INSTRET :
            return instret;
            break;
        case MHARTID :
            return mhartid;
            break;
        case MVENDORID :
            return mvendorid;
            break;
        case MARCHID :
            return marchid;
            break;
        case MIMPID :
            return mimpid;
            break;
        case SATP :
            return satp.read_reg();
            break;
        case MTVAL :
            return mtval;
            break;
        case STVAL :
            return stval;
            break;
        case UTVAL :
            return utval;
            break;
        case MIP :
            return mip.read_reg();
            break;
        case SIP :
            return mip.read_reg();
            break;
        case MIE :
            return mie.read_reg();
            break;
        case SIE :
            return mie.read_reg();
            break;
        case SCOUNTEREN :
            return scounteren;
            break;
        case MCOUNTEREN :
            return mcounteren;
            break;
        case PMPADDR8 : 
            return pmpaddr8; 
            break;
        case PMPADDR9 : 
            return pmpaddr9; 
            break;
        case PMPADDR4 : 
            return pmpaddr4; 
            break;
        case PMPADDR5 : 
            return pmpaddr5; 
            break;
        case PMPADDR6 : 
            return pmpaddr6; 
            break;
        case PMPADDR7 : 
            return pmpaddr7; 
            break;
        case PMPADDR0 : 
            return pmpaddr0; 
            break;
        case PMPADDR1 : 
            return pmpaddr1; 
            break;
        case PMPADDR2 : 
            return pmpaddr2; 
            break;
        case PMPADDR3 : 
            return pmpaddr3; 
            break;
        case PMPCFG1 : 
            return pmpcfg1; 
            break;
        case PMPCFG0 : 
            return pmpcfg0; 
            break;
        case PMPCFG3 : 
            return pmpcfg3; 
            break;
        case PMPCFG2 : 
            return pmpcfg2; 
            break;
        default:
            cout << "CSR not implemented : " << hex << csr_addr << endl;
            break;
    }
}

bool csr_write(uint_t csr_addr, uint_t val){
    switch(csr_addr){
        case MSTATUS :
            //cout << "write mstatus : "<<hex<<val<<endl;
            mstatus.write_reg(val);
            break;
        case SSTATUS :
            sstatus.write_reg(val);
            break;
        case USTATUS :
            ustatus.write_reg(val);
            break;
        case MSCRATCH :
        	mscratch = val;
        	break;
        case SSCRATCH :
            sscratch = val;
            break;
        case MISA :
        	misa = val;
        	break;
        case MEPC :
        	mepc = val;
        	break;
        case SEPC :
        	sepc = val;
        	break;
        case UEPC :
        	uepc = val;
        	break;
        case MTVEC :
            mtvec.write_reg(val);
            break;
        case STVEC :
            stvec.write_reg(val);
            //cout << "Writing stvec" << hex <<val<<endl;
            break;
        case UTVEC :
            utvec.write_reg(val);
            break;
        case MCAUSE :
            mcause.write_reg(val);
            break;
        case SCAUSE :
            scause.write_reg(val);
            break;
        case UCAUSE :
            ucause.write_reg(val);
            break;
        case MEDELEG :
        	medeleg = val;
        	break;
        case SEDELEG :
        	sedeleg = val;
        	break;
        case MIDELEG :
        	mideleg = val;
        	break;
        case SIDELEG :
        	sideleg = val;
        	break;
        case CYCLE : //this should be an exception
            cout << "Exception : Writing to cycle CSR"<<endl; 
            return false;
            break;
        case TIME :
            time_csr = val;
            break;
        case INSTRET :
            instret = val;
            break;
        case MHARTID :
            mhartid = val;
            break;
        case MVENDORID :
            mvendorid = val;
            break;
        case MARCHID :
            marchid = val;
            break;
        case MIMPID :
            mimpid = val;
            break;
        case SATP :
            satp.write_reg(val);
            break;
        case MTVAL :
            mtval = val;
            break;
        case STVAL :
            stval = val;
            break;
        case UTVAL :
            utval = val;
            break;
        case MIP :
            mip.write_reg(val);
            break;
        case SIP :
            mip.write_reg(val);
            break;
        case MIE :
            mie.write_reg(val);
            break;
        case SIE :
            mie.write_reg(val);
            break;
        case SCOUNTEREN :
            scounteren = val;
            break;
        case MCOUNTEREN :
            mcounteren = val;
            break;
        case PMPADDR8 : 
            pmpaddr8 = val; 
            break;
        case PMPADDR9 : 
            pmpaddr9 = val; 
            break;
        case PMPADDR4 : 
            pmpaddr4 = val; 
            break;
        case PMPADDR5 : 
            pmpaddr5 = val; 
            break;
        case PMPADDR6 : 
            pmpaddr6 = val; 
            break;
        case PMPADDR7 : 
            pmpaddr7 = val; 
            break;
        case PMPADDR0 : 
            pmpaddr0 = val; 
            break;
        case PMPADDR1 : 
            pmpaddr1 = val; 
            break;
        case PMPADDR2 : 
            pmpaddr2 = val; 
            break;
        case PMPADDR3 : 
            pmpaddr3 = val; 
            break;
        case PMPCFG1 : 
            pmpcfg1 = val; 
            break;
        case PMPCFG0 : 
            pmpcfg0 = val; 
            break;
        case PMPCFG3 : 
            pmpcfg3 = val; 
            break;
        case PMPCFG2 : 
            pmpcfg2 = val; 
            break;
        default:
            cout << "CSR not implemented : " << hex <<csr_addr << endl;
            return false;
            break;
    }
    return true;
}



static uint_t clint_read( uint_t offset)
{

    uint_t val;

    switch(offset) {
    case 0xbff8:    //rtc time
        val = mtime;
        break;
    case 0x4000:    //timecmp
        val = mtimecmp;
        break;
    default:
        val = 0;
        break;
    }
    return val;
}

static void clint_write(uint_t offset, uint_t val)
{
    switch(offset) {
        case 0x4000:
            mtimecmp = val;
            mip.STIP=0;
            break;
        default:
            break;
    }
}

static uint_t virtio_read( uint_t offset)
{
    return 0;
    uint_t val;

    switch(offset) {
    case 0xbff8:    //rtc time
        val = mtime;
        break;
    case 0x4000:    //timecmp
        val = mtimecmp;
        break;
    default:
        val = 0;
        break;
    }
    return val;
}


extern "C" uint32_t mem_read32(uint64_t addr){
    uint64_t load_data = memory.at((addr - DRAM_BASE)/8);
    uint64_t wb_data = 0;
    load_word(addr, load_data, wb_data);

    return (uint32_t)wb_data;
}

extern "C" void mem_write32(uint64_t addr, uint64_t data){
    uint64_t store_data = 0;
    store_data = memory.at((addr - DRAM_BASE)/8);
    uint64_t wb_data = 0;
    store_word(addr, store_data, data, wb_data);
    memory.at((addr - DRAM_BASE)/8) = (  wb_data);
}

extern "C" uint16_t mem_read16(uint64_t addr){
    uint64_t load_data = memory.at((addr - DRAM_BASE)/8);
    uint64_t wb_data = 0;
    load_halfw(addr, load_data, wb_data);
    return (uint16_t)wb_data;
}

extern "C" void mem_write16(uint64_t addr, uint64_t data){
    uint64_t store_data = memory.at((addr - DRAM_BASE)/8);
    uint64_t wb_data = 0;
    store_halfw(addr, store_data, data, wb_data);
    memory.at((addr - DRAM_BASE)/8) = (  wb_data);
}

extern "C" uint8_t mem_read8(uint64_t addr){
    uint64_t load_data = memory.at((addr - DRAM_BASE)/8);
    uint64_t wb_data = 0;
    load_byte(addr, load_data, wb_data);
    return (uint8_t)wb_data;
}

extern "C" void mem_write8(uint64_t addr, uint64_t data){
    uint64_t store_data = memory.at((addr - DRAM_BASE)/8);
    uint64_t wb_data = 0;
    store_byte(addr, store_data, data, wb_data);
    memory.at((addr - DRAM_BASE)/8) = (  wb_data);
}

////////////////////////////////////////PLIC Related things///////////////////////////////
#define PLIC_HART_BASE 0x200000
#define PLIC_HART_SIZE 0x1000
static uint_t plic_read( uint_t offset)
{
    
    uint_t val;

    switch(offset) {
        case PLIC_HART_BASE:
            val = 0;
            break;
        case PLIC_HART_BASE + 4:
         
            if (mip.SEIP==1){
                
                plic_served_irq = true;
                val =1;
                mip.SEIP = 0;
            }
            else 
                val = 0;
            break;
        default:
            val =0;
            break;
    }
    return val;
}

static void plic_write(uint_t offset, uint_t val)
{

    switch(offset) {
  
        case PLIC_HART_BASE + 4:
            plic_served_irq = false;
            if (plic_pending_irq)
                mip.SEIP = 1;
            else
                mip.SEIP = 0;
            break;
        default:
            break;
    }

}


void plic_set_irq_emu(int irq_num, int state)
{
    uint32_t mask;

    mask = 1 << (irq_num - 1);
    if (state){
        plic_pending_irq = true;
        if (~plic_served_irq)
            mip.SEIP = 1;
        else
            mip.SEIP = 0;
    }
    else{
        plic_pending_irq = false;
        mip.SEIP = 0;
    }
    //plic_update_mip(s);
}

plevel_t trap_mode_select(uint_t cause, bool interrupt, plevel_t current_privilage){
	uint_t mtrap_deleg_reg = 0;
	uint_t strap_deleg_reg = 0;
	switch(current_privilage){
		case MMODE : 
			return MMODE;
			break;

		case SMODE :
			if (interrupt)
				mtrap_deleg_reg = mideleg;
			else 
				mtrap_deleg_reg = medeleg;

            //if (mtrap_deleg_reg == -1)
            //    return current_privilage;

			if (((mtrap_deleg_reg>>cause) & 0b1 ) == 1)
				return SMODE;
			else 
				return MMODE;
			break;

		case UMODE :
			if (interrupt){
				mtrap_deleg_reg = mideleg;
				strap_deleg_reg = sideleg;
			}
			else {
				mtrap_deleg_reg = medeleg;
				strap_deleg_reg = sedeleg;
			}

            //if (mtrap_deleg_reg == -1)
            //    return current_privilage;

			if (((mtrap_deleg_reg>>cause) & 0b1 ) == 1){     		//delegating to smode
            	if (((strap_deleg_reg>>cause) & 0b1 ) == 1){      //delegating to umode
                    return UMODE;
                }
            	else
                	return SMODE;
            }
        	else
        		return MMODE;

        	break;
	}
}


uint_t excep_function(uint_t PC, uint_t mecode , uint_t secode, uint_t uecode, plevel_t current_privilage){

	uint_t ecode = 0;
	uint_t new_PC = 0;

	if (current_privilage == UMODE)
        ecode = uecode;
    else if (current_privilage == SMODE)
        ecode = secode;
    else if (current_privilage == MMODE)
        ecode = mecode;

    plevel_t handling_mode = trap_mode_select(ecode, false, current_privilage);
    //plevel_t handling_mode = current_privilage;
    //cout << "handling mode : "<< (uint_t)handling_mode<<endl;
/*
    if (handling_mode == UMODE)
        ecode = uecode;
    else if (handling_mode == SMODE)                          ### ecode should not depend on handling mode
        ecode = secode;
    else if (handling_mode == MMODE)
        ecode = mecode;
*/
    if ( handling_mode == UMODE){
        exit(4);
        cp = UMODE;
    	ustatus.upie = ustatus.uie;
        ustatus.uie  = 0;
        ucause.interrupt = 0;
        ucause.ecode = ecode;
        uepc = PC-4;

        mstatus.mpp = 0b0; // setting both to UMODE
        mstatus.spp = 0b0;

        new_PC = utvec.base;   
    }
    else if ( handling_mode == SMODE ){
        mstatus.mpp = (uint_t)cp; // setting both to SMODE
        mstatus.spp = (uint_t)cp;
        cp = SMODE;
    	sstatus.spie = sstatus.sie;
        sstatus.sie  = 0;
        scause.interrupt = 0;
        scause.ecode = ecode;
        sepc = PC-4;

        new_PC = stvec.base; 
    }
    else if ( handling_mode == MMODE ){
    //cout << "excep MMODE : " << mtvec.base << endl;
        mstatus.mpp = (uint_t)cp;
        cp = MMODE;
    	mstatus.mpie = mstatus.mie;
        mstatus.mie  = 0;
        mcause.interrupt = 0;
        mcause.ecode = ecode;
        mepc = PC-4;
        //mstatus.mpp = 0b11; // setting to MMODE
        //if (ecode == 2){
        //    new_PC = 0x3ac0;
        //}
        //else{
        new_PC = mtvec.base; 
        //}
    }
    else{ 
    	cout << "Unrecognized mode for excep_function" <<endl;
        return PC;
    }


    return new_PC;
}

uint_t interrupt_function(uint_t PC, uint_t mecode, plevel_t current_privilage){

    uint_t ecode = 0;
    uint_t new_PC = 0;



    /*if (current_privilage == UMODE)
        ecode = uecode;
    else if (current_privilage == SMODE)
        ecode = secode;
    else if (current_privilage == MMODE)*/
        ecode = mecode;

    plevel_t handling_mode = trap_mode_select(ecode, true, current_privilage);
    //plevel_t handling_mode = current_privilage;

    //cout << "handling mode : "<<(uint_t)handling_mode<<endl;
    //cout << "sie : "<<(uint_t)mstatus.sie<<endl;

    //mstatus.sie = 1;
/*
    if (handling_mode == UMODE)
        ecode = uecode;                         ### ecode should not depend on handling mode
    else if (handling_mode == SMODE)
        ecode = secode;
    else if (handling_mode == MMODE)
        ecode = mecode;
*/
    if ((handling_mode == UMODE) & (mstatus.uie==1)){
        exit(1);
        cp = UMODE;
        ustatus.upie = ustatus.uie;
        ustatus.uie  = 0;
        ucause.interrupt = 1;
        ucause.ecode = ecode;
        uepc = PC;

        mstatus.mpp = 0b0; // setting both to UMODE
        mstatus.spp = 0b0;
        if (utvec.mode ==0b1)
            new_PC = utvec.base + 4*ecode;
        else if (utvec.mode ==0b0)
            new_PC = utvec.base ;

    }
    else if ((handling_mode == SMODE) & (mstatus.sie==1)){
        mstatus.mpp = (uint_t)cp; // setting both to SMODE
        mstatus.spp = (uint_t)cp;
        cp = SMODE;
        sstatus.spie = sstatus.sie;
        sstatus.sie  = 0;
        scause.interrupt = 1;
        scause.ecode = ecode;
        sepc = PC;
        //cout << "handling in smode "<<(int) sstatus.spie <<endl;
        if (stvec.mode ==0b1){
            exit(2);
            new_PC = stvec.base + 4*ecode;
            //cout << "stvec mode 1" << endl;
        }
        else if (stvec.mode ==0b0){
            //exit(57);
            new_PC = stvec.base ;
            if (current_privilage == MMODE){
                exit(5);
            }
            //cout << "stvec mode 2"<<hex<< new_PC<< endl;
        }
    }
    else if ((handling_mode == MMODE) & (mstatus.mie==1)){
        //cout << "excep MMODE : " << mtvec.base << endl;
        mstatus.mpp = (uint_t)cp;
        cp = MMODE;
        mstatus.mpie = mstatus.mie;
        mstatus.mie  = 0;
        mcause.interrupt = 1;
        mcause.ecode = ecode;
        mepc = PC;

        if (mtvec.mode ==0b1){
            exit(3);
            new_PC = mtvec.base + 4*ecode;
        }
        else if (mtvec.mode ==0b0){
            new_PC = mtvec.base ;
        if (current_privilage == MMODE){
            exit(5);
        }
    }

    }
    else {
        //cout << "Unrecognized mode for interrupt_function" <<endl;
        return PC;
    }


    return new_PC;
}

enum ttype_t{
    INST = 0b00,
    LOAD = 0b01,
    STOR = 0b10
};

struct sv39va_t{
    uint16_t page_offset ; uint16_t VPN0; uint16_t VPN1; uint16_t VPN2;
    sv39va_t(){
        page_offset = 0;
        VPN0 = 0;
        VPN1 = 0;
        VPN2 = 0;
    }
    uint_t read_reg(){
        return  ( ((VPN2 & 0x1FF)<<30) + ((VPN1 & 0x1FF)<<21) + ((VPN0 & 0x1FF)<<12) + (page_offset & 0xFFF) );
    }
    void write_reg(uint_t val){
        page_offset = val & 0xFFF;
        VPN0 = (val>>12) & 0x1FF;
        VPN1 = (val>>21) & 0x1FF;
        VPN2 = (val>>30) & 0x1FF;
    }
};

struct sv39pa_t{
    uint16_t page_offset ; uint16_t PPN0; uint16_t PPN1; uint32_t PPN2;
    sv39pa_t(){
        page_offset = 0;
        PPN0 = 0;
        PPN1 = 0;
        PPN2 = 0;
    }
    uint_t read_reg(){
        return  ( ((PPN2 & 0x3FFFFFF)<<30) + ((PPN1 & 0x1FF)<<21) + ((PPN0 & 0x1FF)<<12) + (page_offset & 0xFFF) );
    }
    void write_reg(uint_t val){
        page_offset = val & 0xFFF;
        PPN0 = (val>>12) & 0x1FF;
        PPN1 = (val>>21) & 0x1FF;
        PPN2 = (val>>30) & 0x3FFFFFF;
    }
};

struct sv39pte_t{
    uint8_t D,A,G,U,X,W,R,V;
    uint8_t RSW ; uint16_t PPN0; uint16_t PPN1; uint32_t PPN2;
    uint_t PPN;
    sv39pte_t(){
        D = 0; A = 0; G = 0; U = 0; X = 0; W = 0; R = 0; V = 0; 
        RSW = 0;
        PPN0 = 0;
        PPN1 = 0;
        PPN2 = 0;
        PPN = 0;
    }
    uint_t read_reg(){
        return  ( ((PPN2 & 0x3FFFFFF)<<28) + ((PPN1 & 0x1FF)<<19) + ((PPN0 & 0x1FF)<<10) \
                + ((RSW & 0b11)<<8) + ((D & 0b1)<<7) + ((A & 0b1)<<6) \
                + ((G & 0b1)<<5) + ((U & 0b1)<<4) + ((X & 0b1)<<3) \
                + ((W & 0b1)<<2) + ((R & 0b1)<<1) + (V & 0b1) ) ;
    }
    void write_reg(uint_t val){
        D = (val>>7) & 0b1; A = (val>>6) & 0b1; G = (val>>5) & 0b1; 
        U = (val>>4) & 0b1; X = (val>>3) & 0b1; W = (val>>2) & 0b1; 
        R = (val>>1) & 0b1; V = val & 0b1; 
        RSW  = (val>> 8) & 0b11;
        PPN0 = (val>>10) & 0x1FF;
        PPN1 = (val>>19) & 0x1FF;
        PPN2 = (val>>28) & 0x3FFFFFF;
        PPN = (PPN2<<18) + (PPN1<<9) + PPN0;
    }
};

#define PAGESIZE 4096   //4KiB
#define LEVELS 3
#define PTESIZE 8

uint_t translate(uint_t virtual_addr, ttype_t translation_type, plevel_t current_privilage){

    uint_t physical_addr = 0;
    uint_t a = 0;
    sv39va_t vir_addr;
    sv39pa_t phy_addr;
    sv39pte_t pte2, pte1, pte0, pte_final;
    uint_t pte_addr;
    switch(satp.MODE){
        case 0 : //Bare
            physical_addr = virtual_addr;
            return physical_addr;
            break;
        case 8 : //Sv39
            if ((current_privilage == MMODE) & (mstatus.mprv == 0)){
                physical_addr = virtual_addr;
                return physical_addr;
            }
            else if ((current_privilage == MMODE) & (mstatus.mprv == 1) & (translation_type == INST)){
                physical_addr = virtual_addr;
                return physical_addr;
            }
            else if ( (mstatus.mprv == 1) & (mstatus.mpp ==3)){
                physical_addr = virtual_addr;
                return physical_addr;
            }

            vir_addr.write_reg(virtual_addr);

            a = satp.PPN * PAGESIZE;

            pte_addr = a + vir_addr.VPN2 * PTESIZE;

            if (pte_addr >= DRAM_BASE){ // check whether this is inside dram range
                pte_addr = pte_addr - DRAM_BASE;    // // mapping to emulator array memory
            }
            else {
                cout <<"ERROR : Looking for PTE outside RAM"<<endl;
            }

            
            
            pte2.write_reg(memory.at(pte_addr/8));
           
            if ( (pte2.V==0) | ( (pte2.R==0) & (pte2.W==1) ) ){ //page fault -validity check failed
                return -1;
            }

            if ( (pte2.R==1) | (pte2.X==1) ){ //pte is a leaf
                pte_final = pte2;
                phy_addr.page_offset = vir_addr.page_offset; ////superpage translation
                phy_addr.PPN2 = pte2.PPN2;
                phy_addr.PPN1 = vir_addr.VPN1;
                phy_addr.PPN0 = vir_addr.VPN0;
            }
            else{ //pte2 is a pointer to another pte
                a = pte2.PPN * PAGESIZE;
                pte_addr = a + vir_addr.VPN1 * PTESIZE;

                if (pte_addr >= DRAM_BASE){ // check whether this is inside dram range
                    pte_addr = pte_addr - DRAM_BASE;    // // mapping to emulator array memory
                }
                else {
                    cout <<"ERROR : Looking for PTE outside RAM"<<endl;
                }

                pte1.write_reg(memory.at(pte_addr/8));
                if ( (pte1.V==0) | ( (pte1.R==0) & (pte1.W==1) ) ){ //page fault -validity check failed
                    return -1;
                }

                if ( (pte1.R==1) | (pte1.X==1) ){ //pte is a leaf
                    pte_final = pte1;
                    phy_addr.page_offset = vir_addr.page_offset; ////superpage translation
                    phy_addr.PPN2 = pte1.PPN2;
                    phy_addr.PPN1 = pte1.PPN1;
                    phy_addr.PPN0 = vir_addr.VPN0;
                }
                else{
                    a = pte1.PPN * PAGESIZE;
                    pte_addr = a + vir_addr.VPN0 * PTESIZE;

                    if (pte_addr >= DRAM_BASE){ // check whether this is inside dram range
                        pte_addr = pte_addr - DRAM_BASE;    // // mapping to emulator array memory
                    }
                    else {
                        cout <<"ERROR : Looking for PTE outside RAM"<<endl;
                    }

                    pte0.write_reg(memory.at(pte_addr/8));
                    if ( (pte0.V==0) | ( (pte0.R==0) & (pte0.W==1) ) ){ //page fault -validity check failed

                        return -1;
                    }
                    if ( (pte0.R==1) | (pte0.X==1) ){ //pte is a leaf
                        pte_final = pte0;
                        phy_addr.page_offset = vir_addr.page_offset; 
                        phy_addr.PPN2 = pte0.PPN2;
                        phy_addr.PPN1 = pte0.PPN1;
                        phy_addr.PPN0 = pte0.PPN0;
                    }
                    else{ //page fault - last level pte is also not valid
                        return -1;
                    }
                }
            }

            if ( (translation_type==STOR) & ((pte_final.A==0) |  (pte_final.D==0) ) ){ // page fault according to spec - point 7
                 //cout<<"dirty bit not set"<<endl;
                return -1;
            } else if ((translation_type==INST|translation_type==LOAD) & (pte_final.A==0)){
                 //cout<<"access bit not set"<<endl;
                return -1;
            } 
            // else if ( (translation_type==LOAD) & (pte_final.R==0) ){
            //     return -1;
            // }
             else if ( (translation_type==STOR) & (pte_final.W==0) ){
                return -1;
            }
            return phy_addr.read_reg();

            // switch(translation_type){
            //     case INST : 
                    
            //         if (pte_final.X==0) // fetch page is not executable
            //             return -1;

            //         // if (current_privilage==UMODE){
            //         //     if (pte_final.U==0) // mode is U but access in not
            //         //         return -1;
            //         // }
            //         // else if (current_privilage==SMODE){
            //         //     if (pte_final.U==1)  // cant execute user pages in smode
            //         //         return -1;
            //         // }
            //         if(pte_final.A==0) 
            //             return -1;
            //         return phy_addr.read_reg();

            //         break;

            //     case LOAD :
                    
            //         if (pte_final.R==0 & (sstatus.mxr==0)) // leaf page is not readable
            //             {
            //                 // cout<<"not readable"<<endl;
            //                 return -1;
            //             }

            //         if (current_privilage==UMODE){
            //             if (pte_final.U==0) // mode is U but access in not
            //                   {
            //                 // cout<<"U readable"<<endl;
            //                 return -1;
            //             }
            //         }
            //         else if (current_privilage==SMODE){
            //             if ((pte_final.U==1) & (sstatus.sum==0))  
            //                   {
            //                 // cout<<"sum readable"<<endl;
            //                 return -1;
            //             }
            //         }

            //         // if ( (pte_final.X==1) & (sstatus.mxr==0) ) //load from executable pages are not enabled
            //         //         {
            //         //         cout<<"mxr readable"<<endl;
            //         //         return -1;
            //         //     }
            //         if(pte_final.A==0) 
            //                {
            //                 // cout<<"A readable"<<endl;
            //                 return -1;
            //             }

            //         return phy_addr.read_reg();
            //         break;

            //     case STOR : 

            //         if (pte_final.W==0) // leaf page is not writable
            //             return -1;
            //         if(pte_final.A==0) 
            //             return -1;
            //         if(pte_final.D==0) 
            //             return -1;
            //         return phy_addr.read_reg();
            //         break;
            // }

            break;
        case 9 : //Sv48
            cout << "Sv48 mode not implemented"<<endl;
			exit(0);
            break;
        default :
            cout << "Invalid or not-implemented translation mode : "<< satp.MODE <<endl;
            break;
    }

    return -1;
}
