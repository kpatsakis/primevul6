static void elf32_init(struct pt_regs *regs)
{
	struct task_struct *me = current; 
	regs->rdi = 0;
	regs->rsi = 0;
	regs->rdx = 0;
	regs->rcx = 0;
	regs->rax = 0;
	regs->rbx = 0; 
	regs->rbp = 0; 
	regs->r8 = regs->r9 = regs->r10 = regs->r11 = regs->r12 =
		regs->r13 = regs->r14 = regs->r15 = 0; 
    me->thread.fs = 0; 
	me->thread.gs = 0;
	me->thread.fsindex = 0; 
	me->thread.gsindex = 0;
    me->thread.ds = __USER_DS; 
	me->thread.es = __USER_DS;
}