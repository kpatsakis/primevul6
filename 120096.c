
void pasp_del(GF_Box *s)
{
	GF_PixelAspectRatioBox *ptr = (GF_PixelAspectRatioBox*)s;
	if (ptr == NULL) return;
	gf_free(ptr);