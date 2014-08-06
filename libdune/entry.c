int dune_init(bool map_full)
	1、打开驱动设备：/dev/dune
	2、分配一个页面大小为PGSIZE并且开始地址为PGSIZE的页，并初始化,为pgroot //LDT page for process enter dune
	3、为进程分配内存映射（线性区），并分配页的标示符
	3、调用setup_mappings(map_full);
	4、setup_syscall();
	5、除了sigstop,sigkill,sigchld,sigint,其他的信号都将忽略
	6、setup_idt();

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
	5、对该进程下的每个虚拟地址空间setup_vdso

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
		void		*priv;
	}elf;
	2、struct dynsym {
		char		*ds_name;
		int		ds_idx;
		int		ds_off;
		struct dynsym	*ds_next;
	}*ds,*d;
    	

static void setup_vdso_cb(const struct dune_procmap_entry *ent)
	if(ent->type==PROCMAP_TYPE_VDSO)  setup_vdso(...);


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

