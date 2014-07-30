int vmx_launch(struct dune_config *conf,int64_t *ret_code)
	1、分配vcpu:vmx_create_vcpu(conf);

//1个cpu只能有一个vcpu吗？
static struct vmx_vcpu * vmx_create_vcpu(struct dune_config *conf)
	struct vmx_vcpu {                         1、分配vcpu的内存并初始化
		int cpu;                              2、获得处理器的编号，分配vmcs区域
		int vpid;                             3、为vcpu分配vpid,在vmx_vpid_bitmap基础上
		int launched;                         4、初始化vcpu的cpu为-1，和syscall_tbl
                                              5、初始化ept，分配一个空闲页，将虚拟地址减去pageoffset赋值给
		struct mmu_notifier mmu_notifier;        vcpu->ept_root
		spinlock_t ept_lock;                  6、vcpu->eptp=construct_eptp(vcpu->ept_rooot);//不懂
		unsigned long ept_root;               7、执行vmx_get_cpu(vcpu),用来加载current vmcs,并为vcpu的cpu指 
		unsigned long eptp;                      定现在的cpu 
		bool ept_ad_enabled;                  8、调用vmcs_setup_vmcs，配置vm-execution控制区域（检查在之前。
                                                 已经完成）以及对host_state进行配置写入
		u8  fail;                             9、调用vmx_setup_initial_guest_state,检查guest-state区域赋值
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

static void vmx_setup_vmcs(struct vmx_vcpu *vcpu)
	本质上是对控制区域以及host-state区域进行检查的，但是因为控制区域在setup_vmcs_config已经进行检查了，所以这里仅仅是配置。

static void vmx_get_cpu(struct vmx_vcpu *vcpu)
	called before using a cpu,and disables preemption
	1、获取当前处理器，并且禁止内核抢占
	2、设置现在用的cpu的local_vcpu为vcpu
	3、如果vcpu的cpu不是当前运行的cpu(清空vcpu的vmcs，并且使其处在cpu没确定的情况)，否则10：
	   3.1、如果vcpu已经有cpu，则在其vcpu->cpu上运行函数__vmx_get_cpu_helper(vcpu):清空vcpu的vmcs，并且将运行的cpu上的local_vcpu置NULL
	   3.2、负责就直接清空vcpu的vmcs
	4、vpid_sync_contedt(vcpu->vpid);
	5、ept_sync_context(vcpu->eptp);
	6、launchce=0；
	7、执行vmcs_load指令，即vmptrad指令
	8、执行__vmx_setup_cpu(void),获得cpu的变量host_gdt，执行vmwrite(源操作数为寄存器或内存操作数，提供需要写入vmcs字段的值指令)。//linux uses per-cpu TSS and GDT ,so set these when switching processors
	9、设置vcpu的值为本地cpu
	10、直接vmcs_load指令

static inline void vpid_sync_context(u16 vpid)
	vpid_sync_vcpu_single or vpid_sync_vcpu_global

static inline bool cpu_has_vmx_invvpid_global(void)
	VMX_VPID_EXTENT_GLOBAL_CONTEXT_BIT;

static struct vmcs *__vmcs_alloc_vmcs(int cpu)
	1、vmcs_alloc_vmcs是获得当前的处理器编号后再调用该函数
	2、调用alloc_pages_exact在特定的cpu上分配一页内存，pages
	3、vmcs即指向该页
	4、对vmcs进行初始化并分配revision_id;
