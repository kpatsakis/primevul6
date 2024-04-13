static Bool gf_m2ts_is_long_section(u8 table_id)
{
	switch (table_id) {
	case GF_M2TS_TABLE_ID_MPEG4_BIFS:
	case GF_M2TS_TABLE_ID_MPEG4_OD:
	case GF_M2TS_TABLE_ID_INT:
	case GF_M2TS_TABLE_ID_EIT_ACTUAL_PF:
	case GF_M2TS_TABLE_ID_EIT_OTHER_PF:
	case GF_M2TS_TABLE_ID_ST:
	case GF_M2TS_TABLE_ID_SIT:
	case GF_M2TS_TABLE_ID_DSM_CC_PRIVATE:
	case GF_M2TS_TABLE_ID_MPE_FEC:
	case GF_M2TS_TABLE_ID_DSM_CC_DOWNLOAD_DATA_MESSAGE:
	case GF_M2TS_TABLE_ID_DSM_CC_UN_MESSAGE:
		return 1;
	default:
		if (table_id >= GF_M2TS_TABLE_ID_EIT_SCHEDULE_MIN && table_id <= GF_M2TS_TABLE_ID_EIT_SCHEDULE_MAX)
			return 1;
		else
			return 0;
	}
}