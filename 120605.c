GF_Box *encs_New()
{
	ISOM_DECL_BOX_ALLOC(GF_MPEGSampleEntryBox, GF_ISOM_BOX_TYPE_ENCS);
	gf_isom_sample_entry_init((GF_SampleEntryBox*)tmp);
	return (GF_Box *)tmp;
}