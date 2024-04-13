static int smack_syslog(int typefrom_file)
{
	int rc = 0;
	struct smack_known *skp = smk_of_current();

	if (smack_privileged(CAP_MAC_OVERRIDE))
		return 0;

	if (smack_syslog_label != NULL && smack_syslog_label != skp)
		rc = -EACCES;

	return rc;
}