GF_M4VParser *gf_m4v_parser_bs_new(GF_BitStream *bs, Bool mpeg12video)
{
	GF_M4VParser *tmp;
	GF_SAFEALLOC(tmp, GF_M4VParser);
	if (!tmp) return NULL;
	tmp->bs = bs;
	tmp->mpeg12 = mpeg12video;
	return tmp;
}