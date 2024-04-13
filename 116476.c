static inline int fpu_restore_checking(struct fpu *fpu)
{
	if (use_xsave())
		return fpu_xrstor_checking(&fpu->state->xsave);
	else if (use_fxsr())
		return fxrstor_checking(&fpu->state->fxsave);
	else
		return frstor_checking(&fpu->state->fsave);
}