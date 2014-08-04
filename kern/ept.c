int vmx_init_ept(struct vmx_vcpu *vcpu)
	����һ���յ��ڴ�ҳ����ʼ�����������ַ-PAGEOFFSET��ֵ��vcpu->ept_root

int vmx_create_ept(struct vmx_vcpu *vcpu)
	struct mmu_notifier {
		struct hlist_node hlist;
		const struct mmu_notifier_ops *ops;
	};
    1����ʼ��ept_mmu��������:mmu_notifier.ops:�г�ʼ��
	2��ע��mmu_notifier


struct mmu_notifier_.ops:�еĲ���������
	.invalidate_page:remove a page form the ept:
	    ept_mmu_notifier_invalidate_page(struct mmu_notifier *mn,struct mm_struct *mm,unsigned long address)
	.invalidate_range_start= ept_mmu_notifier_invalidate_range_start(*mn,*mm,unsigned long start,end);
    

static void ept_mmu_notifier_invalidate_range_start(struct mmu_notifier *mn,struct mm_struct *mm,unsigned long start,unsigned end)
	1�����vcpu
	2����start��end��ʼѭ��������Ϊpos������gpa��invalidate_page


static int ept_mmu_notifier_invalidate_page(struct mmu_notifier *mn,struct mm_struct *mm,unsigned long address)
	//�����ǲ����ģ��ÿ����ڴ��ٿ�

static int ept_invalidate_page(struct vmx_vcpu *vcpu,struct mm_struct *mm,unsigned long addr)
	1��hva_to_gpa

struct unsigned long hva_to_gpa(struct vmx_vcpu *vcpu,struct mm_struct *mm,
		unsigned long hva)
   //�����ַ���ͻ��������ַ 
   
static int ept_lookup_gpa(struct vmx_vcpu *vcpu,void *pga,int level,int create,epte_t **epte_out)
	//������չ��ҳ�еĴ�����bug
	level������ʾgpa���õ��Ǽ���ҳ��
	createΪ1����������ept����Ӧ��pgaΪ�յĻ�������ڴ�
	1��gpa�ṩ���ǿͻ��������ַ,���ص���hva�������epte_out��

static void vmx_free_ept(unsigned long ept_root)
	���λ��pgd,pud,pmd,pte�������е�ÿһ����в����������ҳ�Ļ�����Ƿ���Ҫ��д�Ȳ�����Ȼ���������ͷ��ڴ�ҳ

static inline int epte_present(epte_t epte)
	���epte��ҳ���ָ��ҳ�棬����false

static uintptr_t epte_addr(epte_t epte) //����epte��ָ��ҳ��������ַ
pfn_to_page(pte) ���ݽṹ��ĸ�������ӻ��page�������ַ,pteΪhost page frame number,��mem_map��ֵ����pfn��    �ṹ���ƫ���������ҳ�������ַ

staitc void free_ept_page(epte_t epte)
	1�����page�ĵ�ַ����ΪPAE��ҳ���ܴ�СΪ2MB������4kb
	2���ж��Ƿ�ΪPFN mappings��������û�д
	3�����ҳ�����ˣ�����Ҫ��д�� set_page_dirty_lock(page);
	4��put_page(page);//�ͷ�ҳ

