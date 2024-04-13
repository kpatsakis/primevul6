static void xen_cpuid(unsigned int *ax, unsigned int *bx,
		      unsigned int *cx, unsigned int *dx)
{
	unsigned maskebx = ~0;

	/*
	 * Mask out inconvenient features, to try and disable as many
	 * unsupported kernel subsystems as possible.
	 */
	switch (*ax) {
	case CPUID_MWAIT_LEAF:
		/* Synthesize the values.. */
		*ax = 0;
		*bx = 0;
		*cx = cpuid_leaf5_ecx_val;
		*dx = cpuid_leaf5_edx_val;
		return;

	case 0xb:
		/* Suppress extended topology stuff */
		maskebx = 0;
		break;
	}

	asm(XEN_EMULATE_PREFIX "cpuid"
		: "=a" (*ax),
		  "=b" (*bx),
		  "=c" (*cx),
		  "=d" (*dx)
		: "0" (*ax), "2" (*cx));

	*bx &= maskebx;
}