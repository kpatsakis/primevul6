static __always_inline void amd_set_ssb_virt_state(unsigned long tifn)
{
	/*
	 * SSBD has the same definition in SPEC_CTRL and VIRT_SPEC_CTRL,
	 * so ssbd_tif_to_spec_ctrl() just works.
	 */
	wrmsrl(MSR_AMD64_VIRT_SPEC_CTRL, ssbd_tif_to_spec_ctrl(tifn));
}