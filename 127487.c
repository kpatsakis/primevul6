static int get_cpuid_mode(void)
{
	return !test_thread_flag(TIF_NOCPUID);
}