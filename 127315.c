static inline void tss_invalidate_io_bitmap(void)
{
	PVOP_VCALL0(cpu.invalidate_io_bitmap);
}