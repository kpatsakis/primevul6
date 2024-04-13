elf_core_copy_task_fpregs(struct task_struct *tsk, struct pt_regs *regs, elf_fpregset_t *fpu)
{
	struct _fpstate_ia32 *fpstate = (void*)fpu; 
	mm_segment_t oldfs = get_fs();

	if (!tsk_used_math(tsk))
		return 0;
	if (!regs)
		regs = task_pt_regs(tsk);
	if (tsk == current)
		unlazy_fpu(tsk);
	set_fs(KERNEL_DS); 
	save_i387_ia32(tsk, fpstate, regs, 1);
	/* Correct for i386 bug. It puts the fop into the upper 16bits of 
	   the tag word (like FXSAVE), not into the fcs*/ 
	fpstate->cssel |= fpstate->tag & 0xffff0000; 
	set_fs(oldfs); 
	return 1; 
}