static inline int is_x32_frame(void)
{
	return config_enabled(CONFIG_X86_X32_ABI) && test_thread_flag(TIF_X32);
}