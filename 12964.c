static void smack_cred_getsecid(const struct cred *cred, u32 *secid)
{
	struct smack_known *skp;

	rcu_read_lock();
	skp = smk_of_task(smack_cred(cred));
	*secid = skp->smk_secid;
	rcu_read_unlock();
}