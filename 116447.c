static inline int is_ia32_compat_frame(void)
{
	return config_enabled(CONFIG_IA32_EMULATION) &&
	       test_thread_flag(TIF_IA32);
}