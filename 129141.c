static inline int elf_core_copy_task_regs(struct task_struct *t, elf_gregset_t* elfregs)
{	
	struct pt_regs *pp = task_pt_regs(t);
	ELF_CORE_COPY_REGS((*elfregs), pp);
	/* fix wrong segments */ 
	(*elfregs)[7] = t->thread.ds; 
	(*elfregs)[9] = t->thread.fsindex; 
	(*elfregs)[10] = t->thread.gsindex; 
	(*elfregs)[8] = t->thread.es; 	
	return 1; 
}