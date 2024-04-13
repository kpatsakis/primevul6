void gf_m4v_parser_del(GF_M4VParser *m4v)
{
	gf_bs_del(m4v->bs);
	gf_free(m4v);
}