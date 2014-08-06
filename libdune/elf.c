static int elf_read(struct dune_elf *elf,void *dst,int len,int offset)
	1、如果elf->mem为空，返回读取elf->fd(文件)的结果
	2、求需要读取的长度：为dr=min(elf->len-off,len);
	3、赋值elf->mem的偏移量off开始的dr的长度为dst

static int do_elf_open(struct dune_elf *elf)
	1、elf_read函数，将elf中的东西读入Elf64_Ehdr hdr;
typedef struct
{
  unsigned char	e_ident[EI_NIDENT];	/* Magic number and other info */
  Elf64_Half	e_type;			/* Object file type */
  Elf64_Half	e_machine;		/* Architecture */
  Elf64_Word	e_version;		/* Object file version */
  Elf64_Addr	e_entry;		/* Entry point virtual address */
  Elf64_Off	    e_phoff;		/* Program header table file offset */
  Elf64_Off	    e_shoff;		/* Section header table file offset */
  Elf64_Word	e_flags;		/* Processor-specific flags */
  Elf64_Half	e_ehsize;		/* ELF header size in bytes */
  Elf64_Half	e_phentsize;		/* Program header table entry size */
  Elf64_Half	e_phnum;		/* Program header table entry count */
  Elf64_Half	e_shentsize;		/* Section header table entry size */
  Elf64_Half	e_shnum;		/* Section header table entry count */
  Elf64_Half	e_shstrndx;		/* Section header string table index */
} Elf64_Ehdr;

typedef struct
{
  Elf64_Word	sh_name;		/* Section name (string tbl index) */
  Elf64_Word	sh_type;		/* Section type */
  Elf64_Xword	sh_flags;		/* Section flags */
  Elf64_Addr	sh_addr;		/* Section virtual addr at execution */
  Elf64_Off	    sh_offset;		/* Section file offset */
  Elf64_Xword	sh_size;		/* Section size in bytes */
  Elf64_Word	sh_link;		/* Link to another section */
  Elf64_Word	sh_info;		/* Additional section information */
  Elf64_Xword	sh_addralign;		/* Section alignment */
  Elf64_Xword	sh_entsize;		/* Entry size if section holds table */
} Elf64_Shdr;

static int vdso_sh_cb(struct dune_elf *elf,const char *sname ,int snum,Elf64_Shdr *shdr)
	//ds的理解不够充分
    1、	如果shdr.sh_type为String table并且sname为".dynstr":
	   1.1、ds=elf->prev
	   1.2、循环对每个ds的ds_name重新指向一个新的地址:elf->mem+shdr->sh_offset+ds->ds_idx
	2、如果shdr.sh_type为dynamic linker symbol table:
	   2.1、求得shdr的size(len)和shdr的mem(s)
	   
int dune_elf_iter_sh(struct dune_elf *elf,dune_elf_shcb cb)
	//shdr为section head table  strtab为shdr的关于section table string table的部分
	1、查看elf的shdr(section header)是否为空，如果空的话，就调用elf_open_shs(elf):首先检查setction header table entry size and count,然后求出总长度，将elf->mem(fd)开始的偏移量为elf->hdr.e_shoff的长度为求得的总长度写进shdr
    2、//检查elf->hdr.e_shstrndx>elf->hdr.e_shnum ,则错误
    3、//检查section type for string table,stringtable's type is SHT_STRTAB
	4、分配一个长度为section size(shdr[elf->hdr.e_shstrndx].sh_size)的空间为strtab//shdr string table 的size为strtablen
    5、用elf_read函数将section的内容读近strtab
    6、将shdr和shdrstr的值赋值给elf的shdr和shdrstr


static void setup_vdso(void *addr,size_t len)
	1、elf就行清空初始化，分配ds标示符
	2、调用dune_elf_open_mem(&elf,addr,len)函数，将mem和len等赋值给elf并且将addr开始的长为sizeof(Elf64_Ehdr)的数据读入Elf64_Ehdr结构体elf->hdr
	3、//elf.hdr.e_type需要满足的条件
	
