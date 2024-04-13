u32 gf_m4v_parser_get_obj_type(GF_M4VParser *m4v)
{
	if (m4v) return m4v->current_object_type;
	return 0;
}