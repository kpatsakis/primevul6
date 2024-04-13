GF_Err edts_Size(GF_Box *s)
{
	GF_Err e;
	GF_EditBox *ptr = (GF_EditBox *)s;

	//here we have a trick: if editList is empty, skip the box
	if (!ptr->editList || ! gf_list_count(ptr->editList->entryList)) {
		ptr->size = 0;
	} else {
		e = gf_isom_box_size((GF_Box *)ptr->editList);
		if (e) return e;
		ptr->size += ptr->editList->size;
	}
	return GF_OK;
}