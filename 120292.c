void traf_del(GF_Box *s)
{
	GF_TrackFragmentBox *ptr = (GF_TrackFragmentBox *)s;
	if (ptr == NULL) return;
	if (ptr->tfhd) gf_isom_box_del((GF_Box *) ptr->tfhd);
	if (ptr->sdtp) gf_isom_box_del((GF_Box *) ptr->sdtp);
	if (ptr->sub_samples) gf_isom_box_array_del(ptr->sub_samples);
	if (ptr->tfdt) gf_isom_box_del((GF_Box *) ptr->tfdt);
	if (ptr->sample_encryption) gf_isom_box_del((GF_Box *) ptr->sample_encryption);
	gf_isom_box_array_del(ptr->TrackRuns);
	if (ptr->sampleGroups) gf_isom_box_array_del(ptr->sampleGroups);
	if (ptr->sampleGroupsDescription) gf_isom_box_array_del(ptr->sampleGroupsDescription);
	if (ptr->sai_sizes) gf_isom_box_array_del(ptr->sai_sizes);
	if (ptr->sai_offsets) gf_isom_box_array_del(ptr->sai_offsets);
	gf_free(ptr);
}