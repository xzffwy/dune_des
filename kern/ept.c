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
    	

static int ept_mmu_notifier_invalidate_page(struct mmu_notifier *mn,struct mm_struct *mm,unsigned long address)
	//总体是不懂的，得看完内存再看

struct unsigned long hva_to_gpa(struct vmx_vcpu *vcpu,struct mm_struct *mm,
		unsigned long hva)
   //logic to liner address 
   
static int ept_lookup_gpa(struct vmx_vcpu *vcpu,void *pga,int level,int create,epte_t **epte_out)
	其实就是得出页物理地址的
	1、获得ept的虚拟地址为:dir，即为ept的首地址
	2、以循环的方式访问pga的页，不断求得在dir的偏移量idx  P63
	   2.1、如果dir[idx]为空，并且是create是1则创造新的页，否则return
	   2.2、如果是扩展页，levl=i，break
    3、输出结果在epte_out中，为dir[idx]

static void vmx_free_ept(unsigned long ept_root)
	依次获得pgd,pud,pmd,pte，对其中的每一项进行操作，如果是页的话检查是否需要回写等操作，然后再依次释放内存页

staitc void free_ept_page(epte_t epte)
	1、获得page的地址，因为PAE，页的总大小为2MB，不是4kb
	2、判断是否为PFN mappings，如果则不用回写
	3、如果页面脏了，是需要回写的 set_page_dirty_lock(page);
	4、put_page(page);

