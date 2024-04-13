static GF_Err M4V_Reset(GF_M4VParser *m4v, u64 start)
{
	gf_bs_seek(m4v->bs, start);

	assert(start < (u64)1<<31);
	m4v->current_object_start = (u32)start;
	m4v->current_object_type = 0;
	return GF_OK;
}