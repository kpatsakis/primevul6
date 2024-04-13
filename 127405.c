static void xen_invalidate_io_bitmap(void)
{
	struct physdev_set_iobitmap iobitmap = {
		.bitmap = 0,
		.nr_ports = 0,
	};

	native_tss_invalidate_io_bitmap();
	HYPERVISOR_physdev_op(PHYSDEVOP_set_iobitmap, &iobitmap);
}