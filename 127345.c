static inline void set_ldt(const void *addr, unsigned entries)
{
	PVOP_VCALL2(cpu.set_ldt, addr, entries);
}