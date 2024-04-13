void vvnc_box_del(GF_Box *s)
{
	GF_VVCNaluConfigurationBox *ptr = (GF_VVCNaluConfigurationBox*)s;
	gf_free(ptr);
}