static void xen_load_gs_index(unsigned int idx)
{
	if (HYPERVISOR_set_segment_base(SEGBASE_GS_USER_SEL, idx))
		BUG();
}