static int elf_dump_thread_status(long signr, struct elf_thread_status *t)
{
	struct task_struct *p = t->thread;
	int sz = 0;

	t->num_notes = 0;

	fill_prstatus(&t->prstatus, p, signr);
	elf_core_copy_task_regs(p, &t->prstatus.pr_reg);

	fill_note(&t->notes[0], "CORE", NT_PRSTATUS, sizeof(t->prstatus),
		  &t->prstatus);
	t->num_notes++;
	sz += notesize(&t->notes[0]);

	t->prstatus.pr_fpvalid = elf_core_copy_task_fpregs(p, NULL, &t->fpu);
	if (t->prstatus.pr_fpvalid) {
		fill_note(&t->notes[1], "CORE", NT_PRFPREG, sizeof(t->fpu),
			  &t->fpu);
		t->num_notes++;
		sz += notesize(&t->notes[1]);
	}

#ifdef ELF_CORE_COPY_XFPREGS
	if (elf_core_copy_task_xfpregs(p, &t->xfpu)) {
		fill_note(&t->notes[2], "LINUX", NT_PRXFPREG, sizeof(t->xfpu),
			  &t->xfpu);
		t->num_notes++;
		sz += notesize(&t->notes[2]);
	}
#endif
	return sz;
}