static inline void fpu_fxsave(struct fpu *fpu)
{
	if (config_enabled(CONFIG_X86_32))
		asm volatile( "fxsave %[fx]" : [fx] "=m" (fpu->state->fxsave));
	else if (config_enabled(CONFIG_AS_FXSAVEQ))
		asm volatile("fxsaveq %0" : "=m" (fpu->state->fxsave));
	else {
		/* Using "rex64; fxsave %0" is broken because, if the memory
		 * operand uses any extended registers for addressing, a second
		 * REX prefix will be generated (to the assembler, rex64
		 * followed by semicolon is a separate instruction), and hence
		 * the 64-bitness is lost.
		 *
		 * Using "fxsaveq %0" would be the ideal choice, but is only
		 * supported starting with gas 2.16.
		 *
		 * Using, as a workaround, the properly prefixed form below
		 * isn't accepted by any binutils version so far released,
		 * complaining that the same type of prefix is used twice if
		 * an extended register is needed for addressing (fix submitted
		 * to mainline 2005-11-21).
		 *
		 *  asm volatile("rex64/fxsave %0" : "=m" (fpu->state->fxsave));
		 *
		 * This, however, we can work around by forcing the compiler to
		 * select an addressing mode that doesn't require extended
		 * registers.
		 */
		asm volatile( "rex64/fxsave (%[fx])"
			     : "=m" (fpu->state->fxsave)
			     : [fx] "R" (&fpu->state->fxsave));
	}
}