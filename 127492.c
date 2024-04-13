static uint32_t __init xen_platform_pv(void)
{
	if (xen_pv_domain())
		return xen_cpuid_base();

	return 0;
}