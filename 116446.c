static inline int xstate_sigframe_size(void)
{
	return use_xsave() ? xstate_size + FP_XSTATE_MAGIC2_SIZE : xstate_size;
}