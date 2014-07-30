int vmx_launch(struct dune_config *conf,int64_t *ret_code)
	1、分配vcpu:vmx_create_vcpu(conf);

//1个cpu只能有一个vcpu吗？
static struct vmx_vcpu * vmx_create_vcpu(struct dune_config *conf)
	struct vmx_vcpu {                         1、分配vcpu的内存并初始化
		int cpu;							  2、获得处理器的编号，分配vmcs区域
		int vpid;                             3、为vcpu分配vpid,在vmx_vpid_bitmap基础上
		int launched; 						  4、初始化vcpu的cpu为-1，和syscall_tbl
											  5、初始化ept，分配一个空闲页，将虚拟地址减去pageoffset赋值给
		struct mmu_notifier mmu_notifier;	     vcpu->ept_root
		spinlock_t ept_lock;                  6、vcpu->eptp=construct_eptp(vcpu->ept_rooot);//不懂
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
	1、获取当前处理器，并且禁止内核抢占
	2、设置现在用的cpu的local_vcpu为vcpu
	3、如果vcpu的cpu不是当前运行的cpu(清空vcpu的vmcs，并且使其处在cpu没确定的情况)：
	   3.1、如果vcpu已经有cpu，则在其vcpu->cpu上运行函数__vmx_get_cpu_helper(vcpu):清空vcpu的vmcs，并且将运行的cpu上的local_vcpu置NULL
	   3.2、负责就直接清空vcpu的vmcs

static inline void vpid_sync_context(u16 vpid)
	1、

static inline bool cpu_has_vmx_invvpid_global(void)
  

static struct vmcs *__vmcs_alloc_vmcs(int cpu)
	1、vmcs_alloc_vmcs是获得当前的处理器编号后再调用该函数
	2、调用alloc_pages_exact在特定的cpu上分配一页内存，pages
	3、vmcs即指向该页
	4、对vmcs进行初始化并分配revision_id;
