static inline int fpu_save_init(struct fpu *fpu)
{
	if (use_xsave()) {
		fpu_xsave(fpu);

		/*
		 * xsave header may indicate the init state of the FP.
		 */
		if (!(fpu->state->xsave.xsave_hdr.xstate_bv & XSTATE_FP))
			return 1;
	} else if (use_fxsr()) {
		fpu_fxsave(fpu);
	} else {
		asm volatile("fnsave %[fx]; fwait"
			     : [fx] "=m" (fpu->state->fsave));
		return 0;
	}

	/*
	 * If exceptions are pending, we need to clear them so
	 * that we don't randomly get exceptions later.
	 *
	 * FIXME! Is this perhaps only true for the old-style
	 * irq13 case? Maybe we could leave the x87 state
	 * intact otherwise?
	 */
	if (unlikely(fpu->state->fxsave.swd & X87_FSW_ES)) {
		asm volatile("fnclex");
		return 0;
	}
	return 1;
}