GF_Err chpl_Read(GF_Box *s,GF_BitStream *bs)
{
	GF_ChapterEntry *ce;
	u32 nb_chaps, len, i, count;
	GF_ChapterListBox *ptr = (GF_ChapterListBox *)s;

	/*reserved or ???*/
	gf_bs_read_u32(bs);
	nb_chaps = gf_bs_read_u8(bs);

	count = 0;
	while (nb_chaps) {
		GF_SAFEALLOC(ce, GF_ChapterEntry);
		if (!ce) return GF_OUT_OF_MEM;
		ce->start_time = gf_bs_read_u64(bs);
		len = gf_bs_read_u8(bs);
		if (len) {
			ce->name = (char *)gf_malloc(sizeof(char)*(len+1));
			gf_bs_read_data(bs, ce->name, len);
			ce->name[len] = 0;
		} else {
			ce->name = gf_strdup("");
		}

		for (i=0; i<count; i++) {
			GF_ChapterEntry *ace = (GF_ChapterEntry *) gf_list_get(ptr->list, i);
			if (ace->start_time >= ce->start_time) {
				gf_list_insert(ptr->list, ce, i);
				ce = NULL;
				break;
			}
		}
		if (ce) gf_list_add(ptr->list, ce);
		count++;
		nb_chaps--;
	}
	return GF_OK;
}