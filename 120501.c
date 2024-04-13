GF_Err dinf_Size(GF_Box *s)
{
	GF_Err e;
	GF_DataInformationBox *ptr = (GF_DataInformationBox *)s;
	if (ptr->dref) {
		e = gf_isom_box_size((GF_Box *) ptr->dref);
		if (e) return e;
		ptr->size += ptr->dref->size;
	}
	return GF_OK;
}