static int fill_psinfo(struct elf_prpsinfo *psinfo, struct task_struct *p,
		       struct mm_struct *mm)
{
	unsigned int i, len;

	/* first copy the parameters from user space */
	memset(psinfo, 0, sizeof(struct elf_prpsinfo));

	len = mm->arg_end - mm->arg_start;
	if (len >= ELF_PRARGSZ)
		len = ELF_PRARGSZ - 1;
	if (copy_from_user(&psinfo->pr_psargs,
		           (const char __user *) mm->arg_start, len))
		return -EFAULT;
	for (i = 0; i < len; i++)
		if (psinfo->pr_psargs[i] == 0)
			psinfo->pr_psargs[i] = ' ';
	psinfo->pr_psargs[len] = 0;

	psinfo->pr_pid = p->pid;
	psinfo->pr_ppid = p->parent->pid;
	psinfo->pr_pgrp = process_group(p);
	psinfo->pr_sid = process_session(p);

	i = p->state ? ffz(~p->state) + 1 : 0;
	psinfo->pr_state = i;
	psinfo->pr_sname = (i > 5) ? '.' : "RSDTZW"[i];
	psinfo->pr_zomb = psinfo->pr_sname == 'Z';
	psinfo->pr_nice = task_nice(p);
	psinfo->pr_flag = p->flags;
	SET_UID(psinfo->pr_uid, p->uid);
	SET_GID(psinfo->pr_gid, p->gid);
	strncpy(psinfo->pr_fname, p->comm, sizeof(psinfo->pr_fname));

	return 0;
}