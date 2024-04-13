static void __init xen_dom0_set_legacy_features(void)
{
	x86_platform.legacy.rtc = 1;
}