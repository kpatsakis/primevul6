static int smack_ptrace_access_check(struct task_struct *ctp, unsigned int mode)
{
	struct smack_known *skp;

	skp = smk_of_task_struct_obj(ctp);

	return smk_ptrace_rule_check(current, skp, mode, __func__);
}