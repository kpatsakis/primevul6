static void xen_write_cr4(unsigned long cr4)
{
	cr4 &= ~(X86_CR4_PGE | X86_CR4_PSE | X86_CR4_PCE);

	native_write_cr4(cr4);
}