int dune_page_init(void)
	1、初始化页头&pages_free,page_head.
	2、调用do_mapping(PAGEBASE,num_pages*PGSIZE)，返回给mem;
	3、分配地址大小为sizeof(struct page)*MAX_PAGES(4GB)的空间,用来表示struct page
	4、由3步分配的pages加入以pages_free为头的链表，这里只加512个，小于分配的MAX_PAGES

static void * do_mapping(void *base,unsigned long len)
	调用mmap函数，将base开始的长度为len的文件映射的内存区域。base为null代表让内核指定映射的地址
