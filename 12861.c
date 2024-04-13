static void smack_task_getsecid_obj(struct task_struct *p, u32 *secid)
{
	struct smack_known *skp = smk_of_task_struct_obj(p);

	*secid = skp->smk_secid;
}