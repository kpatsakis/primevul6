static __always_inline __pure bool use_eager_fpu(void)
{
	return static_cpu_has(X86_FEATURE_EAGER_FPU);
}