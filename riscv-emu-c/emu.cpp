#include "emu.h"
#include "csr_file.h"
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include <opencv2/core.hpp>
#include <iostream>
#define echo
#ifdef TEMU
#include <fstream>
using std::ofstream;

BlockDevice drive1, *drive=&drive1;

char *fname;


VIRTIODevice *block_dev;

#endif
int kbhit(void)
{
#ifdef echo
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

#endif
  return 0;
  
}
uint32_t image[1920*1080];

void imfeed(int line_no,int pix_colum,int pix_color,char pixval,bool print){
    char new_pix_color = 0;
    if(pix_color==1)
        new_pix_color =1;
    if(pix_color==0)
        new_pix_color =2;
    if(pix_color==2)
        new_pix_color =0;
    unsigned char pix_value = pixval & 0xff;
    image[line_no*1920+pix_colum] = ((image[line_no*1920+pix_colum]& (~(0xff<<(new_pix_color*8))) ) | (pix_value<< (new_pix_color*8)));
    if(print)
        printf("%0x\n",image[line_no*1920+pix_colum]);
}
 int insno=0;
int writePPM(uint16_t width, uint16_t height, uint32_t *image)
{
  int i, j;
  char fn[32];
  sprintf(fn, "view.ppm");
  FILE *fp = fopen(fn, "wb");
  if (fp==NULL)
    return -1;
  fprintf(fp, "P6\n%d %d\n255\n", width, height);
  for (j=0; j<height; j++)
  {
    for (i=0; i<width; i++)
      fwrite((char *)(image + j*width + i), 1, 3, fp);
  }
  fclose(fp);
  return 0;
}

 void unset_echo() {
#ifdef echo
   struct termios term;
   tcgetattr(0, &term);
   term.c_lflag &= ~ECHO;
   tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
#endif
 }

int main(int argc, char** argv){
    ofstream outdata; // outdata is like cin

outdata.open("example2.dat"); 
  unset_echo();
    if (argc==2){
        
        char a[150] = "utils/elf2hex/elf2hex --bit-width 32 --input ";

        strcat(a ,argv[1]);

        strcat(a ," --output data_hex.txt");

        string ou = exec(a);

    }
    else if (argc>2){
        cout << "ERROR : Please provide the ELF image to run" << endl;
        cout << "USAGE : ./emu bbl" <<endl;
    }

    

#ifdef TEMU
    /////////////////////////////// tinyemu init begin //////////////////////////////////
    VirtMachine *s;
    const char *path, *cmdline, *build_preload_file;
    int c, option_index, ii, ram_size, accel_enable;
    BOOL allow_ctrlc;
    BlockDeviceModeEnum drive_mode;
    VirtMachineParams p_s, *p = &p_s;

    ram_size = -1;
    allow_ctrlc = FALSE;
    (void)allow_ctrlc;
    drive_mode = BF_MODE_SNAPSHOT;
    accel_enable = -1;
    cmdline = NULL;
    build_preload_file = NULL;

    allow_ctrlc = TRUE;
    drive_mode = BF_MODE_RW;
    cmdline = "";    // append argument 
    ram_size = 512;


    //path = argv[optind++];
    virt_machine_set_defaults(p);
    virt_machine_load_config_file(p, "./temu/riscv64.cfg", NULL, NULL);   // loading from config file commented, default config used

    if (ram_size > 0) {
        p->ram_size = (uint64_t)ram_size << 20;
    }
    if (accel_enable != -1)
        p->accel_enable = accel_enable;
    if (cmdline) {
        vm_add_cmdline(p, cmdline);
    }

    /* open the files & devices */
    printf("Drive count : %d\n",(uint32_t)(p->drive_count));
    for(ii = 0; ii < p->drive_count; ii++) {

        BlockDevice *drive;
        char *fname;
        fname = get_file_path(p->cfg_filename, p->tab_drive[ii].filename);
        {
            drive = block_device_init(fname, drive_mode);
            printf("block device init\n");
            
        }
        free(fname);
        p->tab_drive[ii].block_dev = drive;
    }

    for(int iii = 0; iii < p->fs_count; iii++) {
        FSDevice *fs;
        const char *path;
        path = p->tab_fs[iii].filename;
        {
            char *fname;
            fname = get_file_path(p->cfg_filename, path);
            fs = fs_disk_init(fname);
            if (!fs) {
                fprintf(stderr, "%s: must be a directory\n", fname);
                exit(1);
            }
            free(fname);
        }
        p->tab_fs[iii].fs_dev = fs;
    }

    p->console = console_init(allow_ctrlc);

    p->rtc_real_time = TRUE;

    IRQSignal plic_irq;

    irq_init(&plic_irq, plic_set_irq_emu, 1);

    VIRTIOBusDef vbus_sf;
    VIRTIOBusDef *vbus= &vbus_sf;

    memset(vbus, 0, sizeof(*vbus));

    //for(i = 0; i < p->drive_count; i++) {
    vbus->irq = &plic_irq;
    block_dev = virtio_block_init(vbus, p->tab_drive[0].block_dev);

        //irq_num++;
        //s->virtio_count++;
    
    
    virt_machine_free_config(p);
#endif
    /////////////////////////////// tinyemu init end //////////////////////////////////

    ifstream infile("data_hex.txt");
    string line;

    uint_t i = 0;

    uint_t PC = DRAM_BASE;
    uint_t PC_phy = 0;
    uint_t instruction = 0;
    bool branch = false;
    uint_t load_data = 0;
    uint_t store_data = 0;
    uint_t store_addr = 0;
    uint_t store_addr_phy = 0;
    uint_t load_addr = 0;
    uint_t load_addr_phy = 0;
    uint_t val = 0;
    uint_t itr = 0;

    uint_t cycle_count = 0;

    __uint128_t instr_count = 0;
    __uint128_t mult_temp=0;

    uint_t wb_data  = 0;
    uint_t ret_data = 0;
    uint_t lPC      = 0;
    uint_t rd       = 0;
    uint_t func3    = 0;
    uint_t rs1      = 0;
    uint_t rs2      = 0;
    uint_t func7    = 0;
    uint_t imm11_0  = 0;
    uint_t imm31_12 = 0;
    uint_t imm_j    = 0;
    uint_t imm_b    = 0;
    uint_t imm_s    = 0;
    uint_t csr_data = 0;
    plevel_t LR_cp;
    uint_t LR_count = 0;
    bool csr_bool = false;

    queue<int> fifo_queue; 

    uint_t load_addr_phy_virt = 0;

    bool write_tval = false;

    plevel_t LR_cp64;
    uint_t LR_count64 = 0;

    uint_t amo_op   = 0;
    bool amo_reserve_valid = false;
    bool amo_reserve_valid64 = false;
    uint_t amo_reserve_addr = 0;
    uint_t amo_reserve_addr64 = 0;


    bool ls_success = false;

    misa = 0b101000001000100000001;
    misa = (misa | (0b1llu<<63));

    enum opcode_t opcode;

    uint_t temp;

    while (std::getline(infile, line)) {            // Initialize memory with instructions
        temp = stoul(line,nullptr,16);
        if (i%2==0)
            memory.at(i/2) = temp;
        else
            memory.at(i/2) = ((memory.at(i/2)) | ((temp)<<32));
        i+=1;
    }
    #ifdef DEBUG
        printf("Mem load successful\n");
    #endif

    mstatus.fs = 1;

    struct timeval tv;
    gettimeofday(&tv,NULL);
    uint_t time_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;

    uint_t testval_pre;
    uint_t testval_pos;

    memory.at(MTIME_ADDR/8) = 0;
    memory.at(MTIMECMP_ADDR/8) = -1;
    uint32_t imm=0;
    time_csr = (uint_t)time(NULL);

    mhartid = 0;

    auto start = chrono::steady_clock::now();
    
        for(int i = 0;i<1920*1080;i++) {
                image[i]=0;
          }
    while (1){


        cycle_count += 1;
        cycle  = cycle_count ;
        instret  = cycle ;
        gettimeofday(&tv,NULL);
        time_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;
        mtime = (uint_t)(time_in_micros*10);
        time_csr = mtime;

        // cout << "PC       : "<<hex<<PC<<endl;
        // cout << "mtime    : "<<mtime<<endl;
        // cout << "mtimecmp : "<<mtimecmp<<endl;
        // cout << "mie.STIE : "<<(uint_t)mie.STIE<<endl;
        // cout << "mip.STIP : "<<(uint_t)mip.STIP<<endl;
        


        PC_phy = PC;

		if(cp ==0 & PC==0  ) 
		{
			cout <<"instruction " << hex<<instruction<<endl;
			exit(0);
		}
		else if (cp ==0 & PC_phy != -1 ) 
		{
			
		}


        if (PC_phy==-1){

    		mtval = PC;
            PC = excep_function(PC+4,CAUSE_FETCH_PAGE_FAULT,CAUSE_FETCH_PAGE_FAULT,CAUSE_FETCH_PAGE_FAULT,cp);
            
            utval=mtval;
            stval=mtval; 
            
        	continue;
        }



        if (PC_phy >= DRAM_BASE & PC_phy<(DRAM_BASE+0x9000000)){ // mapping to RAM
            PC_phy = PC_phy - DRAM_BASE; // mapping to emulator array memory
        }
        else{ // mapping to peripheral
            cout << "peripheral access PC :"<< hex << PC_phy << endl;
            break;
        }


        instruction = getINST(PC_phy/4,&memory);
        //printf("%0x %0x\n",PC_phy,instruction);


        reg_file[0] = 0;

        #ifdef DEBUG
            bitset<32> ins(instruction);
            cout << "Instruction : "<<ins << endl;
        #endif


        opcode = static_cast<opcode_t>((instruction) & 0b1111111);


        wb_data = 0;

        rd      = ((instruction) >> 7 ) & 0b11111   ;
        func3   = ((instruction) >> 12) & 0b111     ;
        rs1     = ((instruction) >> 15) & 0b11111   ;
        rs2     = ((instruction) >> 20) & 0b11111   ;
        func7   = ((instruction) >> 25) & 0b1111111 ;

        imm11_0  = ((instruction) >> 20) & 0b111111111111 ;
        imm31_12 = ((instruction) >> 12) & 0xFFFFF ;       // extract 20 bits

        imm_j    = ((((instruction)>>31) & 0b1)<<20) + ((instruction) & (0b11111111<<12)) + ((((instruction)>>20) & 0b1)<<11) + ((((instruction)>>21) & 0b1111111111)<<1); //((instruction>>31) & 0b1)<<20 + (instruction & (0b11111111<<12)) + ((instruction>>20) & 0b1)<<11 +
        imm_b    = ((((instruction)>>31) & 0b1)<<12) + ((((instruction)>>7) & 0b1)<<11) + ((((instruction)>>25) & 0b111111)<<5) + (((instruction)>>7) & 0b11110) ;
        imm_s    = ((((instruction)>>25) & 0b1111111)<<5) + (((instruction)>>7) & 0b11111) ;
        imm = rd | ((instruction >> (25 - 5)) & 0xfe0);
        imm = (imm << 20) >> 20;

 
           
        amo_op   = ((instruction) >> 27) & 0b11111 ;


        if (amo_reserve_valid64){
            LR_count64 +=1;
            // if ( (opcode==jump) | (opcode==jumpr) | (opcode==cjump) | (opcode==load) | (opcode==store) | (opcode==fence) | (opcode==systm) ){
            //     cout << "Illegal instruction in between LR/SC 64 : " << (uint_t)opcode << endl;
            //     amo_reserve_valid64 = false;
            //     amo_reserve_addr64 = 0;
            //     LR_count64 = 0;
            // }
            // else if (LR_cp64 != cp){
            //     cout << "Privilege changed in between LR/SC 64 : " << endl;
            //     amo_reserve_valid64 = false;
            //     amo_reserve_addr64 = 0;
            //     LR_count64 = 0;
            // }
        }

        if (amo_reserve_valid){
            LR_count +=1;
        //     if ( (opcode==jump) | (opcode==jumpr) | (opcode==cjump) | (opcode==load) | (opcode==store) | (opcode==fence) | (opcode==systm) ){
        //         cout << "Illegal instruction in between LR/SC : " << (uint_t)opcode << endl;
        //         amo_reserve_valid = false;
        //         amo_reserve_addr = 0;
        //         LR_count = 0;
        //     }
        //     else if (LR_cp != cp){
        //         cout << "Privilege changed in between LR/SC : " << endl;
        //         amo_reserve_valid = false;
        //         amo_reserve_addr = 0;
        //         LR_count = 0;
        //     }
        }

        if ((PC%4)!=0){
            cout << "PC mis aligned "<<hex<<PC <<endl;
        }
        lPC = PC;
        PC += 4;
        switch(opcode){
            case lui :
                #ifdef DEBUG
                    printf("LUI\n");
                #endif
                //cout << "Instruction : "<<instruction << endl;
                wb_data = sign_extend<uint_t>((instruction & 0xFFFFF000),32);
                //cout << wb_data << endl;
                reg_file[rd] = wb_data;
                break;

            case auipc :
                #ifdef DEBUG
                    printf("AUIPC\n");
                #endif
                wb_data = (PC-4) + sign_extend<uint_t>(((imm31_12) << 12),32);
                reg_file[rd] = wb_data;
                //cout << "AUIPC : "<<hex<<wb_data<<endl;
                break;

            case jump :
                #ifdef DEBUG
                    cout << "JUMP - JAL" <<endl;
                #endif
                wb_data = PC;
                PC = (PC-4) + sign_extend<uint_t>(imm_j,21); //21 bits sign extend
                reg_file[rd] = wb_data ;// PC + 4 to rd
                break;

            case jumpr :
                #ifdef DEBUG
                    cout << "JUMPR - JALR" <<endl;
                #endif
                wb_data = PC;
                PC = (reg_file[rs1] + sign_extend<uint_t>(imm11_0,12)) & 0xFFFFFFFFFFFFFFFE; //setting LSB to 0 as spec page 20
                reg_file[rd] = wb_data;// PC + 4 to rd 
                break;

            case cjump :
                #ifdef DEBUG
                    printf("CJUMP\n");
                #endif
                switch(func3){
                    case 0b000 : 
                        //cout << "BEQ : "<<reg_file[rs1]<<" : "<<reg_file[rs2]<<endl;
                        branch = (reg_file[rs1] == reg_file[rs2]); 
                        break; //BEQ

                    case 0b001 : branch = (reg_file[rs1] != reg_file[rs2]); break; //BNE

                    case 0b100 : branch = (signed_value(reg_file[rs1]) < signed_value(reg_file[rs2])); break; //BLT

                    case 0b101 : branch = (signed_value(reg_file[rs1]) >= signed_value(reg_file[rs2])); break; //BGE

                    case 0b110 : branch = (reg_file[rs1] < reg_file[rs2]); break; //BLTU

                    case 0b111 : branch = (reg_file[rs1] >= reg_file[rs2]); break; //BGEU

                    default : printf("******INVALID INSTRUCTION******\nINS :%lu\nOPCODE :%lu\n",instruction,instruction & 0b1111111);
                        break;
                }
                if (branch==true)
                    PC = PC - 4 + sign_extend<uint_t>(imm_b,13);
                break;

            case load :
                #ifdef DEBUG
                    printf("LOAD\n");
                #endif
                load_addr = reg_file[rs1] + sign_extend<uint_t>(imm11_0,12);


                if ((load_addr != FIFO_ADDR_RX) && ((load_addr != FIFO_ADDR_TX))){
                    {
                        if(load_addr==0){
                            //printf("null pointer exception \n");
                           // exit(0);
                        }

                        load_addr_phy = load_addr;

                        if (load_addr_phy==-1){
                            mtval = load_addr;
                             
                            PC = excep_function(PC,CAUSE_LOAD_PAGE_FAULT,CAUSE_LOAD_PAGE_FAULT,CAUSE_LOAD_PAGE_FAULT,cp);
                            switch(cp){
                                case MMODE : 
                                mtval = mtval;
                                break;
                                case SMODE :
                                stval = mtval;
                                mtval = 0;
                                break;
                                case UMODE :
                                utval = mtval;
                                mtval = 0;
                                break;
                            }
                            continue;
                        }

                        if ((load_addr_phy >= DRAM_BASE) & (load_addr_phy<= (DRAM_BASE+0x9000000))){ // mapping to RAM
                            load_addr_phy = load_addr_phy - DRAM_BASE; // mapping to emulator array memory

                            if (load_addr_phy > 0x9000000){
                                cout << "ERROR : Exceeds RAM limit" << endl;
                                exit(0);
                            }
                        load_data = memory.at(load_addr_phy/8);
                      
//                            printf("tranlstation %0lx %0lx, %0lx\n",load_addr, load_addr_phy,load_data);

                        }
                        else{ // mapping to peripheral
                           // printf("peripheral %0x\n",load_addr_phy);

                            if ((load_addr_phy >= CLINT_BASE) & (load_addr_phy <= (CLINT_BASE+CLINT_SIZE))){
                                load_data = clint_read(load_addr_phy-CLINT_BASE);
                            }
                            else if ((load_addr_phy >= PLIC_BASE) & (load_addr_phy <= (PLIC_BASE+PLIC_SIZE))){
                                load_data = ((uint_t)plic_read(load_addr_phy-PLIC_BASE))<<(32*((load_addr_phy%8)!=0));
                            }
#ifdef TEMU
                            else if ((load_addr_phy >= VIRTIO_BASE) & (load_addr_phy <= (VIRTIO_BASE+VIRTIO_SIZE))){
                                load_addr_phy_virt = load_addr_phy/8;
                                load_addr_phy_virt = load_addr_phy_virt *8;
                                load_data =( (uint_t)virtio_mmio_read(block_dev,load_addr_phy_virt -VIRTIO_BASE+4,2)<<32)+ (virtio_mmio_read (block_dev,load_addr_phy_virt -VIRTIO_BASE,2));
                            }
#endif
                            else {
                                           load_data=0x1000;
                                //cout << "New peripheral load"<< hex << load_addr_phy<<"val"<<load_addr<<endl;
                                //exit(0);
                            }
                        }

                        switch(func3){
                            case 0b000 : 
                                if (!load_byte(load_addr_phy,load_data, wb_data)){
                                    LD_ADDR_MISSALIG = true;
                                    mtval = load_addr;                                  
                                } else {
                                    wb_data = sign_extend<uint_t>(wb_data & (0xFF)      , 8); 
                                    reg_file[rd] = wb_data;
                                }
                                break; //LB sign extend  8 bit value

                            case 0b001 : 
                                if (!load_halfw(load_addr_phy,load_data, wb_data)){
                                    LD_ADDR_MISSALIG = true;
                                    mtval = load_addr;
                                } else {
                                    wb_data = sign_extend<uint_t>(wb_data & (0xFFFF)    ,16); 
                                    reg_file[rd] = wb_data;
                                }
                                break; //LH sign extend 16 bit value

                            case 0b010 : 
                                if (!load_word(load_addr_phy,load_data, wb_data)){
                                    LD_ADDR_MISSALIG = true;
                                    mtval = load_addr;
                                    //cout << "LW"<<endl;
                                } else {
                                    wb_data = sign_extend<uint_t>(wb_data & (0xFFFFFFFF),32); 
                                    reg_file[rd] = wb_data;
                                }
                                break; //LW sign extend 32 bit value

                            case 0b100 : 
                                if (!load_byte(load_addr_phy,load_data, wb_data)){
                                    LD_ADDR_MISSALIG = true;
                                    mtval = load_addr;
                                    //cout << "LBU" <<hex<<load_addr_phy<<endl;
                                } else {
                                    wb_data = wb_data & 0xFF      ; 
                                    reg_file[rd] = wb_data;
                                }
                                break; //LBU zero extend  8 bit value

                            case 0b101 : 
                                if (!load_halfw(load_addr_phy,load_data, wb_data)){
                                    LD_ADDR_MISSALIG = true;
                                    mtval = load_addr;
                                    //cout << "LHU"<<endl;
                                } else {
                                    wb_data = wb_data & 0xFFFF    ; 
                                    reg_file[rd] = wb_data;
                                }
                                break; //LHU zero extend 16 bit value

                            case 0b110 : 
                                if (!load_word(load_addr_phy,load_data, wb_data)){
                                    LD_ADDR_MISSALIG = true;
                                    mtval = load_addr;
                                    //cout << "LWU"<<endl;
                                } else {
                                    wb_data = wb_data & 0xFFFFFFFF; 
                                    reg_file[rd] = wb_data;
                                }
                                break; //LWU zero extend 32 bit value

                            case 0b011 : 
                                if ((load_addr_phy%8)==0){
                                    wb_data = load_data ; 
                                    reg_file[rd] = wb_data;
                                    
                                } else {
                                    LD_ADDR_MISSALIG = true;
                                    mtval = load_addr;
                                    //cout << "LD"<<endl;
                                }
                                break;//LD

                            default : printf("******INVALID INSTRUCTION******\nINS :%lu\nOPCODE :%lu\n",instruction,instruction & 0b1111111);
                                bitset<3> ins(func3);
                                cout<<  "func3 : "<<ins<<endl;
                                break;
                        }

                    }
                } 
                else if ( load_addr == FIFO_ADDR_RX ) {
                    if(kbhit()){
                        wb_data=0;
                        }
                    else wb_data=2;
                //     printf("waiting 1\n");
                //    int c =getchar();
                //     printf("waiting 2\n");
                //    
                //    if (c != EOF){
                //        wb_data = 0 ;
                //        fifo_queue.push(c);
                //    }
                //    else
                //     wb_data = 2 ;

                    reg_file[rd] = wb_data;
                }
                else if ( load_addr == FIFO_ADDR_TX ){
                    // if(kbhit())
                    wb_data=getchar();
                    if(wb_data==41){
                       cout<<"screenshot saved"<<endl;
                       writePPM(1920,1080,(uint32_t*)&image) ;
                    }
                    //if (fifo_queue.empty()){
                    //    wb_data = (uint_t)-1 ;
                    //}
                    //else{
                    //    wb_data = fifo_queue.front();
                    //    fifo_queue.pop();
                    //}
                    reg_file[rd] = wb_data;

                }
                else {
                    printf("unknow address %0lx\n",load_addr);
                }
                break;

            case store :
                #ifdef DEBUG
                    printf("STORE\n");
                #endif
                store_addr = reg_file[rs1] + sign_extend<uint_t>(imm_s,12);

 
                if (store_addr != FIFO_ADDR_TX){                                 //& (store_addr != MTIME_ADDR) & (store_addr != MTIMECMP_ADDR)

                    store_addr_phy = store_addr;

                    if (store_addr_phy==-1){

                            PC = excep_function(PC,CAUSE_STORE_PAGE_FAULT,CAUSE_STORE_PAGE_FAULT,CAUSE_STORE_PAGE_FAULT,cp);

                            mtval = store_addr;
                            switch(cp){
                                    case MMODE : 
                                        mtval = mtval;
                                        break;
                                    case SMODE :
                                        stval = mtval;
                                        mtval = 0;
                                        break;
                                    case UMODE :
                                        utval = mtval;
                                        mtval = 0;
                                        break;
                                }

                            continue;
                        }

                    if ((store_addr_phy >= DRAM_BASE) & (store_addr_phy< (DRAM_BASE+0x9000000)) ){ // mapping to RAM
                        store_addr_phy = store_addr_phy - DRAM_BASE; // mapping to emulator array memory
                          if (store_addr_phy >= (0x9000000)){
                                cout << "Physical memory limit exceeded : "<<hex<<store_addr_phy+DRAM_BASE<<endl;
                        printf("dram size %lx\n", 0x9000000);
                                cout << "PC : "<<hex<<PC_phy<<endl;
                            }else{
                                
                                store_data = memory.at(store_addr_phy/8);


                                switch(func3){                                                      // Setting lower n bits to 0 and adding storing value
                                    case 0b000 :
                                        val = reg_file[rs2] & 0xFF;
                                        ls_success = store_byte(store_addr_phy,store_data,val, wb_data);
                                        break;//SB  setting LSB 8 bit

                                    case 0b001 :
                                        val = reg_file[rs2] & 0xFFFF;
                                        ls_success = store_halfw(store_addr_phy,store_data,val, wb_data);
                                        break;//SH setting LSB 16 bit value

                                    case 0b010 :
                                        val = reg_file[rs2] & 0xFFFFFFFF;
                                        ls_success = store_word(store_addr_phy,store_data,val, wb_data);
                                        break;//SW setting LSB 32 bit value

                                    case 0b011 : 
                                        if ((store_addr_phy%8)==0){
                                            wb_data = reg_file[rs2] ; 
                                            ls_success = true;
                                        }else{
                                            ls_success = false;
                                        }
                                        break; //SD

                                    default : printf("******INVALID INSTRUCTION******\nINS :%lu\nOPCODE :%lu\n",instruction,instruction & 0b1111111);
                                    bitset<3> ins(func3);
                                    cout<<  "func3 : "<<func3<<endl;
                                    break;
                                }
                                if (!ls_success){
                                    //cout << "Mis-aligned store exception"<<endl;
                                    //PC = excep_function(PC,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,cp);
                                }else {
                                    
                                    memory.at(store_addr_phy/8) = wb_data;
                                }
                            }
                    }
                    else{ // mapping to peripheral

                        if ((store_addr_phy >= CLINT_BASE) & (store_addr_phy <= (CLINT_BASE+CLINT_SIZE))){
                            clint_write(store_addr_phy-CLINT_BASE, reg_file[rs2]);
                        }
                        else if ((store_addr_phy >= PLIC_BASE) & (store_addr_phy <= (PLIC_BASE+PLIC_SIZE))){
                            plic_write(store_addr_phy-PLIC_BASE, reg_file[rs2]);
                        }
#ifdef TEMU
                        else if ((store_addr_phy >= VIRTIO_BASE) & (store_addr_phy <= (VIRTIO_BASE+VIRTIO_SIZE))){
                            virtio_mmio_write(block_dev, store_addr_phy-VIRTIO_BASE, (reg_file[rs2] & 0xFFFFFFFF), 2);
                        }
#endif
                        else {
                            if(store_addr_phy == FIFO_ADDR_TX){
                                #ifdef DEBUG
                                    printf("STORE2\n");
                                #endif
                                cout << (char)reg_file[rs2]<<std::flush;
                            }
                            if((store_addr_phy >= 0x19000000) & store_addr_phy<0x20000000){
                                  int kk = store_addr_phy - 0x19000000;
                                  int byte_start = kk;
                                  char pixval; 
                                 if(func3==3){
                                  for(int i =byte_start; i<(byte_start+8);i++) {
                                        int line_no = i/(1920*4); int colum_byte = i%(1920*4);
                                        int pix_colum = colum_byte/3;
                                        int pix_color = colum_byte%3;
                                        bool print =0;

                                        pixval=(reg_file[rs2] >> ((i-byte_start)*8)) & 0xff;
                                        //if(reg_file[rs2]!=0){
                                         //   print = 1;
                                            //printf("address %0x byte_start %0d, line_no %0d, pix_colum %0d,colum_byte %0d, pix_color%0d pixval %0d func %0d\n",store_addr_phy,byte_start,line_no,pix_colum,colum_byte,pix_color,pixval,func3);
                                        //}
                                        //if(reg_file[rs2]!=0)
                                        //    printf("feeding\n");
                                        imfeed(line_no,pix_colum,pix_color, pixval, print);
                                  }
                                }
                                else if(func3==2){
                                  for(int i =byte_start; i<(byte_start+4);i++) {
                                        int line_no = i/(1920*4); 
                                        int colum_byte = i%(1920*4);
                                        int pix_colum = colum_byte/3;
                                        int pix_color = colum_byte%3;
                                        bool print =0;

                                        pixval=((reg_file[rs2] & 0xFFFFFFFF) >> ((i-byte_start)*8)) & 0xff;
                                        //if(reg_file[rs2]!=0){
                                         //   print = 1;
                                            //printf("address %0x byte_start %0d, line_no %0d, pix_colum %0d,colum_byte %0d, pix_color%0d pixval %0d func %0d\n",store_addr_phy,byte_start,line_no,pix_colum,colum_byte,pix_color,pixval,func3);
                                        //}
                                        //if(reg_file[rs2]!=0)
                                        //    printf("feeding\n");
                                        imfeed(line_no,pix_colum,pix_color, pixval, print);
                                  }
                                    
                                }
                            }
                            else{
                            }
                            //exit(0);
                        }
                    }

                } 
                else if(store_addr == FIFO_ADDR_TX){
                    #ifdef DEBUG
                        printf("STORE2\n");
                    #endif
                    cout << (char)reg_file[rs2]<<std::flush;
                }
                break;
            
            case iops  :
                #ifdef DEBUG
                    printf("IOPS %lu\n",imm11_0);
                #endif
                switch(func3){
                    case 0b000 :
                        wb_data = reg_file[rs1] + sign_extend<uint_t>(imm11_0,12); //ADDI
                        break;

                    case 0b010 :
                        wb_data = (signed_value(reg_file[rs1]) < signed_value(sign_extend<uint_t>(imm11_0,12))) ? 1 : 0; //SLTI
                        break;

                    case 0b011 :
                        wb_data = (reg_file[rs1] < sign_extend<uint_t>(imm11_0,12)) ? 1 : 0; //SLTIU
                        break;

                    case 0b111 :
                        wb_data = reg_file[rs1] & sign_extend<uint_t>(imm11_0,12); //ANDI
                        break;

                    case 0b110 :
                        wb_data = reg_file[rs1] | sign_extend<uint_t>(imm11_0,12); //ORI
                        break;

                    case 0b100 :
                        wb_data = reg_file[rs1] ^ sign_extend<uint_t>(imm11_0,12); //XORI
                        break;

                    case 0b001 :
                        wb_data = reg_file[rs1] << (imm11_0 & 0b111111); //SLLI
                        break;

                    case 0b101 :
                        if (((func7)>>1) == 0b000000)
                            wb_data = reg_file[rs1] >> ((imm11_0) & 0b111111); //SRLI
                        else if (((func7)>>1)== 0b010000){
                            wb_data = reg_file[rs1];
                            for (itr=0;itr<((imm11_0) & 0b111111);itr++){
                                wb_data = (wb_data & (1llu<<63)) | ((wb_data) >> (1)); //SRAI
                            }
                        }
                        break;
                    default :
                        printf("******INVALID INSTRUCTION******\nINS :%lu\nOPCODE :%lu\n",instruction,instruction & 0b1111111);
                        bitset<3> ins(func3);
                        cout<<  "func3 : "<<ins<<endl;
                        break;
                }
                reg_file[rd] = wb_data;
                break;

            case iops64 :
                #ifdef DEBUG
                    printf("IOPS64 %lu\n",imm11_0);
                #endif
                switch(func3){
                    case 0b000 :  //ADDIW
                        wb_data = sign_extend<uint_t>(MASK32 & (reg_file[rs1] + sign_extend<uint_t>(imm11_0,12)),32);
                        break;

                    case 0b001 : //SLLIW
                        wb_data = sign_extend<uint_t>(MASK32 & (reg_file[rs1] << (imm11_0 & 0b11111)),32);
                        break;

                    case 0b101 :
                        if (((func7)>>1) == 0b000000) //SRLIW
                            wb_data = sign_extend<uint_t>(MASK32 & ((reg_file[rs1] & MASK32) >> (imm11_0 & 0b11111)),32);
                        else if (((func7)>>1)== 0b010000){//SRAIW
                            wb_data = reg_file[rs1] & MASK32;
                            for (itr=0;itr<((imm11_0) & 0b11111);itr++){
                                wb_data = ((wb_data & ((1llu)<<31)) | ((wb_data) >> (1)));
                            }
                            wb_data = sign_extend<uint_t>(MASK32 & wb_data, 32);
                        }
                        break;
                }
                reg_file[rd] = wb_data;
                break;

            case rops :
                if (func7 == 0b0000001){
                    #ifdef DEBUG
                        printf("ROPS\n");
                    #endif
                    switch (func3) {
                        case 0b000 : //MUL
                            mult_temp = reg_file[rs1] * reg_file[rs2];
                            reg_file[rd] = (mult_temp) & MASK64;
                            break;

                        case 0b001 : //MULH
                            mult_temp = ((__uint128_t)signed_value(reg_file[rs1]) * (__uint128_t)signed_value(reg_file[rs2]));
                            reg_file[rd] = ((mult_temp) >> 64) & MASK64;
                            break;

                        case 0b010 : //MULHSU
                            mult_temp = ((__uint128_t)signed_value(reg_file[rs1]) * (__uint128_t)reg_file[rs2]);
                            reg_file[rd] = ((mult_temp) >> 64) & MASK64;
                            break;

                        case 0b011 : //MULHU
                            mult_temp = (__uint128_t)reg_file[rs1] * (__uint128_t)reg_file[rs2];
                            reg_file[rd] = ((mult_temp) >> 64) & MASK64;
                            break;

                        case 0b100 : //DIV
                            reg_file[rd] = (uint_t)divi<int64_t>(signed_value(reg_file[rs1]), signed_value(reg_file[rs2]),0);

                            break;

                        case 0b101 : //DIVU
                            reg_file[rd] = divi<uint_t>(reg_file[rs1], reg_file[rs2],1);
                            break;

                        case 0b110 : //REM
                            reg_file[rd] = (uint_t)divi<int64_t>(signed_value(reg_file[rs1]), signed_value(reg_file[rs2]),2);
                            break;

                        case 0b111 : //REMU
                            reg_file[rd] = divi<uint_t>(reg_file[rs1], reg_file[rs2],3);
                            break;
                    }
                } else if (func7 == 0b0000000){
                    #ifdef DEBUG
                        printf("RROPS1\n");
                    #endif
                    switch(func3){
                        case 0b000 :
                            wb_data = reg_file[rs1] + reg_file[rs2]; //ADD
                            break;
                        case 0b010 :
                            wb_data = (signed_value(reg_file[rs1]) < signed_value(reg_file[rs2])) ? 1 : 0; //SLT
                            break;

                        case 0b011 :
                            wb_data = (reg_file[rs1] < reg_file[rs2]) ? 1 : 0; //SLTU
                            break;

                        case 0b111 :
                            wb_data = reg_file[rs1] & reg_file[rs2]; //AND
                            break;

                        case 0b110 :
                            wb_data = reg_file[rs1] | reg_file[rs2]; //OR
                            break;

                        case 0b100 :
                            wb_data = reg_file[rs1] ^ reg_file[rs2]; //XOR
                            break;

                        case 0b001 :
                            wb_data = ((reg_file[rs1]) << (reg_file[rs2] & 0b111111)); //SLL
                            break;

                        case 0b101 :
                            wb_data = reg_file[rs1] >> (reg_file[rs2] & 0b111111); //SRL
                            break;
                        default :
                            printf("******INVALID INSTRUCTION******\nINS :%lu\nOPCODE :%lu\n",instruction,instruction & 0b1111111);
                            bitset<3> ins(func3);
                            cout<<  "func3 : "<<ins<<endl;
                            break;

                    }
                    reg_file[rd] = wb_data;

                } else if (func7 == 0b0100000){
                    #ifdef DEBUG
                        printf("RROPS2\n");
                    #endif
                    switch(func3){
                        case 0b000 : //SUB
                            wb_data = reg_file[rs1] - reg_file[rs2];
                            break;

                        case 0b101 : //SRA
                            wb_data = reg_file[rs1];
                            for (itr=0;itr<(reg_file[rs2] & 0b111111);itr++){
                                wb_data = (( wb_data & ((1llu)<<63)) | ((wb_data) >> (1)));
                            }
                            break;
                    }
                    reg_file[rd] = wb_data;
                }
                break;

            case rops64 :
                if (func7 == 0b0000001){
                    #ifdef DEBUG
                        printf("ROPS64\n");
                    #endif
                    switch (func3) {
                        case 0b000 : //MULW
                            wb_data = sign_extend<uint_t>(((reg_file[rs1] & MASK32) * (reg_file[rs2] & MASK32)) & MASK32, 32) ;
                            break;
                        case 0b100 : //DIVW
                            wb_data = sign_extend<uint_t>(MASK32 & ((uint_t)divi32<int32_t>(signed_value32(reg_file[rs1] & MASK32), signed_value32(reg_file[rs2] & MASK32),0)),32);
                            break;

                        case 0b101 : //DIVUW
                            wb_data = sign_extend<uint_t>((uint_t)divi32<uint32_t>(reg_file[rs1], reg_file[rs2],1),32);
                            break;

                        case 0b110 : //REMW
                            wb_data = sign_extend<uint_t>(((uint_t)divi32<int32_t>(signed_value32(reg_file[rs1]), signed_value32(reg_file[rs2]),2) & MASK32 ),32);
                            break;

                        case 0b111 : //REMUW
                            wb_data = sign_extend<uint_t>((uint_t)divi32<uint32_t>(reg_file[rs1], reg_file[rs2],3),32);
                            break;
                    }
                    reg_file[rd] = wb_data;
                }
                else if (func7 == 0b0000000){
                    #ifdef DEBUG
                        printf("ROPS64-1\n");
                    #endif
                    switch(func3) {
                        case 0b000 : //ADDW
                            wb_data = sign_extend<uint_t>(((reg_file[rs1] + reg_file[rs2]) & MASK32), 32);
                            break;
                        case 0b001 : //SLLW
                            wb_data = sign_extend<uint_t>(((reg_file[rs1]) << ((reg_file[rs2]) & 0b11111)) & MASK32,32);
                            break;
                        case 0b101 : //SRLW
                            wb_data = sign_extend<uint_t>((reg_file[rs1] & MASK32) >> ((reg_file[rs2]) & 0b11111),32);
                            break;
                    }
                    reg_file[rd] = wb_data ;
                }
                else if (func7 == 0b0100000){
                    #ifdef DEBUG
                        printf("ROPS64-2\n");
                    #endif
                    switch(func3) {
                        case 0b000 : //SUBW
                            wb_data = sign_extend<uint_t>((((reg_file[rs1] & MASK32) - (reg_file[rs2] & MASK32)) & MASK32), 32);
                            break;
                        case 0b101 : //SRAW
                            wb_data = reg_file[rs1] & MASK32;
                            for (itr=0;itr<(reg_file[rs2] & 0b11111);itr++){
                                wb_data = ((wb_data & (1llu<<31)) | ((wb_data) >> (1)));
                            }
                            wb_data = sign_extend<uint_t>(wb_data, 32);
                            break;
                    }
                    reg_file[rd] = wb_data ;
                }
                break;

            case amo :
               
                if (func3 == 0b010) { //AMO.W-32
                    #ifdef DEBUG
                        printf("AMO 32\n");
                    #endif
                    load_addr = reg_file[rs1];
                    load_addr_phy = load_addr;

                    if (load_addr_phy==-1){
                            //cout << "Page fault exception store"<<endl;
                            PC = excep_function(PC,CAUSE_STORE_PAGE_FAULT,CAUSE_STORE_PAGE_FAULT,CAUSE_STORE_PAGE_FAULT,cp);
                            // STORE_PAGE_FAULT = true;
                            mtval = load_addr;
                            switch(cp){
                                    case MMODE : 
                                        mtval = mtval;
                                        break;
                                    case SMODE :
                                        stval = mtval;
                                        mtval = 0;
                                        break;
                                    case UMODE :
                                        utval = mtval;
                                        mtval = 0;
                                        break;
                                }

                            continue;
                        }
                    if(load_addr_phy>DRAM_BASE & load_addr_phy<DRAM_BASE+0x9000000) {
                        
                        load_addr_phy = load_addr_phy -DRAM_BASE;
                    }
                    else {
                        printf("illegal access %x %x\n",load_addr_phy,load_addr);
                        exit(0);
                    }
                    load_data = memory.at(load_addr_phy/8);
                    switch (amo_op){
                        case 0b00010 : //LR.W
                            
                            ls_success = load_word(load_addr_phy,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-LR.W : Mis-aligned memory access" << endl;
                                STORE_ADDR_MISSALIG = true;
                                mtval = load_addr;
                            }
                            else {
                                ret_data = sign_extend<uint_t>(wb_data & MASK32,32);
                                amo_reserve_valid = true;
                                amo_reserve_addr = load_addr;
                                LR_count = 0;
                                LR_cp = cp;
                            }
                            break;

                        case 0b00011 : //SC.W
                            if (amo_reserve_valid && (reg_file[rs1]==amo_reserve_addr)){
                                store_data = reg_file[rs2] & MASK32;
                                

                                ls_success = store_word(load_addr_phy, load_data, store_data, wb_data);
                                if (!ls_success){
                                    cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                    STORE_ADDR_MISSALIG = true;
                                    mtval = load_addr;
                                    ret_data = 1;
                                }
                                else {
                                    memory.at(load_addr_phy/8) = wb_data;
                                    ret_data = 0;
                                }
                            }
                            else {
                                ret_data = 1;
                            }
                            LR_count = 0;
                            amo_reserve_addr = 0;
                            amo_reserve_valid = false;
                            break;

                        case 0b00001 : //AMOSWAP.W
                            
                            ls_success = load_word(load_addr_phy,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = sign_extend<uint_t>((wb_data & MASK32),32);
                                wb_data = reg_file[rs2] & MASK32;
                                ls_success = store_word(load_addr,load_data,wb_data, store_data);
                                memory.at(load_addr_phy/8) = store_data;
                            }
                            break;

                        case 0b00000 : //AMOADD.W
                            
                            ls_success = load_word(load_addr_phy,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = sign_extend<uint_t>((wb_data & MASK32),32);
                                wb_data = ((wb_data & MASK32) + (reg_file[rs2] & MASK32)) & MASK32;
                                ls_success = store_word(load_addr,load_data,wb_data, store_data);
                                memory.at(load_addr_phy/8) = store_data;
                            }
                            break;

                        case 0b00100 : //AMOXOR.W
                            
                            ls_success = load_word(load_addr_phy,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = sign_extend<uint_t>((wb_data & MASK32),32);
                                wb_data = ((wb_data & MASK32) ^ (reg_file[rs2] & MASK32)) & MASK32;
                                ls_success = store_word(load_addr,load_data,wb_data, store_data);
                                memory.at(load_addr_phy/8) = store_data;
                            }
                            break;

                        case 0b01100 : //AMOAND.W
                            
                            ls_success = load_word(load_addr_phy,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = sign_extend<uint_t>((wb_data & MASK32),32);
                                wb_data = ((wb_data & MASK32) & (reg_file[rs2] & MASK32)) & MASK32;
                                ls_success = store_word(load_addr,load_data,wb_data, store_data);
                                memory.at(load_addr_phy/8) = store_data;
                            }
                            break;

                        case 0b01000 : //AMOOR.W
                            
                            ls_success = load_word(load_addr_phy,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = sign_extend<uint_t>((wb_data & MASK32),32);
                                wb_data = ((wb_data & MASK32) | (reg_file[rs2] & MASK32)) & MASK32;
                                ls_success = store_word(load_addr,load_data,wb_data, store_data);
                                memory.at(load_addr_phy/8) = store_data;
                           }
                            break;

                        case 0b10000 : //AMOMIN.W
                            
                            ls_success = load_word(load_addr_phy,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = sign_extend<uint_t>((wb_data & MASK32),32);
                                wb_data = min(signed_value32(wb_data & MASK32), signed_value32(reg_file[rs2] & MASK32)) & MASK32;
                                ls_success = store_word(load_addr,load_data,wb_data, store_data);
                                memory.at(load_addr_phy/8) = store_data;
                            }
                            break;

                        case 0b10100 : //AMOMAX.W
                            
                            ls_success = load_word(load_addr_phy,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = sign_extend<uint_t>((wb_data & MASK32),32);
                                wb_data = max(signed_value32(wb_data & MASK32), signed_value32(reg_file[rs2] & MASK32)) & MASK32;
                                ls_success = store_word(load_addr,load_data,wb_data, store_data);
                                memory.at(load_addr_phy/8) = store_data;
                            }
                            break;

                        case 0b11000 : //AMOMINU.W
                            
                            ls_success = load_word(load_addr_phy,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = sign_extend<uint_t>((wb_data & MASK32),32);
                                wb_data = min((wb_data & MASK32), (reg_file[rs2] & MASK32)) & MASK32;
                                ls_success = store_word(load_addr,load_data,wb_data, store_data);
                                memory.at(load_addr_phy/8) = store_data;
                            }
                            break;

                        case 0b11100 : //AMOMAXU.W
                            
                            ls_success = load_word(load_addr_phy,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr_phy;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = sign_extend<uint_t>((wb_data & MASK32),32);
                                wb_data = max((wb_data & MASK32), (reg_file[rs2] & MASK32)) & MASK32;
                                ls_success = store_word(load_addr,load_data,wb_data, store_data);
                                memory.at(load_addr_phy/8) = store_data;
                            }
                            break;

                        default :
                            printf("******INVALID INSTRUCTION******\nINS :%lu\nOPCODE :%lu\n",instruction,(uint_t)opcode);
                            bitset<5> ins(amo_op);

                            mtval = instruction;
                            ILL_INS = true;
                            break;   
                    }
                    reg_file[rd] = ret_data;
                }
                else if (func3 == 0b011){
                    #ifdef DEBUG
                        printf("AMO 64\n");
                    #endif
                        load_addr = reg_file[rs1];
                        load_addr_phy = load_addr;

                        if (load_addr_phy==-1){
                            //cout << "Page fault exception store"<<endl;
                            PC = excep_function(PC,CAUSE_STORE_PAGE_FAULT,CAUSE_STORE_PAGE_FAULT,CAUSE_STORE_PAGE_FAULT,cp);
                            // STORE_PAGE_FAULT = true;
                            mtval = load_addr;
                            switch(cp){
                                    case MMODE : 
                                        mtval = mtval;
                                        break;
                                    case SMODE :
                                        stval = mtval;
                                        mtval = 0;
                                        break;
                                    case UMODE :
                                        utval = mtval;
                                        mtval = 0;
                                        break;
                                }

                            continue;
                        }
                    if(load_addr_phy>DRAM_BASE & load_addr_phy<(DRAM_BASE+0x9000000)) {
                        load_addr_phy = load_addr_phy -DRAM_BASE;
                    }
                    else {
                       printf("illegal access %x %x\n",load_addr_phy,load_addr);
                        exit(0);
                    }

                    wb_data = memory.at(load_addr_phy/8);
                    switch (amo_op){ 
                        case 0b00010 : //LR.D
                            
                            if ((load_addr_phy%8)!=0){
                                cout << "AMO-LR.D : Mis-aligned memory access" << endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = wb_data;
                                amo_reserve_valid64 = true;
                                amo_reserve_addr64 = reg_file[rs1];
                                LR_count64 = 0;
                                LR_cp64 = cp;
                            }
                            break;

                        case 0b00011 : //SC.D
                            if (amo_reserve_valid64 && (reg_file[rs1]==amo_reserve_addr64)){
                                store_data = reg_file[rs2];
                                if ((load_addr_phy%8)!=0){
                                    cout << "AMO-SC.D : Mis-aligned memory access" << endl;
                                    mtval = load_addr;
                                    STORE_ADDR_MISSALIG = true;
                                }
                                else {
                                    memory.at(load_addr_phy/8) = store_data;
                                    ret_data = 0;
                                }
                            }
                            else {
                                ret_data = 1;
                            }
                            amo_reserve_addr64 = 0;
                            amo_reserve_valid64 = false;
                            break;

                        case 0b00001 : //AMOSWAP.D
                            
                            if ((load_addr_phy%8)!=0){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = wb_data;
                                wb_data = reg_file[rs2];
                                memory.at(load_addr_phy/8) = wb_data;
                            }
                            break;

                        case 0b00000 : //AMOADD.D
                            
                            if ((load_addr_phy%8)!=0){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = wb_data;
                                wb_data = (wb_data + reg_file[rs2]);
                                memory.at(load_addr_phy/8) = wb_data;
                            }
                            break;

                        case 0b00100 : //AMOXOR.D
                            
                            if ((load_addr_phy%8)!=0){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = wb_data;
                                wb_data = (wb_data ^ reg_file[rs2]);
                                memory.at(load_addr_phy/8) = wb_data;
                            }
                            break;

                        case 0b01100 : //AMOAND.D
                            
                            if ((load_addr_phy%8)!=0){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = wb_data;
                                wb_data = (wb_data & reg_file[rs2]);
                                memory.at(load_addr_phy/8) = wb_data;
                            }
                            break;

                        case 0b01000 : //AMOOR.D
                            
                            if ((load_addr_phy%8)!=0){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = wb_data;
                                wb_data = (wb_data | reg_file[rs2]);
                                memory.at(load_addr_phy/8) = wb_data;
                            }
                            break;

                        case 0b10000 : //AMOMIN.D
                            
                            if ((load_addr_phy%8)!=0){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = wb_data;
                                wb_data = min(signed_value(wb_data), signed_value(reg_file[rs2]));
                                memory.at(load_addr_phy/8) = wb_data;
                            }
                            break;

                        case 0b10100 : //AMOMAX.D
                            
                            if ((load_addr_phy%8)!=0){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = wb_data;
                                wb_data = max(signed_value(wb_data), signed_value(reg_file[rs2]));
                                memory.at(load_addr_phy/8) = wb_data;
                            }
                            break;

                        case 0b11000 : //AMOMINU.D
                            
                            if ((load_addr_phy%8)!=0){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = wb_data;
                                wb_data = min(wb_data, reg_file[rs2]);
                                memory.at(load_addr_phy/8) = wb_data;
                            }
                            break;

                        case 0b11100 : //AMOMAXU.D
                            
                            if ((load_addr_phy%8)!=0){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = wb_data;
                                wb_data = max(wb_data, reg_file[rs2]);
                                memory.at(load_addr_phy/8) = wb_data;
                            }
                            break;

                        default :  
                            printf("******INVALID INSTRUCTION******\nINS :%lu\nOPCODE :%lu\n",instruction,instruction & 0b1111111);
                            bitset<3> ins(func3);
                            cout<<  "func3 : "<<ins<<endl;
                            break;                        
                    }
                    reg_file[rd] = ret_data;
                }
                break;

            case fence :
                continue;

            case systm :

                switch(func3){
                    case 0b001 : // CSRRW

                        csr_data = csr_read(imm11_0);
                        if (csr_read_success){
                            store_data = reg_file[rs1];

                            csr_bool = csr_write(imm11_0,store_data);
                            if(!csr_bool){
                                mtval = instruction;
                                ILL_INS = true;
                            }
                            else if (rd!=0)
                                reg_file[rd] = csr_data;
                        } else{
                            mtval = instruction;
                            ILL_INS = true;
                        }
                        break;

                    case 0b010 : // CSRRS     rdtime, rdcycle, rdinsret should be handled here
                    
                        csr_data = csr_read(imm11_0);

                        if (csr_read_success) {
                            if ( (imm11_0==CYCLE) | (imm11_0==TIME) | (imm11_0==INSTRET) ){
                                if(imm11_0==TIME){
                                //:    printf("%0x\n time read\n",instruction);
                                    reg_file[rd] = csr_data;
                                }
                            }
                            else {
                                store_data = reg_file[rs1];
                                store_data = (store_data | csr_data);

                                csr_bool = csr_write(imm11_0,store_data);
                                if(!csr_bool){
                                    mtval = instruction;
                                    ILL_INS = true;
                                } else 
                                    reg_file[rd] = csr_data;
                            }
                        } 
                        else{
                            mtval = instruction;
                            ILL_INS = true;
                        }
                        break;

                    case 0b011 : // CSRRC

                        csr_data = csr_read(imm11_0);
                        if (csr_read_success){
                            store_data = reg_file[rs1];
                            store_data = (csr_data & (MASK64 - store_data));

                            csr_bool = csr_write(imm11_0,store_data);
                            if(!csr_bool){
                                mtval = instruction;
                                ILL_INS = true;
                            } else 
                                reg_file[rd] = csr_data;
                        } else{
                            mtval = instruction;
                            ILL_INS = true;
                        }
                        break;

                    case 0b101 : // CSRRWI

                        csr_data = csr_read(imm11_0);
                        if (csr_read_success){

                            csr_bool = csr_write(imm11_0,rs1);
                            if(!csr_bool){
                                mtval = instruction;
                                ILL_INS = true;
                            }
                            else if (rd!=0)
                                reg_file[rd] = csr_data;
                        } else{
                            mtval = instruction;
                            ILL_INS = true;
                        }
                        break;

                    case 0b110 : // CSRRSI

                        csr_data = csr_read(imm11_0);
                        if (csr_read_success){
                            store_data = (rs1 | csr_data);

                            csr_bool = csr_write(imm11_0,store_data);
                            if(!csr_bool){
                                mtval = instruction;
                                ILL_INS = true;
                            } else 
                                reg_file[rd] = csr_data;
                        } else{
                            mtval = instruction;
                            ILL_INS = true;
                        }
                        break;

                    case 0b111 : // CSRRCI

                        csr_data = csr_read(imm11_0);
                        if (csr_read_success){
                            store_data = (csr_data & (MASK64 - rs1));

                            csr_bool = csr_write(imm11_0,store_data);
                            if(!csr_bool){
                                mtval = instruction;
                                ILL_INS = true;
                            } else 
                                reg_file[rd] = csr_data;
                        } else{
                            mtval = instruction;
                            ILL_INS = true;
                        }
                        break;

                    case 0b000 : 
                        switch(imm11_0){
                            case 0 : //ecall
                        
                                PC = excep_function(PC,CAUSE_MACHINE_ECALL,CAUSE_SUPERVISOR_ECALL,CAUSE_USER_ECALL,cp);
                                break;

                            case 1 : //ebreak

                                EBREAK = true;
                                break;

                            case 770 : //mret
                                PC = mepc;
                                cp = (plevel_t)mstatus.mpp;

                                mstatus.mie = 1;//mstatus.mpie;
                                mstatus.mpp = 0b00; //setting to umode
                                mstatus.mpie = 0;
                                break;

                            case 258 : //sret
                                PC = sepc;
								
                                cp = (plevel_t)mstatus.spp;
                                mstatus.spp = 0b0;
                                sstatus.sie = sstatus.spie;
                                sstatus.spie = 1;

                                break;
						
                            case 2 : //uret
								exit(0);
                                PC = uepc;
                                cp = (plevel_t)UMODE;
                                mstatus.mpp = 0b00; //setting to umode
                                mstatus.spp = 0b0;
                                ustatus.uie = ustatus.upie;
                                ustatus.upie = 1;
                                break;

                            default :
                                break;
                        }
                        break;

                    default :
                        cout << "Invalid system instruction : "<< func3 << endl;
                        break;
                }
                break;

            case fd1 :
                cout << "FD Instructions"<<endl;
                mtval = instruction;
                ILL_INS = true;
                break;

            case fd2 :
                cout << "FD Instructions"<<endl;
                mtval = instruction;
                ILL_INS = true;
                break;

            case fd3 :
                cout << "FD Instructions"<<endl;
                mtval = instruction;
                ILL_INS = true;
                break;

            case fd4 :
                cout << "FD Instructions"<<endl;
                mtval = instruction;
                ILL_INS = true;
                break;

            case fd5 :
                cout << "FD Instructions"<<endl;
                mtval = instruction;
                ILL_INS = true;
                break;

            case fd6 :
                cout << "FD Instructions"<<endl;
                mtval = instruction;
                ILL_INS = true;
                break;

            case fd7 :
                cout << "FD Instructions"<<endl;
                mtval = instruction;
                ILL_INS = true; 
                break;

            default :
                printf("default %0d %0x %0x\n",__LINE__,PC_phy,instruction );
                exit(0);
                break;
        }
        

        if (mstatus.fs==3){
            mstatus.sd = 1;
        }
        else {
            mstatus.sd = 0;
        }


        

        mip.STIP = (mtime >= mtimecmp );

        //if (mtime >= mtimecmp )
        //    cout<< "################## Timer fired ###################"<<endl;



        //exception/interupt finding combo

        //external interupts >> software interupts >> timer interupts >> synchornous traps
      
        if(LD_ACC_FAULT) {
            cout << "load access fault!!"<<endl;
            LD_ACC_FAULT = false;
            PC = excep_function(PC,CAUSE_LOAD_ACCESS,CAUSE_LOAD_ACCESS,CAUSE_LOAD_ACCESS,cp);
            write_tval = false;
            
        }
       
     
      
        else if( mie.MEIE & mip.MEIP) {

            PC = interrupt_function(PC, CAUSE_MACHINE_EXT_INT, cp);
        }
        else if( mie.MTIE & mip.MTIP) {

            PC = interrupt_function(PC, CAUSE_MACHINE_TIMER_INT, cp);
        }
        else if( mie.MSIE & mip.MSIP) {
            PC = interrupt_function(PC, CAUSE_MACHINE_SOFT_INT, cp);
        }
        else if( mie.SEIE & mip.SEIP) {
            PC = interrupt_function(PC, CAUSE_SUPERVISOR_EXT_INT, cp);  
        }

        else if( mie.STIE & mip.STIP) {
            // printf("################## Timer fired ###################\n");
            PC = interrupt_function(PC, CAUSE_SUPERVISOR_TIMER_INT, cp);
        }
        else if( mie.SSIE & mip.SSIP) {
             PC = interrupt_function(PC, CAUSE_SUPERVISOR_SOFT_INT, cp);
        }
        else if( mie.UEIE & mip.UEIP) {
                PC = interrupt_function(PC, CAUSE_USER_EXT_INT, cp);
        }
        else if( mie.UTIE & mip.UTIP) {
             PC = interrupt_function(PC, CAUSE_USER_TIMER_INT, cp);
        }
        else if( mie.USIE & mip.USIP) {
            PC = interrupt_function(PC, CAUSE_USER_SOFT_INT, cp);
        }
        
    
        else if(INS_ACC_FAULT) {
            INS_ACC_FAULT = false;
            PC = excep_function(PC,CAUSE_FETCH_ACCESS,CAUSE_FETCH_ACCESS,CAUSE_FETCH_ACCESS,cp);
            write_tval = false;

        }
        
        else if (ILL_INS) {
            ILL_INS = false;
            PC = excep_function(PC,CAUSE_ILLEGAL_INSTRUCTION,CAUSE_ILLEGAL_INSTRUCTION,CAUSE_ILLEGAL_INSTRUCTION,cp);
            write_tval = true;
        }
       // else if(EBREAK) {
       //     EBREAK = false;
       //     PC = excep_function(PC,CAUSE_BREAKPOINT,CAUSE_BREAKPOINT,CAUSE_BREAKPOINT,cp);
       //     write_tval = false;
       // }
        else if(INS_ADDR_MISSALIG) {
            INS_ADDR_MISSALIG = false;
            PC = excep_function(PC,CAUSE_MISALIGNED_FETCH,CAUSE_MISALIGNED_FETCH,CAUSE_MISALIGNED_FETCH,cp);
            write_tval = true;
        }
        else if (LD_ADDR_MISSALIG) {
            printf("load missalgined %0x %0x %0d %0x\n",load_addr,load_addr_phy,func3,lPC);
            //exit(1);
            LD_ADDR_MISSALIG = false;
            PC = excep_function(PC,CAUSE_MISALIGNED_LOAD,CAUSE_MISALIGNED_LOAD,CAUSE_MISALIGNED_LOAD,cp);
            write_tval = true;

        }
        else if(STORE_ADDR_MISSALIG) {
            STORE_ADDR_MISSALIG = false;
            PC = excep_function(PC,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,cp);
            write_tval = true;
        }
        insno++;
        if (lPC==PC){
            cout << "Infinite loop!"<<endl;
            auto end = chrono::steady_clock::now();
            cout << "Elapsed time in seconds : " << chrono::duration_cast<chrono::seconds>(end - start).count()<<endl;
            cout << "Nanoseconds per cycle : " << ((double)chrono::duration_cast<chrono::nanoseconds>(end - start).count() / cycle) <<endl;

            break;
        }

    }

    return 0;
}


