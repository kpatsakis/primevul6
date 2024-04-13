
GF_Err sgpd_Size(GF_Box *s)
{
	u32 i;
	GF_SampleGroupDescriptionBox *p = (GF_SampleGroupDescriptionBox *)s;

	p->size += 8;

	//we force all sample groups to version 1, v0 being deprecated
	p->version=1;
	p->size += 4;

	if (p->version>=2) p->size += 4;
	p->default_length = 0;

	for (i=0; i<gf_list_count(p->group_descriptions); i++) {
		void *ptr = gf_list_get(p->group_descriptions, i);
		u32 size = sgpd_size_entry(p->grouping_type, ptr);
		p->size += size;
		if (!p->default_length) {
			p->default_length = size;
		} else if (p->default_length != size) {
			p->default_length = 0;
		}
	}
	if (p->version>=1) {
		if (!p->default_length) p->size += gf_list_count(p->group_descriptions)*4;
	}
	return GF_OK;