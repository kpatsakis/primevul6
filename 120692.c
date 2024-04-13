void rely_del(GF_Box *s)
{
	GF_RelyHintBox *rely = (GF_RelyHintBox *)s;
	gf_free(rely);
}