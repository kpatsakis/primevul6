static inline int fxrstor_checking(struct i387_fxsave_struct *fx)
{
	if (config_enabled(CONFIG_X86_32))
		return check_insn(fxrstor %[fx], "=m" (*fx), [fx] "m" (*fx));
	else if (config_enabled(CONFIG_AS_FXSAVEQ))
		return check_insn(fxrstorq %[fx], "=m" (*fx), [fx] "m" (*fx));

	/* See comment in fpu_fxsave() below. */
	return check_insn(rex64/fxrstor (%[fx]), "=m" (*fx), [fx] "R" (fx),
			  "m" (*fx));
}