static void xen_set_debugreg(int reg, unsigned long val)
{
	HYPERVISOR_set_debugreg(reg, val);
}