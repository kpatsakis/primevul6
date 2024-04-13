void gf_isom_sample_del(GF_ISOSample **samp)
{
	if (!samp || ! *samp) return;
	if ((*samp)->data && (*samp)->dataLength) gf_free((*samp)->data);
	gf_free(*samp);
	*samp = NULL;
}