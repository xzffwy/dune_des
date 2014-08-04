int vmx_init_ept(struct vmx_vcpu *vcpu)
	分配一个空的内存页，初始化，将虚拟地址-PAGEOFFSET赋值给vcpu->ept_root

int vmx_create_ept(struct vmx_vcpu *vcpu)
	struct mmu_notifier {
		struct hlist_node hlist;
		const struct mmu_notifier_ops *ops;
	};
    1、初始化ept_mmu操作函数:mmu_notifier.ops:中初始化
	2、注册mmu_notifier


struct mmu_notifier_.ops:中的操作函数：
	.invalidate_page:remove a page form the ept:
	    ept_mmu_notifier_invalidate_page(struct mmu_notifier *mn,struct mm_struct *mm,unsigned long address)
	.invalidate_range_start= ept_mmu_notifier_invalidate_range_start(*mn,*mm,unsigned long start,end);
    

static void ept_mmu_notifier_invalidate_range_start(struct mmu_notifier *mn,struct mm_struct *mm,unsigned long start,unsigned end)
	1、获得vcpu
	2、从start到end开始循环，变量为pos，查找gpa，invalidate_page


static int ept_mmu_notifier_invalidate_page(struct mmu_notifier *mn,struct mm_struct *mm,unsigned long address)
	//总体是不懂的，得看完内存再看

static int ept_invalidate_page(struct vmx_vcpu *vcpu,struct mm_struct *mm,unsigned long addr)
	1、hva_to_gpa

struct unsigned long hva_to_gpa(struct vmx_vcpu *vcpu,struct mm_struct *mm,
		unsigned long hva)
   //虚拟地址到客户机物理地址 
   
static int ept_lookup_gpa(struct vmx_vcpu *vcpu,void *pga,int level,int create,epte_t **epte_out)
	//对于扩展分页中的处理有bug
	level用来表示gpa采用的是几级页表
	create为1代表的是如果ept所对应的pga为空的话则分配内存
	1、gpa提供的是客户机物理地址,返回的是hva，输出在epte_out中

static void vmx_free_ept(unsigned long ept_root)
	依次获得pgd,pud,pmd,pte，对其中的每一项进行操作，如果是页的话检查是否需要回写等操作，然后再依次释放内存页

static inline int epte_present(epte_t epte)
	如果epte的页表项不指向页面，返回false

static uintptr_t epte_addr(epte_t epte) //返回epte所指的页面的物理地址
pfn_to_page(pte) 根据结构体的个数，相加获得page的物理地址,pte为host page frame number,将mem_map的值加上pfn（    结构体的偏移量）获得页的物理地址

staitc void free_ept_page(epte_t epte)
	1、获得page的地址，因为PAE，页的总大小为2MB，不是4kb
	2、判断是否为PFN mappings，如果则不用回写
	3、如果页面脏了，是需要回写的 set_page_dirty_lock(page);
	4、put_page(page);//释放页

