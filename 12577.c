static int smk_curacc_shm(struct kern_ipc_perm *isp, int access)
{
	struct smack_known *ssp = smack_of_ipc(isp);
	struct smk_audit_info ad;
	int rc;

#ifdef CONFIG_AUDIT
	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_IPC);
	ad.a.u.ipc_id = isp->id;
#endif
	rc = smk_curacc(ssp, access, &ad);
	rc = smk_bu_current("shm", ssp, access, rc);
	return rc;
}