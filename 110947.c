Bool gf_m4v_is_valid_object_type(GF_M4VParser *m4v)
{
	return ((s32)m4v->current_object_type == -1) ? 0 : 1;
}