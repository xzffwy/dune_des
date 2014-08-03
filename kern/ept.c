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
    	

static int ept_mmu_notifier_invalidate_page(struct mmu_notifier *mn,struct mm_struct *mm,unsigned long address)
	//�����ǲ����ģ��ÿ����ڴ��ٿ�

struct unsigned long hva_to_gpa(struct vmx_vcpu *vcpu,struct mm_struct *mm,
		unsigned long hva)
   //logic to liner address 
   
static int ept_lookup_gpa(struct vmx_vcpu *vcpu,void *pga,int level,int create,epte_t **epte_out)
	��ʵ���ǵó�ҳ�����ַ��
	1�����ept�������ַΪ:dir����Ϊept���׵�ַ
	2����ѭ���ķ�ʽ����pga��ҳ�����������dir��ƫ����idx  P63
	   2.1�����dir[idx]Ϊ�գ�������create��1�����µ�ҳ������return
	   2.2���������չҳ��levl=i��break
    3����������epte_out�У�Ϊdir[idx]

static void vmx_free_ept(unsigned long ept_root)
	���λ��pgd,pud,pmd,pte�������е�ÿһ����в����������ҳ�Ļ�����Ƿ���Ҫ��д�Ȳ�����Ȼ���������ͷ��ڴ�ҳ

staitc void free_ept_page(epte_t epte)
	1�����page�ĵ�ַ����ΪPAE��ҳ���ܴ�СΪ2MB������4kb
	2���ж��Ƿ�ΪPFN mappings��������û�д
	3�����ҳ�����ˣ�����Ҫ��д�� set_page_dirty_lock(page);
	4��put_page(page);

