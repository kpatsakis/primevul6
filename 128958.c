static void dump_thread32(struct pt_regs * regs, struct user32 * dump)
{
	u32 fs,gs;

/* changed the size calculations - should hopefully work better. lbt */
	dump->magic = CMAGIC;
	dump->start_code = 0;
	dump->start_stack = regs->rsp & ~(PAGE_SIZE - 1);
	dump->u_tsize = ((unsigned long) current->mm->end_code) >> PAGE_SHIFT;
	dump->u_dsize = ((unsigned long) (current->mm->brk + (PAGE_SIZE-1))) >> PAGE_SHIFT;
	dump->u_dsize -= dump->u_tsize;
	dump->u_ssize = 0;
	dump->u_debugreg[0] = current->thread.debugreg0;  
	dump->u_debugreg[1] = current->thread.debugreg1;  
	dump->u_debugreg[2] = current->thread.debugreg2;  
	dump->u_debugreg[3] = current->thread.debugreg3;  
	dump->u_debugreg[4] = 0;  
	dump->u_debugreg[5] = 0;  
	dump->u_debugreg[6] = current->thread.debugreg6;  
	dump->u_debugreg[7] = current->thread.debugreg7;  

	if (dump->start_stack < 0xc0000000)
		dump->u_ssize = ((unsigned long) (0xc0000000 - dump->start_stack)) >> PAGE_SHIFT;

	dump->regs.ebx = regs->rbx;
	dump->regs.ecx = regs->rcx;
	dump->regs.edx = regs->rdx;
	dump->regs.esi = regs->rsi;
	dump->regs.edi = regs->rdi;
	dump->regs.ebp = regs->rbp;
	dump->regs.eax = regs->rax;
	dump->regs.ds = current->thread.ds;
	dump->regs.es = current->thread.es;
	asm("movl %%fs,%0" : "=r" (fs)); dump->regs.fs = fs;
	asm("movl %%gs,%0" : "=r" (gs)); dump->regs.gs = gs; 
	dump->regs.orig_eax = regs->orig_rax;
	dump->regs.eip = regs->rip;
	dump->regs.cs = regs->cs;
	dump->regs.eflags = regs->eflags;
	dump->regs.esp = regs->rsp;
	dump->regs.ss = regs->ss;

#if 1 /* FIXME */
	dump->u_fpvalid = 0;
#else
	dump->u_fpvalid = dump_fpu (regs, &dump->i387);
#endif
}