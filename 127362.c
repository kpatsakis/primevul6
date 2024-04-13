void arch_cpu_idle_enter(void)
{
	tsc_verify_tsc_adjust(false);
	local_touch_nmi();
}