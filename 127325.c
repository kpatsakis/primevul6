static void __init xen_pv_guest_late_init(void)
{
#ifndef CONFIG_SMP
	/* Setup shared vcpu info for non-smp configurations */
	xen_setup_vcpu_info_placement();
#endif
}