dune_procmap_iterate(dune_procmap_cb  cb):
	读取/proc/self/maps里面的映射（查看进程的虚拟地址空间是如何使用的），然后根据提供的cb函数参数，进行cb(&e)对于特定的映射进行操作,其中e的结构为：
struct dune_procmap_entry {
	uintptr_t	begin;//库在进程里的地址范围
	uintptr_t	end;
	uint64_t	offset;//库在进程里的地址偏移量
	bool		r; // Readable
	bool		w; // Writable
	bool		x; // Executable
	bool		p; // Private (or shared)
	char		*path;
	int			type;
};
		


