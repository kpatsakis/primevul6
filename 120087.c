
void dvcC_del(GF_Box *s)
{
	GF_DOVIConfigurationBox *ptr = (GF_DOVIConfigurationBox*)s;
	gf_free(ptr);