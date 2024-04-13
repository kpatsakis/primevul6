static inline void switch_to_bitmap(unsigned long tifp)
{
	/*
	 * Invalidate I/O bitmap if the previous task used it. This prevents
	 * any possible leakage of an active I/O bitmap.
	 *
	 * If the next task has an I/O bitmap it will handle it on exit to
	 * user mode.
	 */
	if (tifp & _TIF_IO_BITMAP)
		tss_invalidate_io_bitmap();
}