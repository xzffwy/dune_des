dune_procmap_iterate(dune_procmap_cb  cb):
	��ȡ/proc/self/maps�����ӳ�䣨�鿴���̵������ַ�ռ������ʹ�õģ���Ȼ������ṩ��cb��������������cb(&e)�����ض���ӳ����в���,����e�ĽṹΪ��
struct dune_procmap_entry {
	uintptr_t	begin;//���ڽ�����ĵ�ַ��Χ
	uintptr_t	end;
	uint64_t	offset;//���ڽ�����ĵ�ַƫ����
	bool		r; // Readable
	bool		w; // Writable
	bool		x; // Executable
	bool		p; // Private (or shared)
	char		*path;
	int			type;
};
		


