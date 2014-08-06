static int elf_read(struct dune_elf *elf,void *dst,int len,int offset)
	1�����elf->memΪ�գ����ض�ȡelf->fd(�ļ�)�Ľ��
	2������Ҫ��ȡ�ĳ��ȣ�Ϊdr=min(elf->len-off,len);
	3����ֵelf->mem��ƫ����off��ʼ��dr�ĳ���Ϊdst

static int do_elf_open(struct dune_elf *elf)
	1��elf_read��������elf�еĶ�������Elf64_Ehdr hdr;
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
	//ds����ⲻ�����
    1��	���shdr.sh_typeΪString table����snameΪ".dynstr":
	   1.1��ds=elf->prev
	   1.2��ѭ����ÿ��ds��ds_name����ָ��һ���µĵ�ַ:elf->mem+shdr->sh_offset+ds->ds_idx
	2�����shdr.sh_typeΪdynamic linker symbol table:
	   2.1�����shdr��size(len)��shdr��mem(s)
	   
int dune_elf_iter_sh(struct dune_elf *elf,dune_elf_shcb cb)
	//shdrΪsection head table  strtabΪshdr�Ĺ���section table string table�Ĳ���
	1���鿴elf��shdr(section header)�Ƿ�Ϊ�գ�����յĻ����͵���elf_open_shs(elf):���ȼ��setction header table entry size and count,Ȼ������ܳ��ȣ���elf->mem(fd)��ʼ��ƫ����Ϊelf->hdr.e_shoff�ĳ���Ϊ��õ��ܳ���д��shdr
    2��//���elf->hdr.e_shstrndx>elf->hdr.e_shnum ,�����
    3��//���section type for string table,stringtable's type is SHT_STRTAB
	4������һ������Ϊsection size(shdr[elf->hdr.e_shstrndx].sh_size)�Ŀռ�Ϊstrtab//shdr string table ��sizeΪstrtablen
    5����elf_read������section�����ݶ���strtab
    6����shdr��shdrstr��ֵ��ֵ��elf��shdr��shdrstr


static void setup_vdso(void *addr,size_t len)
	1��elf������ճ�ʼ��������ds��ʾ��
	2������dune_elf_open_mem(&elf,addr,len)��������mem��len�ȸ�ֵ��elf���ҽ�addr��ʼ�ĳ�Ϊsizeof(Elf64_Ehdr)�����ݶ���Elf64_Ehdr�ṹ��elf->hdr
	3��//elf.hdr.e_type��Ҫ���������
	
