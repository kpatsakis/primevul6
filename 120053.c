
GF_Err lsr1_Size(GF_Box *s)
{
	GF_Err e;
	GF_LASeRSampleEntryBox *ptr = (GF_LASeRSampleEntryBox *)s;

	s->size += 8;
	if (ptr->lsr_config) {
		e = gf_isom_box_size((GF_Box *)ptr->lsr_config);
		if (e) return e;
		ptr->size += ptr->lsr_config->size;
	}
	if (ptr->descr) {
		e = gf_isom_box_size((GF_Box *)ptr->descr);
		if (e) return e;
		ptr->size += ptr->descr->size;
	}
	return GF_OK;