static unsigned long xen_read_cr0(void)
{
	unsigned long cr0 = this_cpu_read(xen_cr0_value);

	if (unlikely(cr0 == 0)) {
		cr0 = native_read_cr0();
		this_cpu_write(xen_cr0_value, cr0);
	}

	return cr0;
}