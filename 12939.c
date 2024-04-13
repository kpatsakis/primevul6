static int smack_key_getsecurity(struct key *key, char **_buffer)
{
	struct smack_known *skp = key->security;
	size_t length;
	char *copy;

	if (key->security == NULL) {
		*_buffer = NULL;
		return 0;
	}

	copy = kstrdup(skp->smk_known, GFP_KERNEL);
	if (copy == NULL)
		return -ENOMEM;
	length = strlen(copy) + 1;

	*_buffer = copy;
	return length;
}