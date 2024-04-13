static inline int fxsave_user(struct i387_fxsave_struct __user *fx)
{
	if (config_enabled(CONFIG_X86_32))
		return user_insn(fxsave %[fx], [fx] "=m" (*fx), "m" (*fx));
	else if (config_enabled(CONFIG_AS_FXSAVEQ))
		return user_insn(fxsaveq %[fx], [fx] "=m" (*fx), "m" (*fx));

	/* See comment in fpu_fxsave() below. */
	return user_insn(rex64/fxsave (%[fx]), "=m" (*fx), [fx] "R" (fx));
}