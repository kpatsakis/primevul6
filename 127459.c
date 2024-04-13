static unsigned char xen_get_nmi_reason(void)
{
	unsigned char reason = 0;

	/* Construct a value which looks like it came from port 0x61. */
	if (test_bit(_XEN_NMIREASON_io_error,
		     &HYPERVISOR_shared_info->arch.nmi_reason))
		reason |= NMI_REASON_IOCHK;
	if (test_bit(_XEN_NMIREASON_pci_serr,
		     &HYPERVISOR_shared_info->arch.nmi_reason))
		reason |= NMI_REASON_SERR;

	return reason;
}