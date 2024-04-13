static void tss_copy_io_bitmap(struct tss_struct *tss, struct io_bitmap *iobm)
{
	/*
	 * Copy at least the byte range of the incoming tasks bitmap which
	 * covers the permitted I/O ports.
	 *
	 * If the previous task which used an I/O bitmap had more bits
	 * permitted, then the copy needs to cover those as well so they
	 * get turned off.
	 */
	memcpy(tss->io_bitmap.bitmap, iobm->bitmap,
	       max(tss->io_bitmap.prev_max, iobm->max));

	/*
	 * Store the new max and the sequence number of this bitmap
	 * and a pointer to the bitmap itself.
	 */
	tss->io_bitmap.prev_max = iobm->max;
	tss->io_bitmap.prev_sequence = iobm->sequence;
}