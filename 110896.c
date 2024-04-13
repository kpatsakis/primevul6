void gf_m4v_parser_reset(GF_M4VParser *m4v, u8 sc_type)
{
	m4v->current_object_start = 0;
	m4v->current_object_type = 0;
	m4v->force_next_obj_type = sc_type;
}