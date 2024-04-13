void native_tss_update_io_bitmap(void)
{
	struct tss_struct *tss = this_cpu_ptr(&cpu_tss_rw);
	struct thread_struct *t = &current->thread;
	u16 *base = &tss->x86_tss.io_bitmap_base;

	if (!test_thread_flag(TIF_IO_BITMAP)) {
		native_tss_invalidate_io_bitmap();
		return;
	}

	if (IS_ENABLED(CONFIG_X86_IOPL_IOPERM) && t->iopl_emul == 3) {
		*base = IO_BITMAP_OFFSET_VALID_ALL;
	} else {
		struct io_bitmap *iobm = t->io_bitmap;

		/*
		 * Only copy bitmap data when the sequence number differs. The
		 * update time is accounted to the incoming task.
		 */
		if (tss->io_bitmap.prev_sequence != iobm->sequence)
			tss_copy_io_bitmap(tss, iobm);

		/* Enable the bitmap */
		*base = IO_BITMAP_OFFSET_VALID_MAP;
	}

	/*
	 * Make sure that the TSS limit is covering the IO bitmap. It might have
	 * been cut down by a VMEXIT to 0x67 which would cause a subsequent I/O
	 * access from user space to trigger a #GP because tbe bitmap is outside
	 * the TSS limit.
	 */
	refresh_tss_limit();
}