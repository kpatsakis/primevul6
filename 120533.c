GF_Err traf_Size(GF_Box *s)
{
	GF_Err e;
	GF_TrackFragmentBox *ptr = (GF_TrackFragmentBox *)s;

	if (ptr->tfhd) {
		e = gf_isom_box_size((GF_Box *) ptr->tfhd);
		if (e) return e;
		ptr->size += ptr->tfhd->size;
	}
	if (ptr->sub_samples) {
		e = gf_isom_box_array_size(s, ptr->sub_samples);
		if (e) return e;
	}
	if (ptr->sdtp) {
		e = gf_isom_box_size((GF_Box *) ptr->sdtp);
		if (e) return e;
		ptr->size += ptr->sdtp->size;
	}
	if (ptr->tfdt) {
		e = gf_isom_box_size((GF_Box *) ptr->tfdt);
		if (e) return e;
		ptr->size += ptr->tfdt->size;
	}

	if (ptr->sampleGroups) {
		e = gf_isom_box_array_size(s, ptr->sampleGroups);
		if (e) return e;
	}
	if (ptr->sampleGroupsDescription) {
		e = gf_isom_box_array_size(s, ptr->sampleGroupsDescription);
		if (e) return e;
	}
	if (ptr->sai_sizes) {
		e = gf_isom_box_array_size(s, ptr->sai_sizes);
		if (e) return e;
	}
	if (ptr->sai_offsets) {
		e = gf_isom_box_array_size(s, ptr->sai_offsets);
		if (e) return e;
	}
	if (ptr->sample_encryption) {
		e = gf_isom_box_size((GF_Box *) ptr->sample_encryption);
		if (e) return e;
		ptr->size += ptr->sample_encryption->size;
	}
	if (ptr->tfxd) {
		e = gf_isom_box_size((GF_Box *)ptr->tfxd);
		if (e) return e;
		s->size += ptr->tfxd->size;
	}
	return gf_isom_box_array_size(s, ptr->TrackRuns);
}