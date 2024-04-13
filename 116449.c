static inline void fx_finit(struct i387_fxsave_struct *fx)
{
	memset(fx, 0, xstate_size);
	fx->cwd = 0x37f;
	fx->mxcsr = MXCSR_DEFAULT;
}