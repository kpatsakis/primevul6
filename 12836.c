static void smack_key_free(struct key *key)
{
	key->security = NULL;
}