
GF_Err leva_Read(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	GF_LevelAssignmentBox *ptr = (GF_LevelAssignmentBox*)s;

	if (ptr->size < 4) return GF_BAD_PARAM;
	ptr->level_count = gf_bs_read_u8(bs);
	ptr->size -= 4;
	GF_SAFE_ALLOC_N(ptr->levels, ptr->level_count, GF_LevelAssignment);
	for (i = 0; i < ptr->level_count; i++) {
		GF_LevelAssignment *level = &ptr->levels[i];
		u8 tmp;
		if (ptr->size < 5) return GF_BAD_PARAM;
		level->track_id = gf_bs_read_u32(bs);
		tmp = gf_bs_read_u8(bs);
		level->padding_flag = tmp >> 7;
		level->type = tmp & 0x7F;
		if (level->type == 0) {
			level->grouping_type = gf_bs_read_u32(bs);
		}
		else if (level->type == 1) {
			level->grouping_type = gf_bs_read_u32(bs);
			level->grouping_type_parameter = gf_bs_read_u32(bs);
		}
		else if (level->type == 4) {
			level->sub_track_id = gf_bs_read_u32(bs);
		}
	}
	return GF_OK;