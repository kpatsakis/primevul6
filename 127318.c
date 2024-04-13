static bool __init xen_check_mwait(void)
{
#ifdef CONFIG_ACPI
	struct xen_platform_op op = {
		.cmd			= XENPF_set_processor_pminfo,
		.u.set_pminfo.id	= -1,
		.u.set_pminfo.type	= XEN_PM_PDC,
	};
	uint32_t buf[3];
	unsigned int ax, bx, cx, dx;
	unsigned int mwait_mask;

	/* We need to determine whether it is OK to expose the MWAIT
	 * capability to the kernel to harvest deeper than C3 states from ACPI
	 * _CST using the processor_harvest_xen.c module. For this to work, we
	 * need to gather the MWAIT_LEAF values (which the cstate.c code
	 * checks against). The hypervisor won't expose the MWAIT flag because
	 * it would break backwards compatibility; so we will find out directly
	 * from the hardware and hypercall.
	 */
	if (!xen_initial_domain())
		return false;

	/*
	 * When running under platform earlier than Xen4.2, do not expose
	 * mwait, to avoid the risk of loading native acpi pad driver
	 */
	if (!xen_running_on_version_or_later(4, 2))
		return false;

	ax = 1;
	cx = 0;

	native_cpuid(&ax, &bx, &cx, &dx);

	mwait_mask = (1 << (X86_FEATURE_EST % 32)) |
		     (1 << (X86_FEATURE_MWAIT % 32));

	if ((cx & mwait_mask) != mwait_mask)
		return false;

	/* We need to emulate the MWAIT_LEAF and for that we need both
	 * ecx and edx. The hypercall provides only partial information.
	 */

	ax = CPUID_MWAIT_LEAF;
	bx = 0;
	cx = 0;
	dx = 0;

	native_cpuid(&ax, &bx, &cx, &dx);

	/* Ask the Hypervisor whether to clear ACPI_PDC_C_C2C3_FFH. If so,
	 * don't expose MWAIT_LEAF and let ACPI pick the IOPORT version of C3.
	 */
	buf[0] = ACPI_PDC_REVISION_ID;
	buf[1] = 1;
	buf[2] = (ACPI_PDC_C_CAPABILITY_SMP | ACPI_PDC_EST_CAPABILITY_SWSMP);

	set_xen_guest_handle(op.u.set_pminfo.pdc, buf);

	if ((HYPERVISOR_platform_op(&op) == 0) &&
	    (buf[2] & (ACPI_PDC_C_C1_FFH | ACPI_PDC_C_C2C3_FFH))) {
		cpuid_leaf5_ecx_val = cx;
		cpuid_leaf5_edx_val = dx;
	}
	return true;
#else
	return false;
#endif
}