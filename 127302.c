static unsigned long xen_get_debugreg(int reg)
{
	return HYPERVISOR_get_debugreg(reg);
}