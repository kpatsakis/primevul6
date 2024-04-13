static int selinux_watch_key(struct key *key)
{
	struct key_security_struct *ksec = key->security;
	u32 sid = current_sid();

	return avc_has_perm(&selinux_state,
			    sid, ksec->sid, SECCLASS_KEY, KEY__VIEW, NULL);
}