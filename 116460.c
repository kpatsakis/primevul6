static __always_inline __pure bool use_xsave(void)
{
	return static_cpu_has(X86_FEATURE_XSAVE);
}