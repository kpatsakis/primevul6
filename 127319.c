static void xen_update_io_bitmap(void)
{
	struct physdev_set_iobitmap iobitmap;
	struct tss_struct *tss = this_cpu_ptr(&cpu_tss_rw);

	native_tss_update_io_bitmap();

	iobitmap.bitmap = (uint8_t *)(&tss->x86_tss) +
			  tss->x86_tss.io_bitmap_base;
	if (tss->x86_tss.io_bitmap_base == IO_BITMAP_OFFSET_INVALID)
		iobitmap.nr_ports = 0;
	else
		iobitmap.nr_ports = IO_BITMAP_BITS;

	HYPERVISOR_physdev_op(PHYSDEVOP_set_iobitmap, &iobitmap);
}