
static GF_Err nhmldmx_send_sample(GF_Filter *filter, GF_NHMLDmxCtx *ctx)
{
	GF_XMLNode *node, *childnode;
	u64 sample_duration = 0;
	char szMediaTemp[GF_MAX_PATH], szXmlFrom[1000], szXmlTo[1000];
	char *szSubSampleName = ctx->is_dims ? "DIMSSubUnit" : "NHNTSubSample";

	while ((node = (GF_XMLNode *) gf_list_enum(ctx->root->content, &ctx->current_child_idx))) {
		u8 *data;
		GF_FilterPacket *pck;
		u32 j, dims_flags;
		GF_FilterSAPType sap_type;
		GF_XMLAttribute *att;
		u64 dts=0;
		GF_Err e=GF_OK;
		s32 cts_offset;
		u64 offset=0, byte_offset = GF_FILTER_NO_BO;
		u32 nb_subsamples = 0;
		Bool redundant_rap, append, has_subbs, first_subsample_is_first = GF_FALSE;
		u32 compress_type;
		char *base_data = NULL;
		if (node->type) continue;
		if (stricmp(node->name, ctx->is_dims ? "DIMSUnit" : "NHNTSample") ) continue;

		strcpy(szMediaTemp, "");
		strcpy(szXmlFrom, "");
		strcpy(szXmlTo, "");

		/*by default handle all samples as contiguous*/
		ctx->samp_buffer_size = 0;
		dims_flags = 0;
		append = GF_FALSE;
		compress_type = ctx->compress_type;
		sample_duration = 0;
		redundant_rap = 0;
		sap_type = ctx->has_sap ? GF_FILTER_SAP_NONE : GF_FILTER_SAP_1;

		cts_offset = 0;
		if (ctx->last_dts != GF_FILTER_NO_TS)
			dts = ctx->last_dts;
		else
			dts = 0;

		ctx->sample_num++;


		j=0;
		while ( (att = (GF_XMLAttribute *)gf_list_enum(node->attributes, &j))) {
			if (!stricmp(att->name, "DTS") || !stricmp(att->name, "time")) {
				u32 h, m, s, ms;
				u64 dst_val;
				if (strchr(att->value, ':') && sscanf(att->value, "%u:%u:%u.%u", &h, &m, &s, &ms) == 4) {
					dts = (u64) ( (Double) ( ((h*3600.0 + m*60.0 + s)*1000 + ms) / 1000.0) * ctx->timescale );
				} else if (sscanf(att->value, ""LLU, &dst_val)==1) {
					dts = dst_val;
				}
			}
			else if (!stricmp(att->name, "CTSOffset")) cts_offset = atoi(att->value);
			else if (!stricmp(att->name, "isRAP") ) {
				sap_type = (!stricmp(att->value, "yes")) ? GF_FILTER_SAP_1 : GF_FILTER_SAP_NONE;
			}
			else if (!stricmp(att->name, "isSyncShadow")) redundant_rap = !stricmp(att->value, "yes") ? 1 : 0;
			else if (!stricmp(att->name, "SAPType") ) sap_type = atoi(att->value);
			else if (!stricmp(att->name, "mediaOffset")) offset = (s64) atof(att->value) ;
			else if (!stricmp(att->name, "dataLength")) ctx->samp_buffer_size = atoi(att->value);
			else if (!stricmp(att->name, "mediaFile")) {
				if (!strncmp(att->value, "data:", 5)) {
					char *base = strstr(att->value, "base64,");
					if (!base) {
						GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[NHMLDmx] Data encoding scheme not recognized in sample %d - skipping\n", ctx->sample_num));
					} else {
						base_data = att->value;
					}
				} else if (!strnicmp(att->value, "gmem://", 7)) {
					GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[NHMLDmx] Invalid url %s for NHML import\n", att->value));
				} else {
					char *url = gf_url_concatenate(ctx->src_url, att->value);
					if (!url) {
						GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[NHMLDmx] Failed to get full url for %s\n", att->value));
					} else {
						strncpy(szMediaTemp, url, GF_MAX_PATH-1);
						szMediaTemp[GF_MAX_PATH-1] = 0;
						gf_free(url);
					}
				}
			}
			else if (!stricmp(att->name, "xmlFrom")) strcpy(szXmlFrom, att->value);
			else if (!stricmp(att->name, "xmlTo")) strcpy(szXmlTo, att->value);
			/*DIMS flags*/
			else if (!stricmp(att->name, "is-Scene") && !stricmp(att->value, "yes"))
				dims_flags |= GF_DIMS_UNIT_S;
			else if (!stricmp(att->name, "is-RAP") && !stricmp(att->value, "yes")) {
				dims_flags |= GF_DIMS_UNIT_M;
				sap_type = GF_FILTER_SAP_1;
			}
			else if (!stricmp(att->name, "is-redundant") && !stricmp(att->value, "yes"))
				dims_flags |= GF_DIMS_UNIT_I;
			else if (!stricmp(att->name, "redundant-exit") && !stricmp(att->value, "yes"))
				dims_flags |= GF_DIMS_UNIT_D;
			else if (!stricmp(att->name, "priority") && !stricmp(att->value, "high"))
				dims_flags |= GF_DIMS_UNIT_P;
			else if (!stricmp(att->name, "compress") && !stricmp(att->value, "yes"))
				dims_flags |= GF_DIMS_UNIT_C;
			else if (!stricmp(att->name, "duration") )
				sscanf(att->value, ""LLU, &sample_duration);
		}
		if (sap_type==GF_FILTER_SAP_1)
			dims_flags |= GF_DIMS_UNIT_M;

		if (ctx->is_dims)
			compress_type = (dims_flags & GF_DIMS_UNIT_C) ? 2 : 0 ;

		if (ctx->is_img) sample_duration = ctx->duration.den;

		has_subbs = GF_FALSE;
		j=0;
		while ((childnode = (GF_XMLNode *) gf_list_enum(node->content, &j))) {
			if (childnode->type) continue;
			if (!stricmp(childnode->name, "BS")) {
				has_subbs = GF_TRUE;
				break;
			}
		}

		if (strlen(szXmlFrom) && strlen(szXmlTo)) {
			char *xml_file;
			if (strlen(szMediaTemp)) xml_file = szMediaTemp;
			else xml_file = ctx->szMedia;
			ctx->samp_buffer_size = 0;
			e = nhml_sample_from_xml(ctx, xml_file, szXmlFrom, szXmlTo);
		} else if (ctx->is_dims && !strlen(szMediaTemp)) {

			char *content = gf_xml_dom_serialize(node, GF_TRUE, GF_FALSE);

			ctx->samp_buffer_size = 3 + (u32) strlen(content);
			if (ctx->samp_buffer_alloc < ctx->samp_buffer_size) {
				ctx->samp_buffer_alloc = ctx->samp_buffer_size;
				ctx->samp_buffer = gf_realloc(ctx->samp_buffer, ctx->samp_buffer_size);
			}
			nhml_get_bs(&ctx->bs_w, ctx->samp_buffer, ctx->samp_buffer_size, GF_BITSTREAM_WRITE);
			gf_bs_write_u16(ctx->bs_w, ctx->samp_buffer_size - 2);
			gf_bs_write_u8(ctx->bs_w, (u8) dims_flags);
			gf_bs_write_data(ctx->bs_w, content, (ctx->samp_buffer_size - 3));
			gf_free(content);

			/*same DIMS unit*/
			if (ctx->last_dts == dts)
				append = GF_TRUE;

		} else if (has_subbs) {
			gf_bs_reassign_buffer(ctx->bs_w, ctx->samp_buffer, ctx->samp_buffer_alloc);
			gf_xml_parse_bit_sequence_bs(node, ctx->src_url, ctx->bs_w);
			gf_bs_get_content(ctx->bs_w, &ctx->samp_buffer, &ctx->samp_buffer_size);
			if (ctx->samp_buffer_size > ctx->samp_buffer_alloc)
				ctx->samp_buffer_alloc = ctx->samp_buffer_size;

		} else if (base_data) {
			char *start = strchr(base_data, ',');
			if (start) {
				u32 len = (u32)strlen(start+1);
				if (len > ctx->samp_buffer_alloc) {
					ctx->samp_buffer_alloc = len;
					ctx->samp_buffer = gf_realloc(ctx->samp_buffer, sizeof(char)*ctx->samp_buffer_alloc);
				}
				ctx->samp_buffer_size = gf_base64_decode(start, len, ctx->samp_buffer, ctx->samp_buffer_alloc);
			}
		} else {
			Bool close = GF_FALSE;
			FILE *f = ctx->mdia;

			j = 0;
			while ((childnode = (GF_XMLNode *)gf_list_enum(node->content, &j))) {
				if (childnode->type) continue;
				if (!stricmp(childnode->name, szSubSampleName)) {
					nb_subsamples++;
				}
			}

			if (strlen(szMediaTemp)) {
				f = gf_fopen(szMediaTemp, "rb");
				if (!f) {
					GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[NHMLDmx] import failure in sample %d: file %s not found", ctx->sample_num, close ? szMediaTemp : ctx->szMedia));
					return GF_NON_COMPLIANT_BITSTREAM;
				}
				close = GF_TRUE;
				if (offset) gf_fseek(f, offset, SEEK_SET);
				//when using dedicated source files per samples, we don't allow for data reference yet
			} else {
				if (!offset) offset = ctx->media_done;
				byte_offset = offset;
			}

			if (f) {
				if (!ctx->samp_buffer_size) {
					u64 ssize = gf_fsize(f);
					assert(ssize < 0x80000000);
					ctx->samp_buffer_size = (u32) ssize;
				}
				gf_fseek(f, offset, SEEK_SET);

				if (ctx->is_dims) {
					u32 read;
					if (ctx->samp_buffer_size + 3 > ctx->samp_buffer_alloc) {
						ctx->samp_buffer_alloc = ctx->samp_buffer_size + 3;
						ctx->samp_buffer = (char*)gf_realloc(ctx->samp_buffer, sizeof(char) * ctx->samp_buffer_alloc);
					}
					nhml_get_bs(&ctx->bs_w, ctx->samp_buffer, ctx->samp_buffer_alloc, GF_BITSTREAM_WRITE);
					gf_bs_write_u16(ctx->bs_w, ctx->samp_buffer_size+1);
					gf_bs_write_u8(ctx->bs_w, (u8) dims_flags);
					read = (u32) gf_fread( ctx->samp_buffer + 3, ctx->samp_buffer_size, f);
					if (ctx->samp_buffer_size != read) {
						GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[NHMLDmx] Failed to fully read sample %d: dataLength %d read %d\n", ctx->sample_num, ctx->samp_buffer_size, read));
					}
					ctx->samp_buffer_size += 3;

					/*same DIMS unit*/
					if (ctx->last_dts == dts)
						append = GF_TRUE;
				} else {
					u32 read;
					if (ctx->samp_buffer_alloc < ctx->samp_buffer_size) {
						ctx->samp_buffer_alloc = ctx->samp_buffer_size;
						ctx->samp_buffer = (char*)gf_realloc(ctx->samp_buffer, sizeof(char) * ctx->samp_buffer_alloc);
					}
					read = (u32) gf_fread(ctx->samp_buffer, ctx->samp_buffer_size, f);
					if (ctx->samp_buffer_size != read) {
						GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[NHMLDmx] Failed to fully read sample %d: dataLength %d read %d\n", ctx->sample_num, ctx->samp_buffer_size, read));
					}
				}
				if (close) gf_fclose(f);
			} else if (!nb_subsamples) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[NHMLDmx] No media file associated with sample %d!\n", ctx->sample_num));
				e = GF_URL_ERROR;
			}
		}

		if (e) return e;

		//override DIMS flags
		if (ctx->is_dims) {
			if (strstr(ctx->samp_buffer + 3, "svg ")) dims_flags |= GF_DIMS_UNIT_S;
			if (dims_flags & GF_DIMS_UNIT_S) dims_flags |= GF_DIMS_UNIT_P;
			ctx->samp_buffer[2] = dims_flags;
		}

		if (compress_type) {
#ifndef GPAC_DISABLE_ZLIB
			e = compress_sample_data(ctx, compress_type, ctx->use_dict ? &ctx->dictionary : NULL, ctx->is_dims ? 3 : 0);
			if (e) return e;

			if (ctx->is_dims) {
				nhml_get_bs(&ctx->bs_w, ctx->samp_buffer, ctx->samp_buffer_size, GF_BITSTREAM_WRITE);
				gf_bs_write_u16(ctx->bs_w, ctx->samp_buffer_size-2);
			}
#else
			GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[NHMLDmx] Error: your version of GPAC was compiled with no libz support. Abort."));
			return GF_NOT_SUPPORTED;
#endif
		}

		if (ctx->is_dims && (ctx->samp_buffer_size > 0xFFFF)) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[NHMLDmx] DIMS import failure: sample %d data is too long - maximum size allowed: 65532 bytes", ctx->sample_num));
			return GF_NON_COMPLIANT_BITSTREAM;
		}
		if (ctx->samp_buffer_size) {
			pck = gf_filter_pck_new_alloc(ctx->opid, ctx->samp_buffer_size, &data);
			if (!pck) return GF_OUT_OF_MEM;

			memcpy(data, ctx->samp_buffer, ctx->samp_buffer_size);
			gf_filter_pck_set_framing(pck, append ? GF_FALSE : GF_TRUE, nb_subsamples ? GF_FALSE : GF_TRUE);
			if (!append) {
				gf_filter_pck_set_sap(pck, sap_type);
				gf_filter_pck_set_dts(pck, dts);
				gf_filter_pck_set_cts(pck, dts+cts_offset);
				if (redundant_rap && !ctx->is_dims) gf_filter_pck_set_dependency_flags(pck, 0x1);

				if (sample_duration || ctx->dts_inc)
					gf_filter_pck_set_duration(pck, sample_duration ? (u32) sample_duration : ctx->dts_inc);

				if (byte_offset != GF_FILTER_NO_BO)
					gf_filter_pck_set_byte_offset(pck, byte_offset);

				if (ctx->in_seek) {
					if (dts+cts_offset >= ctx->start_range * ctx->timescale)
						ctx->in_seek = GF_FALSE;
					else
						gf_filter_pck_set_seek_flag(pck, GF_TRUE);
				}
			}
			gf_filter_pck_send(pck);
		} else {
			first_subsample_is_first = GF_TRUE;
		}

		if (nb_subsamples) {
			if (ctx->samp_buffer_alloc<14*nb_subsamples) {
				ctx->samp_buffer_alloc = 14*nb_subsamples;
				ctx->samp_buffer = gf_realloc(ctx->samp_buffer, ctx->samp_buffer_alloc);
			}
			assert(ctx->samp_buffer);
			nhml_get_bs(&ctx->bs_w, ctx->samp_buffer, ctx->samp_buffer_alloc, GF_BITSTREAM_WRITE);
		}

		j = 0;
		while (!append && nb_subsamples && (childnode = (GF_XMLNode *)gf_list_enum(node->content, &j))) {
			if (childnode->type) continue;
			if (!stricmp(childnode->name, szSubSampleName)) {
				u32 k = 0;
				while ((att = (GF_XMLAttribute *)gf_list_enum(childnode->attributes, &k))) {
					if (!stricmp(att->name, "mediaFile")) {
						u64 subsMediaFileSize = 0;
						FILE *f = NULL;
						char *sub_file_url = gf_url_concatenate(ctx->src_url, att->value);
						if (sub_file_url) {
							f = gf_fopen(sub_file_url, "rb");
							gf_free(sub_file_url);
						}

						if (!f) {
							GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[NHMLDmx] Error: mediaFile \"%s\" not found for subsample in sample %d. Abort.\n", att->value, ctx->sample_num));
							return GF_URL_ERROR;
						}
						subsMediaFileSize = gf_fsize(f);
						assert(subsMediaFileSize < 0x80000000);

						//send continuation frame
						pck = gf_filter_pck_new_alloc(ctx->opid, (u32) subsMediaFileSize, &data);
						if (!pck) {
							gf_fclose(f);
							return GF_OUT_OF_MEM;
						}
						subsMediaFileSize = (u32) gf_fread(data, (u32) subsMediaFileSize, f);
						gf_fclose(f);

						nb_subsamples--;
						if (first_subsample_is_first) {
							gf_filter_pck_set_framing(pck, GF_TRUE, nb_subsamples ? GF_FALSE : GF_TRUE);

							gf_filter_pck_set_sap(pck, sap_type);
							gf_filter_pck_set_dts(pck, dts);
							gf_filter_pck_set_cts(pck, dts+cts_offset);
							if (redundant_rap && !ctx->is_dims) gf_filter_pck_set_dependency_flags(pck, 0x1);

							if (sample_duration || ctx->dts_inc)
								gf_filter_pck_set_duration(pck, sample_duration ? (u32) sample_duration : ctx->dts_inc);

							if (ctx->in_seek) {
								if (dts+cts_offset >= ctx->start_range * ctx->timescale)
									ctx->in_seek = GF_FALSE;
								else
									gf_filter_pck_set_seek_flag(pck, GF_TRUE);
							}

							first_subsample_is_first = GF_FALSE;
						} else {
							gf_filter_pck_set_framing(pck, GF_FALSE, nb_subsamples ? GF_FALSE : GF_TRUE);
						}


						gf_bs_write_u32(ctx->bs_w, 0); //flags
						gf_bs_write_u32(ctx->bs_w, (u32) subsMediaFileSize);
						gf_bs_write_u32(ctx->bs_w, 0); //reserved
						gf_bs_write_u8(ctx->bs_w, 0); //priority
						gf_bs_write_u8(ctx->bs_w, 0); //discardable

						if (!nb_subsamples) {
							u32 subs_size = (u32) gf_bs_get_position(ctx->bs_w);
							gf_filter_pck_set_property(pck, GF_PROP_PCK_SUBS, &PROP_DATA(ctx->samp_buffer, subs_size) );
						}

						gf_filter_pck_send(pck);
					}
				}
			}
		}

		ctx->last_dts = dts;

		if (sample_duration)
			ctx->last_dts += sample_duration;
		else
			ctx->last_dts += ctx->dts_inc;
		ctx->media_done += ctx->samp_buffer_size;

		if (gf_filter_pid_would_block(ctx->opid))
			return GF_OK;
	}
	ctx->parsing_state = 2;
	return GF_OK;