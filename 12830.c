static int smk_curacc_msq(struct kern_ipc_perm *isp, int access)
{
	struct smack_known *msp = smack_of_ipc(isp);
	struct smk_audit_info ad;
	int rc;

#ifdef CONFIG_AUDIT
	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_IPC);
	ad.a.u.ipc_id = isp->id;
#endif
	rc = smk_curacc(msp, access, &ad);
	rc = smk_bu_current("msq", msp, access, rc);
	return rc;
}