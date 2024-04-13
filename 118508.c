static void gf_m2ts_section_complete(GF_M2TS_Demuxer *ts, GF_M2TS_SectionFilter *sec, GF_M2TS_SECTION_ES *ses)
{
	//seek mode, only process PAT and PMT
	if (ts->seek_mode && (sec->section[0] != GF_M2TS_TABLE_ID_PAT) && (sec->section[0] != GF_M2TS_TABLE_ID_PMT)) {
		/*clean-up (including broken sections)*/
		if (sec->section) gf_free(sec->section);
		sec->section = NULL;
		sec->length = sec->received = 0;
		return;
	}

	if (!sec->process_section) {
		if ((ts->on_event && (sec->section[0]==GF_M2TS_TABLE_ID_AIT)) ) {
#ifdef GPAC_ENABLE_DSMCC
			GF_M2TS_SL_PCK pck;
			pck.data_len = sec->length;
			pck.data = sec->section;
			pck.stream = (GF_M2TS_ES *)ses;
			//ts->on_event(ts, GF_M2TS_EVT_AIT_FOUND, &pck);
			on_ait_section(ts, GF_M2TS_EVT_AIT_FOUND, &pck);
#endif
		} else if ((ts->on_event && (sec->section[0]==GF_M2TS_TABLE_ID_DSM_CC_ENCAPSULATED_DATA	|| sec->section[0]==GF_M2TS_TABLE_ID_DSM_CC_UN_MESSAGE ||
		                             sec->section[0]==GF_M2TS_TABLE_ID_DSM_CC_DOWNLOAD_DATA_MESSAGE || sec->section[0]==GF_M2TS_TABLE_ID_DSM_CC_STREAM_DESCRIPTION || sec->section[0]==GF_M2TS_TABLE_ID_DSM_CC_PRIVATE)) ) {

#ifdef GPAC_ENABLE_DSMCC
			GF_M2TS_SL_PCK pck;
			pck.data_len = sec->length;
			pck.data = sec->section;
			pck.stream = (GF_M2TS_ES *)ses;
			on_dsmcc_section(ts,GF_M2TS_EVT_DSMCC_FOUND,&pck);
			//ts->on_event(ts, GF_M2TS_EVT_DSMCC_FOUND, &pck);
#endif
		}
#ifdef GPAC_ENABLE_MPE
		else if (ts->on_mpe_event && ((ses && (ses->flags & GF_M2TS_EVT_DVB_MPE)) || (sec->section[0]==GF_M2TS_TABLE_ID_INT)) ) {
			GF_M2TS_SL_PCK pck;
			pck.data_len = sec->length;
			pck.data = sec->section;
			pck.stream = (GF_M2TS_ES *)ses;
			ts->on_mpe_event(ts, GF_M2TS_EVT_DVB_MPE, &pck);
		}
#endif
		else if (ts->on_event) {
			GF_M2TS_SL_PCK pck;
			pck.data_len = sec->length;
			pck.data = sec->section;
			pck.stream = (GF_M2TS_ES *)ses;
			ts->on_event(ts, GF_M2TS_EVT_DVB_GENERAL, &pck);
		}
	} else {
		Bool has_syntax_indicator;
		u8 table_id;
		u16 extended_table_id;
		u32 status, section_start, i;
		GF_M2TS_Table *t, *prev_t;
		unsigned char *data;
		Bool section_valid = 0;

		status = 0;
		/*parse header*/
		data = (u8 *)sec->section;

		/*look for proper table*/
		table_id = data[0];

		if (ts->on_event) {
			switch (table_id) {
			case GF_M2TS_TABLE_ID_PAT:
			case GF_M2TS_TABLE_ID_SDT_ACTUAL:
			case GF_M2TS_TABLE_ID_PMT:
			case GF_M2TS_TABLE_ID_NIT_ACTUAL:
			case GF_M2TS_TABLE_ID_TDT:
			case GF_M2TS_TABLE_ID_TOT:
			{
				GF_M2TS_SL_PCK pck;
				pck.data_len = sec->length;
				pck.data = sec->section;
				pck.stream = (GF_M2TS_ES *)ses;
				ts->on_event(ts, GF_M2TS_EVT_DVB_GENERAL, &pck);
			}
			}
		}

		has_syntax_indicator = (data[1] & 0x80) ? 1 : 0;
		if (has_syntax_indicator) {
			extended_table_id = (data[3]<<8) | data[4];
		} else {
			extended_table_id = 0;
		}

		prev_t = NULL;
		t = sec->table;
		while (t) {
			if ((t->table_id==table_id) && (t->ex_table_id == extended_table_id)) break;
			prev_t = t;
			t = t->next;
		}

		/*create table*/
		if (!t) {
			GF_SAFEALLOC(t, GF_M2TS_Table);
			if (!t) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[MPEG-2 TS] Fail to alloc table %d %d\n", table_id, extended_table_id));
				return;
			}
			GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[MPEG-2 TS] Creating table %d %d\n", table_id, extended_table_id));
			t->table_id = table_id;
			t->ex_table_id = extended_table_id;
			t->last_version_number = 0xFF;
			t->sections = gf_list_new();
			if (prev_t) prev_t->next = t;
			else sec->table = t;
		}

		if (has_syntax_indicator) {
			if (sec->length < 4) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[MPEG-2 TS] corrupted section length %d less than CRC \n", sec->length));
			} else {
				/*remove crc32*/
				sec->length -= 4;
				if (gf_m2ts_crc32_check((char *)data, sec->length)) {
					s32 cur_sec_num;
					t->version_number = (data[5] >> 1) & 0x1f;
					if (t->last_section_number && t->section_number && (t->version_number != t->last_version_number)) {
						GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[MPEG-2 TS] table transmission interrupted: previous table (v=%d) %d/%d sections - new table (v=%d) %d/%d sections\n", t->last_version_number, t->section_number, t->last_section_number, t->version_number, data[6] + 1, data[7] + 1) );
						gf_m2ts_reset_sections(t->sections);
						t->section_number = 0;
					}

					t->current_next_indicator = (data[5] & 0x1) ? 1 : 0;
					/*add one to section numbers to detect if we missed or not the first section in the table*/
					cur_sec_num = data[6] + 1;
					t->last_section_number = data[7] + 1;
					section_start = 8;
					/*we missed something*/
					if (!sec->process_individual && t->section_number + 1 != cur_sec_num) {
						/* TODO - Check how to handle sections when the first complete section does
						   not have its sec num 0 */
						section_valid = 0;
						if (t->is_init) {
							GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[MPEG-2 TS] corrupted table (lost section %d)\n", cur_sec_num ? cur_sec_num-1 : 31) );
						}
					} else {
						section_valid = 1;
						t->section_number = cur_sec_num;
					}
				} else {
					GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[MPEG-2 TS] corrupted section (CRC32 failed)\n"));
				}
			}
		} else {
			section_valid = 1;
			section_start = 3;
		}
		/*process section*/
		if (section_valid) {
			GF_M2TS_Section *section;

			GF_SAFEALLOC(section, GF_M2TS_Section);
			if (!section) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[MPEG-2 TS] Fail to create section\n"));
				return;
			}
			section->data_size = sec->length - section_start;
			section->data = (unsigned char*)gf_malloc(sizeof(unsigned char)*section->data_size);
			memcpy(section->data, sec->section + section_start, sizeof(unsigned char)*section->data_size);
			gf_list_add(t->sections, section);

			if (t->section_number == 1) {
				status |= GF_M2TS_TABLE_START;
				if (t->last_version_number == t->version_number) {
					t->is_repeat = 1;
				} else {
					t->is_repeat = 0;
				}
				/*only update version number in the first section of the table*/
				t->last_version_number = t->version_number;
			}

			if (t->is_init) {
				if (t->is_repeat) {
					status |=  GF_M2TS_TABLE_REPEAT;
				} else {
					status |=  GF_M2TS_TABLE_UPDATE;
				}
			} else {
				status |=  GF_M2TS_TABLE_FOUND;
			}

			if (t->last_section_number == t->section_number) {
				u32 table_size;

				status |= GF_M2TS_TABLE_END;

				table_size = 0;
				for (i=0; i<gf_list_count(t->sections); i++) {
					GF_M2TS_Section *section = gf_list_get(t->sections, i);
					table_size += section->data_size;
				}
				if (t->is_repeat) {
					if (t->table_size != table_size) {
						status |= GF_M2TS_TABLE_UPDATE;
						GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[MPEG-2 TS] Repeated section found with different sizes (old table %d bytes, new table %d bytes)\n", t->table_size, table_size) );

						t->table_size = table_size;
					}
				} else {
					t->table_size = table_size;
				}

				t->is_init = 1;
				/*reset section number*/
				t->section_number = 0;

				t->is_repeat = 0;

			}

			if (sec->process_individual) {
				/*send each section of the table and not the aggregated table*/
				if (sec->process_section)
					sec->process_section(ts, ses, t->sections, t->table_id, t->ex_table_id, t->version_number, (u8) (t->last_section_number - 1), status);

				gf_m2ts_reset_sections(t->sections);
			} else {
				if (status&GF_M2TS_TABLE_END) {
					if (sec->process_section)
						sec->process_section(ts, ses, t->sections, t->table_id, t->ex_table_id, t->version_number, (u8) (t->last_section_number - 1), status);

					gf_m2ts_reset_sections(t->sections);
				}
			}

		} else {
			sec->cc = -1;
			t->section_number = 0;
		}
	}
	/*clean-up (including broken sections)*/
	if (sec->section) gf_free(sec->section);
	sec->section = NULL;
	sec->length = sec->received = 0;
}