
void clap_del(GF_Box *s)
{
	GF_CleanApertureBox *ptr = (GF_CleanApertureBox*)s;
	if (ptr == NULL) return;
	gf_free(ptr);