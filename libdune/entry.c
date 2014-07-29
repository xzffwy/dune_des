int dune_init(bool map_full)
	1���������豸��/dev/dune
	2������һ��ҳ���СΪPGSIZE���ҿ�ʼ��ַΪPGSIZE��ҳ������ʼ��,Ϊpgroot
	3������setup_mappings(map_full);
	4��setup_syscall();
	5������sigstop,sigkill,sigchld,sigint,�������źŶ�������
	6��setup_idt();

static int setup_mappings(bool full)
	1����ioctl�������dune_layout,��/dune/kern/core.c�ṩ�����phys_limit,mmap_base,stack_base
	2������full���������ΪTRUE�����__setup_mappings_full(layout),����͵���__setup_mappings_precise();
	
static int __setup_mapping_precise(void)
	1��dune_vm_map_phys(pgroot,PAGEBASE,MAX_PAGES*PGSIZE,dune_va_to_va(PAGEBASE),PERM_R,PERM_W,PERM_BIG);
    2������dune_procmap_iterate(&__setup_mappings_cb):�鿴���̵�����ռ������ʹ�õģ��ٵ��ú���

static setup_mappings_cb(const struct dune_procmap_entry *ent)
	1������ent��begin��飬��ֹpage�Ѿ�mapped
	2��if(ent->begin == VSYSCALL_ADDR) setup_vsyscall();return;
	3��if(ent->type==VSYSCALL_ADDR) setup_vdso(ent->begin,ent->end-ent->begin);return
	4��dune_vm_map_phys(pgroot,ent->begin,ent->end-ent->begin,dune_va_to_pa(ent->begin),perm);

static void setup_vdso(void *addr,size_t len)
	VDSO����Virtual Dynamic Shared Object�������ں��ṩ�������.so,���.so�ļ����ڴ����ϣ��������ں���ͷ���ں˰Ѱ���ĳ.so���ڴ�ҳ�ڳ���������ʱ��ӳ�������ڴ�ռ䣬��Ӧ�ĳ���Ϳ��Ե���ͨ��.so��ʹ����ͷ�ĺ���������syscall()�������������linux-vdso.so.1��ͷ�ģ����Ǵ����ϲ�û�ж�Ӧ���ļ�.����ͨ��ldd/bin/bash������
    // elf helper functions
    1��struct dune_elf {
    	int		fd;
    	unsigned char	*mem;
	    int		len;
		Elf64_Ehdr	hdr;
		Elf64_Phdr	*phdr;
		Elf64_Shdr	*shdr;
		char		*shdrstr;
		void		*priv;
	}elf;
	2��struct dynsym {
		char		*ds_name;
		int		ds_idx;
		int		ds_off;
		struct dynsym	*ds_next;
	}*ds,*d;
    	


static int __setup_mapping_full(void)
	1��dune_vm_map_phys(pgroot,PAGEBASE,MAX_PAGES*PGSIZE,dune_va_to_va(PAGEBASE),PERM_R,PERM_W,PERM_BIG);
	2��dune_vm_map_phys(pgroot,0,1UL<<32,0,PERM_R,PERM_W,PERM_BIG);
	3��dune_vm_map_phys(pgroot,layout->basemap,MAX_PAGES*layout->basemap,dune_va_to_va(PAGEBASE),PERM_R|PERM_W|PERM_X|PERM|U);
   	4��dune_vm_map_phys(pgroot,layout->basestack,MAX_PAGES*layout->basestack,dune_va_to_va(PAGEBASE),PERM_R|PERM_W|PERM_X|PERM|U);
    5��dune_procmap_iterate(setup_vdso_cb);
	6��setup_vsyscall();

static void setup_vdso_cb(const struct dune_procmap_entry *ent)
	if(ent->type==PROCMAP_TYPE_VDSO)  setup_vdso(...);


int dune_enter(void)
	1������ļ��Ƿ��Ѿ�����dune mode
	2������cpu��vcpu��


static struct dune_percpu *create_percpu(void)
	1��struct dune_percpu {
			uint64_t tmp;
			uint64_t kfs_base;
			uint64_t ufs_base;
			uint64_t in_usermode;
			struct Tss tss;
			uint64_t gdt[NR_GDT_ENTRIES];
		} __attribute__((packed));
    2�������޷��ų�����fs_base,���ú���arch_prctl(ARCH_GET_FS,&fs_base),���üܹ����ض����߳�״̬��return the 64bit base value for the gs register of the current thread in the unsigned long pointed to by the addressparameter
	3������mmap�������������ڴ��ַҪ��Ŀɶ���д������ӳ��
	4������map_ptr(percpu,sizeof(*percpu));�����ַ��������ַ��ӳ�䣬������Ҫ�仯һ��
	5������percpu�е�һЩֵ��kfs_base,ufs_base��Ϊfs_base,in_usermode=0;
	6��setup_safe_stack(percpu);


static void map_ptr(vod *p, int len)
	1��PGADDR(p)�������p��4kb�ı����ĵ�ַ����p&2^12
	2������dune_vm_map_phys��������ӳ�䣬���г���Ϊlen/4kb+PGSIZE,��ʼλp

static int setup_safe_stack(struct dune_percpu *percpu)
	1��safe_stack=����mmap�������ڴ���ʵҪ����޴��ļ��Ĵ�СΪPGSIZE������ӳ��
	2������map_ptr(safe_stack,PGSIZE)��������ַ�������ַ��ӳ�䣬���ȱ仯��һ��
		struct Tss {
		  char tss__ign1[4];
		  uint64_t tss_rsp[3];		/* Stack pointer for CPL 0, 1, 2 */
		  uint64_t tss_ist[8];		/* Note: tss_ist[0] is ignored */
		  char tss__ign2[10];
		  uint16_t tss_iomb;		/* I/O map base */
		  uint8_t tss_iopb[];
		} __attribute__ ((packed));
	3��percpu->tss.tss_iomb=offestof(Tss,tss_iopb);
	4��percpu->tss.tss_ist���鶼����ֵΪ(uintptr_t)safe_stack
	5��percpu->tss.tss_rsp[0]����ֵΪsafe_stack

static int do_dune_enter(struct dune_percpu *percpu)
	1������map_stack,�鿴���̵������ַ�ռ䣬�ڷ��������½��������ڴ浽�����ڴ��ӳ��
	struct dune_config {
		__u64 rip;
		__u64 rsp;
		__u64 cr3;
		__s64 ret;
	} __attribute__((packed));
    2��conf.rip=(__u64)&__dune_ret,con.rsp=0,conf.cr3=(physaddr_t)pgroot;
	

static void map_stack(void)
	dune_procmap_iterate(map_stack_cb);�鿴���̵������ַ�ռ�

static void map_stack_cb(const struct dune_procmap_entry *e)
    1��asm ("mov %%rsp, %0" : "=r" (esp));����
	2��esp��e�ķ�Χ֮�ڣ�����map_ptr(e->begin,e->end-e->begin);���������ڴ浽�����ڴ��ӳ��
