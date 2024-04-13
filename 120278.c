GF_Err edts_AddBox(GF_Box *s, GF_Box *a)
{
	GF_EditBox *ptr = (GF_EditBox *)s;
	if (a->type == GF_ISOM_BOX_TYPE_ELST) {
		if (ptr->editList) return GF_BAD_PARAM;
		ptr->editList = (GF_EditListBox *)a;
		return GF_OK;
	} else {
		return gf_isom_box_add_default(s, a);
	}
	return GF_OK;
}