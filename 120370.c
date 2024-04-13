void ccst_del(GF_Box *s)
{
	GF_CodingConstraintsBox *ptr = (GF_CodingConstraintsBox *)s;
	if (ptr) gf_free(ptr);
	return;
}