int dune_page_init(void)
	1����ʼ��ҳͷ&pages_free,page_head.
	2������do_mapping(PAGEBASE,num_pages*PGSIZE)�����ظ�mem;
	3�������ַ��СΪsizeof(struct page)*MAX_PAGES(4GB)�Ŀռ�,������ʾstruct page
	4����3�������pages������pages_freeΪͷ����������ֻ��512����С�ڷ����MAX_PAGES

static void * do_mapping(void *base,unsigned long len)
	����mmap��������base��ʼ�ĳ���Ϊlen���ļ�ӳ����ڴ�����baseΪnull�������ں�ָ��ӳ��ĵ�ַ
