void tims_del(GF_Box *s)
{
	GF_TSHintEntryBox *tims = (GF_TSHintEntryBox *)s;
	gf_free(tims);
}