static inline void load_gs_index(unsigned int gs)
{
	PVOP_VCALL1(cpu.load_gs_index, gs);
}