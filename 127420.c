static inline void slow_down_io(void)
{
	pv_ops.cpu.io_delay();
#ifdef REALLY_SLOW_IO
	pv_ops.cpu.io_delay();
	pv_ops.cpu.io_delay();
	pv_ops.cpu.io_delay();
#endif
}