static int selinux_key_permission(key_ref_t key_ref,
				  const struct cred *cred,
				  enum key_need_perm need_perm)
{
	struct key *key;
	struct key_security_struct *ksec;
	u32 perm, sid;

	switch (need_perm) {
	case KEY_NEED_VIEW:
		perm = KEY__VIEW;
		break;
	case KEY_NEED_READ:
		perm = KEY__READ;
		break;
	case KEY_NEED_WRITE:
		perm = KEY__WRITE;
		break;
	case KEY_NEED_SEARCH:
		perm = KEY__SEARCH;
		break;
	case KEY_NEED_LINK:
		perm = KEY__LINK;
		break;
	case KEY_NEED_SETATTR:
		perm = KEY__SETATTR;
		break;
	case KEY_NEED_UNLINK:
	case KEY_SYSADMIN_OVERRIDE:
	case KEY_AUTHTOKEN_OVERRIDE:
	case KEY_DEFER_PERM_CHECK:
		return 0;
	default:
		WARN_ON(1);
		return -EPERM;

	}

	sid = cred_sid(cred);
	key = key_ref_to_ptr(key_ref);
	ksec = key->security;

	return avc_has_perm(&selinux_state,
			    sid, ksec->sid, SECCLASS_KEY, perm, NULL);
}