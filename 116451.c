static inline int fsave_user(struct i387_fsave_struct __user *fx)
{
	return user_insn(fnsave %[fx]; fwait,  [fx] "=m" (*fx), "m" (*fx));
}