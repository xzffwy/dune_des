int dune_init(bool map_full)
	1、打开驱动设备：/dev/dune
	2、分配一个页面大小为PGSIZE并且开始地址为PGSIZE的页，并初始化,为pgroot //LDT page for process enter dune
	3、为进程分配内存映射（线性区），并分配页的标示符
	3、调用setup_mappings(map_full);//分为部分映射和全部映射，是出于overhead的考虑
	4、setup_syscall();//
	5、除了sigstop,sigkill,sigchld,sigint,其他的信号都将忽略
	6、setup_idt();

static void setup_idt(void)
#define IDT_ENTRIES     256 
struct idtd {
        uint16_t        low;
        uint16_t        selector;
        uint8_t         ist;
        uint8_t         type;
        uint16_t        middle;
        uint32_t        high;
        uint32_t        zero;
} __attribute__((packed)) __attribute__ ((aligned));
    1、对于每一个中断向量表，isr为&__dune_intr+16*i的偏移，id为一个指向ditd[i]的结构体指针，对于id所指的结构进行一些操作
	

static int setup_syscall(void)
	1、__dune_syscall_end - __dune_syscall(dune.S中定义)
	2、读取MSR_LSTAR的值到lstar
	3、调用mmap映射一个PGSIZE*2大小的可写执行和读的私有的映射为page
	4、将__dune_syscall的内容复制到page+off出，其中off为lstar的低PGSIZE bit
	5、对上一步的page分配物理地址，将page内的东西复制到刚刚开的物理页面中

static int setup_mappings(bool full)
	//GPA_MAP_SIZE 1<<31   GPA_STACK_SIZE  1<<28	
	//layout中表示的mmap_base(abfbc000),stack_base(3af85130)分别表示的是mmap和stack的其实地址，phys_limit为什么为0
	1、由ioctl函数获得dune_layout,由/dune/kern/core.c提供，获得phys_limit,mmap_base,stack_base
	2、根据full参数，如果为TRUE则调用__setup_mappings_full(layout),否则就调用__setup_mappings_precise();
	
static int __setup_mappings_full(void)
	调用dune_vm_map_phys进行虚拟地址到物理地址的映射,根据所提供的参数，如果已经映射了，则不会重复映射了
	//__setup_mappings_precies只执行这个函数中的第一个
	1、PAGEBASE(0X20000000) len：4GB
	2、0  len:4GB
	3、layout->base_map len:GPA_MAP_SIZE
	4、layout->base_stack len:GPA_STACK_SIZE
	5、对vdso在该进程下分配页面进行映射//错在一些细节
	6、setup_vsyscall();分配vsyscall地址，其中地址是固定的，将__dune_vsyscall_page地址的内容给该页

static int __setup_mappings_precise(void)
	1、只mmap full函数中的第一个部分
	2、调用dune_procmap_iterate(&__setup_mappings_cb)将selt/map中的所有地址空间映射到物理地址

//全部映射
static void __setup_mappings_cb(const struct dune_procmap_entry *end)
	1、判断ent->begin,PAGEBASE is mapped and VSYSCALL_ADDR is call_vsyscall and type's PROCMAP_TYPE_VDSO setup_vdso
	2、将ent所表示的地址映射带内存空间中

static setup_mappings_cb(const struct dune_procmap_entry *ent)
	1、进行ent的begin检查，防止page已经mapped
	2、if(ent->begin == VSYSCALL_ADDR) setup_vsyscall();return;
	3、if(ent->type==VSYSCALL_ADDR) setup_vdso(ent->begin,ent->end-ent->begin);return
	4、dune_vm_map_phys(pgroot,ent->begin,ent->end-ent->begin,dune_va_to_pa(ent->begin),perm);

static void setup_vdso(void *addr,size_t len)
	VDSO就是Virtual Dynamic Shared Object，就是内核提供的虚拟的.so,这个.so文件不在磁盘上，而是在内核里头。内核把包含某.so的内存页在程序启动的时候映射入其内存空间，对应的程序就可以当普通的.so来使用里头的函数。比如syscall()这个函数就是在linux-vdso.so.1里头的，但是磁盘上并没有对应的文件.可以通过ldd/bin/bash看看。
    // elf helper functions
    1、struct dune_elf {
    	int		fd;
    	unsigned char	*mem;
	    int		len;
		Elf64_Ehdr	hdr;
		Elf64_Phdr	*phdr;
		Elf64_Shdr	*shdr;
		char		*shdrstr;
		void		*priv;//ds的一个链表，此为链表头
	}elf;
	2、struct dynsym {
		char		*ds_name;
		int		ds_idx;// Sym->st_name
		int		ds_off;//Sym->st_value &0xfff 是一个页中的偏移量，用来在此放syscall
		struct dynsym	*ds_next;
	}*ds,*d;
    1、elf->priv指向ds	
	2、调用dune_elf_open_mem,对elf的mem和len、fd赋值，调用do_elf_open(elf)将elf的值读入elf->hdr
	3、//elf.hdr.e_type!=ET_DYN
	4、dune_elf_iter_sh(&elf,vdso_sh_cb),对于elf->hdr中的每个Section header table entry，如果类型为SHT_DYNSM,则加入到ds链表中，或者为SHT_STRTAB（sname为".dynstr"）进行ds的name更新
	5、调用mmap函数映射一个大小为PGSIZE匿名的私有的vdso空间
	6、将map中的vdso段地址中的内容前PGSIZE内容复制给vdso
	7、调用dune_vm_lookup(pgroot,addr,1,&pte)查找addr所指的页，并将其地址赋值给pte，如果页面不存在则分配
	8、将vdso里面的值放在pte所指的页中
	9、将dlf->priv中的链表的每个ds指向的系统指令放入vdso对应的位置，然后就所有的ds以及elf->priv的东西释放掉
	10、用mprotect函数将vdso的权限更改为read and exec

static void do_vdso(void *addr,char *name)
	1、根据sname来判定syscall的类型
	2、将一段jms_vdso_end(start)的代码放入addr开始的地址中.并将上一步确定的syscall类型放入其中

static int do_elf_open(struct dune_elf *elf)
	1、elf_read函数，将elf中的东西读入Elf64_Ehdr hdr;

static void setup_vdso_cb(const struct dune_procmap_entry *ent)
	只有类型为VDDO的时候才setup_vdso(ent->begin,ent->end-ent->begin);

static int vdso_sh_cb(struct dune_elf *elf,const char *sname ,int snum,Elf64_Shdr *shdr)
	//ds的理解不够充分
    1、	如果shdr.sh_type为String table并且sname为".dynstr":
	   1.1、ds=elf->prev
	   1.2、循环对每个ds的ds_name重新附新值:elf->mem+shdr->sh_offset+ds->ds_idx
	2、如果shdr.sh_type为dynamic linker symbol table:
	   2.1、求得shdr的size(len)和 struct Elf64_Sym s是一个指向dynsym地址的数组
	   2.2、如果len>sizeof(s)循环:创建ds并且赋值，然后将ds加入到elf->priv所指的链表中，s++,len-=sizeof(*s);
	        如果s->st_value不为空，
int dune_enter(void)
	1、检查文件是否已经进入dune mode
	2、创建cpu（vcpu）


static struct dune_percpu *create_percpu(void)
	1、struct dune_percpu {
			uint64_t tmp;
			uint64_t kfs_base;
			uint64_t ufs_base;
			uint64_t in_usermode;
			struct Tss tss;
			uint64_t gdt[NR_GDT_ENTRIES];
		} __attribute__((packed));
    2、声明无符号长整形fs_base,调用函数arch_prctl(ARCH_GET_FS,&fs_base),设置架构的特定的线程状态。return the 64bit base value for the gs register of the current thread in the unsigned long pointed to by the addressparameter
	3、调用mmap函数，进行无内存地址要求的可读可写的匿名映射
	4、调用map_ptr(percpu,sizeof(*percpu));虚拟地址到物理地址的映射，长度需要变化一点
	5、设置percpu中的一些值，kfs_base,ufs_base都为fs_base,in_usermode=0;
	6、setup_safe_stack(percpu);


static void map_ptr(vod *p, int len)
	1、PGADDR(p)函数获得p的4kb的倍数的地址，即p&2^12
	2、调用dune_vm_map_phys函数建立映射，其中长度为len/4kb+PGSIZE,起始位p

static int setup_safe_stack(struct dune_percpu *percpu)
	1、safe_stack=调用mmap建立无内存其实要求的无打开文件的大小为PGSIZE的匿名映射
	2、调用map_ptr(safe_stack,PGSIZE)，物理地址到虚拟地址的映射，长度变化了一点
		struct Tss {
		  char tss__ign1[4];
		  uint64_t tss_rsp[3];		/* Stack pointer for CPL 0, 1, 2 */
		  uint64_t tss_ist[8];		/* Note: tss_ist[0] is ignored */
		  char tss__ign2[10];
		  uint16_t tss_iomb;		/* I/O map base */
		  uint8_t tss_iopb[];
		} __attribute__ ((packed));
	3、percpu->tss.tss_iomb=offestof(Tss,tss_iopb);
	4、percpu->tss.tss_ist数组都附初值为(uintptr_t)safe_stack
	5、percpu->tss.tss_rsp[0]附初值为safe_stack

static int do_dune_enter(struct dune_percpu *percpu)
	1、调用map_stack,查看进程的虚拟地址空间，在符合条件下进行虚拟内存到物理内存的映射
	struct dune_config {
		__u64 rip;
		__u64 rsp;
		__u64 cr3;
		__s64 ret;
	} __attribute__((packed));
    2、conf.rip=(__u64)&__dune_ret,con.rsp=0,conf.cr3=(physaddr_t)pgroot;
	

static void map_stack(void)
	dune_procmap_iterate(map_stack_cb);查看进程的虚拟地址空间

static void map_stack_cb(const struct dune_procmap_entry *e)
    1、asm ("mov %%rsp, %0" : "=r" (esp));不懂
	2、esp在e的范围之内，代用map_ptr(e->begin,e->end-e->begin);进行虚拟内存到物理内存的映射

