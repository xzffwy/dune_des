1、dune_vm_map_phys(ptent_t *root,void *va, size_t len, void *pa ,int perm)
	通过perm的值对create赋值，确定需要映射的物理页的大小，data用来存储va,pa,调用函数:
	__dune_vm_page_work(root,va,va+len-1,&__dune_vm_map_phys_helper,(void *)&data,3,creat);

2、static int __dune_vm_page_walk(ptent_t *dir,void start_va,void *end_va,page_walk_cb cb,const void *arg,int level,int create)//也不是虚拟地址吧？？
	level用来指定分页的级别吧
	1、通过level获得start_idx,和end_idx,代表需要映射的关于局部表的表项索引
	2、获得base_va，即全局表地址？？？？
	3、做一些基本的数据范围判断//NPTENTRIES不懂的
	4、从start_idx到end_idx做循环：
	  4.1、获得当前的cur_va，pte=&dir[i];
	  4.2、如果level为0并且create==Normal 或者pte为空，pte算出正常的页的物理地址cur_va-va+pa
	  4.3、如果level为1并且create==Big或者标记为big，则pte算出big的页的物理地址//????????
	  4.4、如果level为2并且create==BIG_1gb或者pte标记为big，这算出ptg的页的物理地址
	  4.5、如果当前pte没有指向一个页并且create不为0：调用alloc_page分配的已经映射的一个物理page，就行清空，然后mask低12赋值给pte
	  4.6、再一次求得start_va,end_va,进行循环知道level为-1

static inline void * alloc_page(void)
	1、调用dune_page_alloc(),返回一个页的描述符的物理地址//代表此页被程序使用？？？？？
	2、调用dune_page2pa(pg)返回pg的虚拟地址

int dune_vm_lookup(ptent_t *root, void *va,int create,ptent_t **ptent_out)
	1、通过va获得一级二级三级四级页表的偏移量i,j,k,l
	2、pml4为LDT，由va赋值
	3、一次检测各级页表是否为空，如果为空，就为该项调用alloc_page()分配页面，并清空。并且还要注意扩展分页以及大页的分配
	4、将最后找到的页面的指针赋值给*ptent_out

