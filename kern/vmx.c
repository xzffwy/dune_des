int vmx_launch(struct dune_config *conf,int64_t *ret_code)
	1������vcpu:vmx_create_vcpu(conf);

//1��cpuֻ����һ��vcpu��
static struct vmx_vcpu * vmx_create_vcpu(struct dune_config *conf)
	struct vmx_vcpu {                         1������vcpu���ڴ沢��ʼ��
		int cpu;							  2����ô������ı�ţ�����vmcs����
		int vpid;                             3��Ϊvcpu����vpid,��vmx_vpid_bitmap������
		int launched; 						  4����ʼ��vcpu��cpuΪ-1����syscall_tbl
											  5����ʼ��ept������һ������ҳ���������ַ��ȥpageoffset��ֵ��
		struct mmu_notifier mmu_notifier;	     vcpu->ept_root
		spinlock_t ept_lock;                  6��vcpu->eptp=construct_eptp(vcpu->ept_rooot);//����
		unsigned long ept_root;
		unsigned long eptp;
		bool ept_ad_enabled;

		u8  fail;
		u64 exit_reason;
		u64 host_rsp;
		u64 regs[NR_VCPU_REGS];
		u64 cr2;

		int shutdown;
		int ret_code;

		struct msr_autoload {
			unsigned nr;
			struct vmx_msr_entry guest[NR_AUTOLOAD_MSRS];
			struct vmx_msr_entry host[NR_AUTOLOAD_MSRS];
		} msr_autoload;

		struct vmcs *vmcs;
		void *syscall_tbl;
	};

static void vmx_get_cpu(struct vmx_vcpu *vcpu)
	called before using a cpu,and disables preemption
	1����ȡ��ǰ�����������ҽ�ֹ�ں���ռ
	2�����������õ�cpu��local_vcpuΪvcpu
	3�����vcpu��cpu���ǵ�ǰ���е�cpu(���vcpu��vmcs������ʹ�䴦��cpuûȷ�������)��
	   3.1�����vcpu�Ѿ���cpu��������vcpu->cpu�����к���__vmx_get_cpu_helper(vcpu):���vcpu��vmcs�����ҽ����е�cpu�ϵ�local_vcpu��NULL
	   3.2�������ֱ�����vcpu��vmcs

static inline void vpid_sync_context(u16 vpid)
	1��

static inline bool cpu_has_vmx_invvpid_global(void)
  

static struct vmcs *__vmcs_alloc_vmcs(int cpu)
	1��vmcs_alloc_vmcs�ǻ�õ�ǰ�Ĵ�������ź��ٵ��øú���
	2������alloc_pages_exact���ض���cpu�Ϸ���һҳ�ڴ棬pages
	3��vmcs��ָ���ҳ
	4����vmcs���г�ʼ��������revision_id;
