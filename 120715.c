void ftyp_del(GF_Box *s)
{
	GF_FileTypeBox *ptr = (GF_FileTypeBox *) s;
	if (ptr->altBrand) gf_free(ptr->altBrand);
	gf_free(ptr);
}