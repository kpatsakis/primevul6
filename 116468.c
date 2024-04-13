static inline int frstor_user(struct i387_fsave_struct __user *fx)
{
	return user_insn(frstor %[fx], "=m" (*fx), [fx] "m" (*fx));
}