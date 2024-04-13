static inline int is_ia32_frame(void)
{
	return config_enabled(CONFIG_X86_32) || is_ia32_compat_frame();
}