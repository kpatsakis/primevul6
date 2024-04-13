
void lsr1_del(GF_Box *s)
{
	GF_LASeRSampleEntryBox *ptr = (GF_LASeRSampleEntryBox *)s;
	if (ptr == NULL) return;
	gf_isom_sample_entry_predestroy((GF_SampleEntryBox *)s);

	if (ptr->slc) gf_odf_desc_del((GF_Descriptor *)ptr->slc);
	if (ptr->lsr_config) gf_isom_box_del((GF_Box *) ptr->lsr_config);
	if (ptr->descr) gf_isom_box_del((GF_Box *) ptr->descr);
	gf_free(ptr);