static void __init xen_banner(void)
{
	unsigned version = HYPERVISOR_xen_version(XENVER_version, NULL);
	struct xen_extraversion extra;
	HYPERVISOR_xen_version(XENVER_extraversion, &extra);

	pr_info("Booting paravirtualized kernel on %s\n", pv_info.name);
	printk(KERN_INFO "Xen version: %d.%d%s%s\n",
	       version >> 16, version & 0xffff, extra.extraversion,
	       xen_feature(XENFEAT_mmu_pt_update_preserve_ad) ? " (preserve-AD)" : "");

#ifdef CONFIG_X86_32
	pr_warn("WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING!\n"
		"Support for running as 32-bit PV-guest under Xen will soon be removed\n"
		"from the Linux kernel!\n"
		"Please use either a 64-bit kernel or switch to HVM or PVH mode!\n"
		"WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING!\n");
#endif
}