__init int vmx_init(void)
	1、检测系统是否支持vt-x，内核提供的函数
	2、//初始化系统调用
	3、配置vmcs_config，每个程序进入dune模式都需要检查，而这里一次性检查记录配置
	4、检测cpu是否支持vpid
	5、检测cpu是否支持ept，虚拟化的扩展分页
	6、//ability to load EFER register,64位扩展技术，在哪里用，还没有发现
	7、//msr_bitmap中的而一切操作还不懂
	8、初始化vpid的map
	9、给每个cpu分配vmxon区域，并初始化，特别是revision_id
	10、在每一个cpu上执行vmx_enable函数：调用__vmx_enable函数，设置MSR_IA32_FEATURE_CONTROL和寄存器CR4的关于启动vmxon的bit，然后执行vmxon指令，//vpid_sync_vcpu_global,ept_sync_global，还有存储gdt，没有见对host_gdt赋值
__pa函数
int vmx_launch(struct dune_config *conf,int64_t *ret_code)
	1、创建vcpu:vmx_create_vcpu(conf);
	2、vmx_get_cpu(),获取当前cpu，禁止内核抢占，进行vcpu的配置：cpu，clear
	3、dune默认每个进程都是用fpu的，所以如果现在的进程没有用fpu，则调用math_state_restore()函数装在fpu(restore)
	4、阻止本地中断
	5、如果需要调度，激活本地中，重启内核抢占，重新调度程序
	6、检查当前信号是否有信号需要处理，如果有：
	   6.1、激活本地中断，启动内核抢占
	   6.2、启动保护信号描述符和信号处理程序的自旋锁
	   6.3、调用dequeue_signal函数取得一个滞留的进程(先pending后share_pending);
	   6.4、解除保护信号描述符和信号处理程序的自旋锁
	   6.5、如果信号为SIGKILL，赋值vcpu->ret_code，退出
	   6.6、设置关于信号的信息，写入VM_ENTRY_INFO_FIELD
    7、vmx_run_vcpu，执行vmx_launch(),正式进入vmx no-root	
	   //保存host state restore guest state
	8、激活本地中断
	9、//根据vmx_run_vcpu返回的ret值进行一系列操作，这个还不是很懂得

//1个cpu只能有一个vcpu吗？
static struct vmx_vcpu * vmx_create_vcpu(struct dune_config *conf)
	struct vmx_vcpu {                         1、分配vcpu结构体的内存并初始化
		int cpu;                              2、获得处理器的编号，分配vmcs区域
		int vpid;                             3、为vcpu分配vpid,在vmx_vpid_bitmap基础上
		int launched;                         4、//初始化vcpu的cpu为-1，和syscall_tbl
                                              5、初始化ept，分配一个空闲页，返回物理地址
		struct mmu_notifier mmu_notifier;        vcpu->ept_root
		spinlock_t ept_lock;                  6、vcpu->eptp=construct_eptp(vcpu->ept_root);//不懂
		unsigned long ept_root;               7、执行vmx_get_cpu(vcpu),用来加载current vmcs,并为vcpu的cpu指 
		unsigned long eptp;                      定现在的cpu ,配置
		bool ept_ad_enabled;                  8、调用vmcs_setup_vmcs，配置vm-execution控制区域（检查在之前。
                                                 已经完成）以及对host_state进行配置写入
		u8  fail;                             9、调用vmx_setup_initial_guest_state,检查guest-state区域赋值
		u64 exit_reason;                      10、调用vmx_put_cpu使得preempt_enable
		u64 host_rsp;                         11、判断cpu_has_vmx_ept_ad_bits
		u64 regs[NR_VCPU_REGS];               12、vmx_create_ept(vcpu):调用mmu_notifier_register;//太乱了
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
	1、获取当前处理器，并且禁止内核抢占,get_cpu();
	2、设置现在用的cpu的local_vcpu为vcpu//一个cpu只能设置一个vcpu吗？
	3、如果vcpu的cpu不是当前运行的cpu(清空vcpu的vmcs，并且使其处在cpu没确定的情况)，否则10：
	   3.1、如果vcpu已经有cpu，则在其vcpu->cpu上运行函数__vmx_get_cpu_helper(vcpu):清空vcpu的vmcs，并且将运行的cpu上的local_vcpu置NULL
	   3.2、负责就直接清空vcpu的vmcs
	   在vmlaunch之前必须先vmclear
	4、//vpid_sync_context(vcpu->vpid);设置vpid为single或者global，ASM_VMX_INVVPID指令不是很清楚，作用不懂
	5、//ept_sync_context(vcpu->eptp);同上，ASM_VMX_INVEPT指令
	6、launchce=0；
	7、执行vmcs_load指令，即vmptrad指令
	8、// __vmx_setup_cpu执行__vmx_setup_cpu(void),获得cpu的变量host_gdt，执行vmwrite(源操作数为寄存器或内存操作数，提供需要写入vmcs字段的值指令)。//linux uses per-cpu TSS and GDT ,so set these when switching processors
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
