GF_Err edts_Write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_EditBox *ptr = (GF_EditBox *)s;

	//here we have a trick: if editList is empty, skip the box
	if (ptr->editList && gf_list_count(ptr->editList->entryList)) {
		e = gf_isom_box_write_header(s, bs);
		if (e) return e;
		e = gf_isom_box_write((GF_Box *) ptr->editList, bs);
		if (e) return e;
	}
	return GF_OK;
}