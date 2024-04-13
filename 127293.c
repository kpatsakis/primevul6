static inline void tss_update_io_bitmap(void)
{
	PVOP_VCALL0(cpu.update_io_bitmap);
}