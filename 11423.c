similar_chars(slang_T *slang, int c1, int c2)
{
    int		m1, m2;
    char_u	buf[MB_MAXBYTES + 1];
    hashitem_T  *hi;

    if (c1 >= 256)
    {
	buf[mb_char2bytes(c1, buf)] = 0;
	hi = hash_find(&slang->sl_map_hash, buf);
	if (HASHITEM_EMPTY(hi))
	    m1 = 0;
	else
	    m1 = mb_ptr2char(hi->hi_key + STRLEN(hi->hi_key) + 1);
    }
    else
	m1 = slang->sl_map_array[c1];
    if (m1 == 0)
	return FALSE;


    if (c2 >= 256)
    {
	buf[mb_char2bytes(c2, buf)] = 0;
	hi = hash_find(&slang->sl_map_hash, buf);
	if (HASHITEM_EMPTY(hi))
	    m2 = 0;
	else
	    m2 = mb_ptr2char(hi->hi_key + STRLEN(hi->hi_key) + 1);
    }
    else
	m2 = slang->sl_map_array[c2];

    return m1 == m2;
}