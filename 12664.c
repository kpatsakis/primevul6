static int smack_ipc_permission(struct kern_ipc_perm *ipp, short flag)
{
	struct smack_known **blob = smack_ipc(ipp);
	struct smack_known *iskp = *blob;
	int may = smack_flags_to_may(flag);
	struct smk_audit_info ad;
	int rc;

#ifdef CONFIG_AUDIT
	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_IPC);
	ad.a.u.ipc_id = ipp->id;
#endif
	rc = smk_curacc(iskp, may, &ad);
	rc = smk_bu_current("svipc", iskp, may, rc);
	return rc;
}