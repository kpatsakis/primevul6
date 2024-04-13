static inline int frstor_checking(struct i387_fsave_struct *fx)
{
	return check_insn(frstor %[fx], "=m" (*fx), [fx] "m" (*fx));
}