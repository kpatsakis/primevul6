static bool __init xen_check_xsave(void)
{
	unsigned int cx, xsave_mask;

	cx = cpuid_ecx(1);

	xsave_mask = (1 << (X86_FEATURE_XSAVE % 32)) |
		     (1 << (X86_FEATURE_OSXSAVE % 32));

	/* Xen will set CR4.OSXSAVE if supported and not disabled by force */
	return (cx & xsave_mask) == xsave_mask;
}