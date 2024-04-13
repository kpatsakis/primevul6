
void saio_del(GF_Box *s)
{
	GF_SampleAuxiliaryInfoOffsetBox *ptr = (GF_SampleAuxiliaryInfoOffsetBox*)s;
	if (ptr == NULL) return;
	if (ptr->offsets) gf_free(ptr->offsets);
	if (ptr->offsets_large) gf_free(ptr->offsets_large);
	gf_free(ptr);