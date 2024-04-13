elf_core_copy_task_xfpregs(struct task_struct *t, elf_fpxregset_t *xfpu)
{
	struct pt_regs *regs = task_pt_regs(t);
	if (!tsk_used_math(t))
		return 0;
	if (t == current)
		unlazy_fpu(t); 
	memcpy(xfpu, &t->thread.i387.fxsave, sizeof(elf_fpxregset_t));
	xfpu->fcs = regs->cs; 
	xfpu->fos = t->thread.ds; /* right? */ 
	return 1;
}