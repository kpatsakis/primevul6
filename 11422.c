add_banned(
    suginfo_T	*su,
    char_u	*word)
{
    char_u	*s;
    hash_T	hash;
    hashitem_T	*hi;

    hash = hash_hash(word);
    hi = hash_lookup(&su->su_banned, word, hash);
    if (HASHITEM_EMPTY(hi))
    {
	s = vim_strsave(word);
	if (s != NULL)
	    hash_add_item(&su->su_banned, hi, s, hash);
    }
}