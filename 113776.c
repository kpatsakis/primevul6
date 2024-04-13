
static GF_Err nhmldmx_init_parsing(GF_Filter *filter, GF_NHMLDmxCtx *ctx)
{
	GF_Err e;
	Bool inRootOD;
	u32 i, tkID, mtype, streamType, codecid, specInfoSize, par_den, par_num;
	GF_XMLAttribute *att;
	u32 width, height, codec_tag, sample_rate, nb_channels, version, revision, vendor_code, temporal_quality, spatial_quality, h_res, v_res, bit_depth, bits_per_sample;

	u32 dims_profile, dims_level, dims_pathComponents, dims_fullRequestHost, dims_streamType, dims_containsRedundant;
	char *textEncoding, *contentEncoding, *dims_content_script_types, *mime_type, *xml_schema_loc, *xmlns;
	FILE *nhml;
	const GF_PropertyValue *p;
	char *auxiliary_mime_types = NULL;
	char *ext, szName[1000], szInfo[GF_MAX_PATH], szXmlFrom[1000], szXmlHeaderEnd[1000];
	u8 *specInfo;
	char compressor_name[100];
	GF_XMLNode *node;
	FILE *finfo;
	u64 media_size, last_dts;
	char *szRootName, *szSampleName, *szImpName;

	szRootName = ctx->is_dims ? "DIMSStream" : "NHNTStream";
	szSampleName = ctx->is_dims ? "DIMSUnit" : "NHNTSample";
	szImpName = ctx->is_dims ? "DIMS" : "NHML";

	p = gf_filter_pid_get_property(ctx->ipid, GF_PROP_PID_FILEPATH);
	if (!p) {
		gf_filter_pid_drop_packet(ctx->ipid);
		return GF_NOT_SUPPORTED;
	}
	ctx->src_url = p->value.string;
	nhml = gf_fopen(ctx->src_url, "rt");
	if (!nhml) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[NHMLDmx] Cannot find %s file %s", szImpName, ctx->src_url));
		return GF_URL_ERROR;
	}

	szName[0] = 0;
	if (!strncmp(ctx->src_url, "gfio://", 7)) {
		char *base = gf_file_basename( gf_fileio_translate_url(ctx->src_url) );
		if (base) strcpy(szName, base);
	} else {
		strcpy(szName, ctx->src_url);
	}
	ext = gf_file_ext_start(szName);
	if (ext) ext[0] = 0;
	strcpy(ctx->szMedia, szName);
	strcpy(szInfo, szName);
	strcat(ctx->szMedia, ".media");
	strcat(szInfo, ".info");

	ctx->parser = gf_xml_dom_new();
	e = gf_xml_dom_parse(ctx->parser, p->value.string, NULL, NULL);
	if (e) {
		gf_fclose(nhml);
		GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[NHMLDmx] Error parsing %s file: Line %d - %s", szImpName, gf_xml_dom_get_line(ctx->parser), gf_xml_dom_get_error(ctx->parser) ));
		return GF_NON_COMPLIANT_BITSTREAM;
	}
	gf_fclose(nhml);

	ctx->root = gf_xml_dom_get_root(ctx->parser);
	if (!ctx->root) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[NHMLDmx] Error parsing %s file - no root node found", szImpName ));
		return GF_NON_COMPLIANT_BITSTREAM;
	}

	ctx->dts_inc = 0;
	inRootOD = GF_FALSE;
	ctx->compress_type = 0;
	specInfo = NULL;

#ifndef GPAC_DISABLE_ZLIB
	ctx->use_dict = GF_FALSE;
#endif

	if (stricmp(ctx->root->name, szRootName)) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[NHMLDmx] Error parsing %s file - \"%s\" root expected, got \"%s\"", szImpName, szRootName, ctx->root->name));
		return GF_NON_COMPLIANT_BITSTREAM;
	}

	tkID = mtype = streamType = codecid = par_den = par_num = 0;
	ctx->timescale = 1000;
	i=0;
	strcpy(szXmlHeaderEnd, "");
	ctx->header_end = 0;

	width = height = codec_tag = sample_rate = nb_channels = version = revision = vendor_code = temporal_quality = spatial_quality = h_res = v_res = bit_depth = bits_per_sample = 0;

	dims_pathComponents = dims_fullRequestHost = 0;
	textEncoding = contentEncoding = dims_content_script_types = mime_type = xml_schema_loc = xmlns = NULL;
	dims_profile = dims_level = 255;
	dims_streamType = GF_TRUE;
	dims_containsRedundant = 1;

	while ((att = (GF_XMLAttribute *)gf_list_enum(ctx->root->attributes, &i))) {
		if (!stricmp(att->name, "streamType")) {
			NHML_SCAN_INT("%u", streamType)
		} else if (!stricmp(att->name, "mediaType") && (strlen(att->value)==4)) {
			mtype = GF_4CC(att->value[0], att->value[1], att->value[2], att->value[3]);
		} else if (!stricmp(att->name, "mediaSubType") && (strlen(att->value)==4)) {
			codec_tag = GF_4CC(att->value[0], att->value[1], att->value[2], att->value[3]);
		} else if (!stricmp(att->name, "objectTypeIndication")) {
			NHML_SCAN_INT("%u", codecid)
		} else if (!stricmp(att->name, "codecID") && (strlen(att->value)==4)) {
			codecid = GF_4CC(att->value[0], att->value[1], att->value[2], att->value[3]);
		} else if (!stricmp(att->name, "timeScale")) {
			NHML_SCAN_INT("%u", ctx->timescale)
		} else if (!stricmp(att->name, "width")) {
			NHML_SCAN_INT("%u", width)
		} else if (!stricmp(att->name, "height")) {
			NHML_SCAN_INT("%u", height)
		} else if (!stricmp(att->name, "parNum")) {
			NHML_SCAN_INT("%u", par_num)
		} else if (!stricmp(att->name, "parDen")) {
			NHML_SCAN_INT("%u", par_den)
		} else if (!stricmp(att->name, "sampleRate")) {
			NHML_SCAN_INT("%u", sample_rate)
		} else if (!stricmp(att->name, "numChannels")) {
			NHML_SCAN_INT("%u", nb_channels)
		} else if (!stricmp(att->name, "baseMediaFile")) {
			char *url = gf_url_concatenate(ctx->src_url, att->value);
			strcpy(ctx->szMedia, url ? url : att->value);
			if (url) gf_free(url);
		} else if (!stricmp(att->name, "specificInfoFile")) {
			char *url = gf_url_concatenate(ctx->src_url, att->value);
			strcpy(szInfo, url ? url : att->value);
			if (url) gf_free(url);
		} else if (!stricmp(att->name, "headerEnd")) {
			NHML_SCAN_INT("%u", ctx->header_end)
		} else if (!stricmp(att->name, "trackID")) {
			NHML_SCAN_INT("%u", tkID)
		} else if (!stricmp(att->name, "inRootOD")) {
			inRootOD = (!stricmp(att->value, "yes") );
		} else if (!stricmp(att->name, "DTS_increment")) {
			NHML_SCAN_INT("%u", ctx->dts_inc)
		} else if (!stricmp(att->name, "gzipSamples")) {
			if (!stricmp(att->value, "yes") || !stricmp(att->value, "gzip"))
				ctx->compress_type = 2;
			else if (!stricmp(att->value, "deflate"))
				ctx->compress_type = 1;
		} else if (!stricmp(att->name, "auxiliaryMimeTypes")) {
			auxiliary_mime_types = gf_strdup(att->name);
		}
#ifndef GPAC_DISABLE_ZLIB
		else if (!stricmp(att->name, "gzipDictionary")) {
			u32 d_size;
			if (stricmp(att->value, "self")) {
				char *url = gf_url_concatenate(ctx->src_url, att->value);

				e = gf_file_load_data(url ? url : att->value, (u8 **) &ctx->dictionary, &d_size);

				if (url) gf_free(url);
				if (e) {
					GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[NHMLDmx] Cannot open dictionary file %s: %s", att->value, gf_error_to_string(e) ));
					continue;
				}
			}
			ctx->use_dict = GF_TRUE;
		}
#endif
		/*unknown desc related*/
		else if (!stricmp(att->name, "compressorName")) {
			strncpy(compressor_name, att->value, 99);
			compressor_name[99]=0;
		} else if (!stricmp(att->name, "codecVersion")) {
			NHML_SCAN_INT("%u", version)
		} else if (!stricmp(att->name, "codecRevision")) {
			NHML_SCAN_INT("%u", revision)
		} else if (!stricmp(att->name, "codecVendor") && (strlen(att->value)==4)) {
			vendor_code = GF_4CC(att->value[0], att->value[1], att->value[2], att->value[3]);
		} else if (!stricmp(att->name, "temporalQuality")) {
			NHML_SCAN_INT("%u", temporal_quality)
		} else if (!stricmp(att->name, "spatialQuality")) {
			NHML_SCAN_INT("%u", spatial_quality)
		} else if (!stricmp(att->name, "horizontalResolution")) {
			NHML_SCAN_INT("%u", h_res)
		} else if (!stricmp(att->name, "verticalResolution")) {
			NHML_SCAN_INT("%u", v_res)
		} else if (!stricmp(att->name, "bitDepth")) {
			NHML_SCAN_INT("%u", bit_depth)
		} else if (!stricmp(att->name, "bitsPerSample")) {
			NHML_SCAN_INT("%u", bits_per_sample)
		}
		/*DIMS stuff*/
		else if (!stricmp(att->name, "profile")) {
			NHML_SCAN_INT("%u", dims_profile)
		} else if (!stricmp(att->name, "level")) {
			NHML_SCAN_INT("%u", dims_level)
		} else if (!stricmp(att->name, "pathComponents")) {
			NHML_SCAN_INT("%u", dims_pathComponents)
		} else if (!stricmp(att->name, "useFullRequestHost") && !stricmp(att->value, "yes")) {
			dims_fullRequestHost = GF_TRUE;
		} else if (!stricmp(att->name, "stream_type") && !stricmp(att->value, "secondary")) {
			dims_streamType = GF_FALSE;
		} else if (!stricmp(att->name, "contains_redundant")) {
			if (!stricmp(att->value, "main")) {
				dims_containsRedundant = 1;
			} else if (!stricmp(att->value, "redundant")) {
				dims_containsRedundant = 2;
			} else if (!stricmp(att->value, "main+redundant")) {
				dims_containsRedundant = 3;
			}
		} else if (!stricmp(att->name, "text_encoding") || !stricmp(att->name, "encoding")) {
			textEncoding = att->value;
		} else if (!stricmp(att->name, "content_encoding")) {
			if (!strcmp(att->value, "deflate")) {
				contentEncoding = att->value;
				ctx->compress_type = 1;
			}
			else if (!strcmp(att->value, "gzip")) {
				contentEncoding = att->value;
				ctx->compress_type = 2;
			}
		} else if (!stricmp(att->name, "content_script_types")) {
			dims_content_script_types = att->value;
		} else if (!stricmp(att->name, "mime_type")) {
			mime_type = att->value;
		} else if (!stricmp(att->name, "media_namespace")) {
			xmlns = att->value;
		} else if (!stricmp(att->name, "media_schema_location")) {
			xml_schema_loc = att->value;
		} else if (!stricmp(att->name, "xml_namespace")) {
			xmlns = att->value;
		} else if (!stricmp(att->name, "xml_schema_location")) {
			xml_schema_loc = att->value;
		} else if (!stricmp(att->name, "xmlHeaderEnd")) {
			strcpy(szXmlHeaderEnd, att->value);
		}
	}
	if (sample_rate && !ctx->timescale) {
		ctx->timescale = sample_rate;
	}
	if (!bits_per_sample) {
		bits_per_sample = 16;
	}

	if (ctx->is_dims || (codec_tag==GF_ISOM_SUBTYPE_3GP_DIMS)) {
		mtype = GF_ISOM_MEDIA_DIMS;
		codec_tag=GF_ISOM_SUBTYPE_3GP_DIMS;
		codecid = GF_CODECID_DIMS;
		streamType = GF_STREAM_SCENE;
	}
	if (gf_file_exists_ex(ctx->szMedia, ctx->src_url))
		ctx->mdia = gf_fopen_ex(ctx->szMedia, ctx->src_url, "rb");

	specInfoSize = 0;
	if (!streamType && !mtype && !codec_tag) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[NHMLDmx] parsing %s file - StreamType or MediaType not specified", szImpName));
		return GF_NON_COMPLIANT_BITSTREAM;
	}

	finfo = NULL;
	if (gf_file_exists_ex(szInfo, ctx->src_url))
		finfo = gf_fopen_ex(szInfo, ctx->src_url, "rb");

	if (finfo) {
		e = gf_file_load_data_filep(finfo, (u8 **)&specInfo, &specInfoSize);
		gf_fclose(finfo);
		if (e) return e;
	} else if (ctx->header_end) {
		/* for text based streams, the decoder specific info can be at the beginning of the file */
		specInfoSize = ctx->header_end;
		specInfo = (char*)gf_malloc(sizeof(char) * (specInfoSize+1));
		specInfoSize = (u32) gf_fread(specInfo, specInfoSize, ctx->mdia);
		specInfo[specInfoSize] = 0;
		ctx->header_end = specInfoSize;
	} else if (strlen(szXmlHeaderEnd)) {
		/* for XML based streams, the decoder specific info can be up to some element in the file */
		strcpy(szXmlFrom, "doc.start");
		ctx->samp_buffer_size = 0;
		e = nhml_sample_from_xml(ctx, ctx->szMedia, szXmlFrom, szXmlHeaderEnd);
		if (e) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[NHMLDmx] failed to load XML header: %s", gf_error_to_string(e) ));
			return e;
		}

		specInfo = (char*)gf_malloc(sizeof(char) * (ctx->samp_buffer_size +1));
		memcpy(specInfo, ctx->samp_buffer, ctx->samp_buffer_size);
		specInfoSize = ctx->samp_buffer_size;
		specInfo[specInfoSize] = 0;
	}

	i=0;
	while ((node = (GF_XMLNode *) gf_list_enum(ctx->root->content, &i))) {
		if (node->type) continue;
		if (stricmp(node->name, "DecoderSpecificInfo") ) continue;

		e = gf_xml_parse_bit_sequence(node, ctx->src_url, &specInfo, &specInfoSize);
		if (e) {
			if (specInfo) gf_free(specInfo);
			return e;
		}
		break;
	}

	ctx->opid = gf_filter_pid_new(filter);
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_STREAM_TYPE, &PROP_UINT(streamType) );
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_CODECID, &PROP_UINT(codecid) );
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_TIMESCALE, &PROP_UINT(ctx->timescale) );
	if (ctx->reframe)
	   gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_UNFRAMED, &PROP_UINT(GF_TRUE) );

#ifndef GPAC_DISABLE_AV_PARSERS
	if (!width && !height && specInfo && (codecid==GF_CODECID_MPEG4_PART2)) {
		GF_M4VDecSpecInfo dsi;
		e = gf_m4v_get_config(specInfo, specInfoSize, &dsi);
		if (!e) {
			width = dsi.width;
			height = dsi.height;
			par_num = dsi.par_num;
			par_den = dsi.par_den;
		}
	}
#endif

	if (tkID) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_ESID, &PROP_UINT(tkID) );
	if (width) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_WIDTH, &PROP_UINT(width) );
	if (height) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_HEIGHT, &PROP_UINT(height) );

	if (par_den) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_SAR, &PROP_FRAC_INT(par_num, par_den) );
	switch (bits_per_sample) {
	case 8:
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_AUDIO_FORMAT, &PROP_UINT(GF_AUDIO_FMT_U8) );
		break;
	case 16:
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_AUDIO_FORMAT, &PROP_UINT(GF_AUDIO_FMT_S16) );
		break;
	case 24:
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_AUDIO_FORMAT, &PROP_UINT(GF_AUDIO_FMT_S24) );
		break;
	case 32:
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_AUDIO_FORMAT, &PROP_UINT(GF_AUDIO_FMT_S32) );
		break;
	default:
		GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[NHMLDmx] Unsupported audio bit depth %d\n", bits_per_sample));
		break;
	}

	if (sample_rate) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_SAMPLE_RATE, &PROP_UINT(sample_rate) );
	if (nb_channels) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_NUM_CHANNELS, &PROP_UINT(nb_channels) );
	if (bit_depth) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_BIT_DEPTH_Y, &PROP_UINT(bit_depth) );

	if (ctx->is_dims) {
		if (dims_profile) gf_filter_pid_set_property_str(ctx->opid, "dims:profile", &PROP_UINT(dims_profile) );
		if (dims_level) gf_filter_pid_set_property_str(ctx->opid, "dims:level", &PROP_UINT(dims_level) );
		if (dims_pathComponents) gf_filter_pid_set_property_str(ctx->opid, "dims:pathComponents", &PROP_UINT(dims_pathComponents) );
		if (dims_fullRequestHost) gf_filter_pid_set_property_str(ctx->opid, "dims:fullRequestHost", &PROP_UINT(dims_fullRequestHost) );
		if (dims_streamType) gf_filter_pid_set_property_str(ctx->opid, "dims:streamType", &PROP_BOOL(dims_streamType) );
		if (dims_containsRedundant) gf_filter_pid_set_property_str(ctx->opid, "dims:redundant", &PROP_UINT(dims_containsRedundant) );
		if (textEncoding) gf_filter_pid_set_property_str(ctx->opid, "meta:encoding", &PROP_STRING(textEncoding) );
		if (contentEncoding) gf_filter_pid_set_property_str(ctx->opid, "meta:content_encoding", &PROP_STRING(contentEncoding) );
		if (dims_content_script_types) gf_filter_pid_set_property_str(ctx->opid, "dims:scriptTypes", &PROP_STRING(dims_content_script_types) );
		if (mime_type) gf_filter_pid_set_property_str(ctx->opid, "meta:mime", &PROP_STRING(mime_type) );
		if (xml_schema_loc) gf_filter_pid_set_property_str(ctx->opid, "meta:schemaloc", &PROP_STRING(xml_schema_loc) );

	} else if (mtype == GF_ISOM_MEDIA_MPEG_SUBT || mtype == GF_ISOM_MEDIA_SUBT || mtype == GF_ISOM_MEDIA_TEXT) {
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_STREAM_TYPE, &PROP_UINT(mtype) );
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_CODECID, &PROP_UINT(codec_tag) );

		if (codec_tag == GF_ISOM_SUBTYPE_STPP) {
			if (xmlns) gf_filter_pid_set_property_str(ctx->opid, "meta:xmlns", &PROP_STRING(xmlns) );
			if (xml_schema_loc) gf_filter_pid_set_property_str(ctx->opid, "meta:schemaloc", &PROP_STRING(xml_schema_loc) );
			if (auxiliary_mime_types) gf_filter_pid_set_property_str(ctx->opid, "meta:aux_mimes", &PROP_STRING(auxiliary_mime_types) );

		} else if (codec_tag == GF_ISOM_SUBTYPE_SBTT) {
			if (mime_type) gf_filter_pid_set_property_str(ctx->opid, "meta:mime", &PROP_STRING(mime_type) );
			if (textEncoding) gf_filter_pid_set_property_str(ctx->opid, "meta:encoding", &PROP_STRING(textEncoding) );
		} else if (codec_tag == GF_ISOM_SUBTYPE_STXT) {
			if (mime_type) gf_filter_pid_set_property_str(ctx->opid, "meta:mime", &PROP_STRING(mime_type) );
			if (textEncoding) gf_filter_pid_set_property_str(ctx->opid, "meta:encoding", &PROP_STRING(textEncoding) );
			if (contentEncoding) gf_filter_pid_set_property_str(ctx->opid, "meta:content_encoding", &PROP_STRING(contentEncoding) );
		} else {
			e = GF_NOT_SUPPORTED;
		}
	} else if (mtype == GF_ISOM_MEDIA_META) {
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_STREAM_TYPE, &PROP_UINT(mtype) );
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_CODECID, &PROP_UINT(codec_tag) );

		if (codec_tag == GF_ISOM_SUBTYPE_METX) {
			if (xmlns) gf_filter_pid_set_property_str(ctx->opid, "meta:xmlns", &PROP_STRING(xmlns) );
			if (xml_schema_loc) gf_filter_pid_set_property_str(ctx->opid, "meta:schemaloc", &PROP_STRING(xml_schema_loc) );
			if (textEncoding) gf_filter_pid_set_property_str(ctx->opid, "meta:encoding", &PROP_STRING(textEncoding) );
		} else if (codec_tag == GF_ISOM_SUBTYPE_METT) {
			if (mime_type) gf_filter_pid_set_property_str(ctx->opid, "meta:mime", &PROP_STRING(mime_type) );
			if (textEncoding) gf_filter_pid_set_property_str(ctx->opid, "meta:encoding", &PROP_STRING(textEncoding) );
		} else {
			e = GF_NOT_SUPPORTED;
		}
	} else if (!streamType) {
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_STREAM_TYPE, &PROP_UINT(mtype) );
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_CODECID, &PROP_UINT(codec_tag) );

		if (version) gf_filter_pid_set_property_str(ctx->opid, "gene:version", &PROP_UINT(version) );
		if (revision) gf_filter_pid_set_property_str(ctx->opid, "gene:revision", &PROP_UINT(revision) );
		if (vendor_code) gf_filter_pid_set_property_str(ctx->opid, "gene:vendor", &PROP_UINT(vendor_code) );
		if (temporal_quality) gf_filter_pid_set_property_str(ctx->opid, "gene:temporal_quality", &PROP_UINT(temporal_quality) );
		if (spatial_quality) gf_filter_pid_set_property_str(ctx->opid, "gene:spatial_quality", &PROP_UINT(spatial_quality) );
		if (h_res) gf_filter_pid_set_property_str(ctx->opid, "gene:horizontal_res", &PROP_UINT(h_res) );
		if (v_res) gf_filter_pid_set_property_str(ctx->opid, "gene:vertical_res", &PROP_UINT(v_res) );
	}


	if (specInfo) {
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DECODER_CONFIG, &PROP_DATA_NO_COPY(specInfo, specInfoSize) );
		specInfo = NULL;
		specInfoSize = 0;
	}

	if (inRootOD) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_IN_IOD, &PROP_BOOL(GF_TRUE) );

	ctx->media_done = 0;
	ctx->current_child_idx = 0;
	ctx->last_dts = GF_FILTER_NO_TS;

	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_FILEPATH, & PROP_STRING(ctx->szMedia));

	if (ctx->mdia) {
		media_size = gf_fsize(ctx->mdia);
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DOWN_SIZE, & PROP_LONGUINT(media_size) );
	}

	if (specInfo) gf_free(specInfo);
	if (auxiliary_mime_types) gf_free(auxiliary_mime_types);

	//compute duration
	ctx->duration.den = ctx->timescale;
	ctx->duration.num = 0;
	last_dts = 0;
	i=0;
	ctx->has_sap = GF_FALSE;
	while ((node = (GF_XMLNode *) gf_list_enum(ctx->root->content, &i))) {
		u32 j=0;
		u64 dts=0;
		s32 cts_offset=0;
		u64 sample_duration = 0;
		if (node->type) continue;
		if (stricmp(node->name, szSampleName) ) continue;

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
			else if (!stricmp(att->name, "duration") ) sscanf(att->value, ""LLU, &sample_duration);
			else if (!stricmp(att->name, "isRAP") ) ctx->has_sap = GF_TRUE;
		}
		last_dts = ctx->duration.num;
		if (dts) ctx->duration.num = (u32) (dts + cts_offset);
		if (sample_duration) {
			last_dts = 0;
			ctx->duration.num += (u32) sample_duration;
		} else if (ctx->dts_inc) {
			last_dts = 0;
			ctx->duration.num += ctx->dts_inc;
		}
	}
	if (last_dts) {
		ctx->duration.num += (u32) (ctx->duration.num - last_dts);
	}
	//assume image, one sec (default for old arch)
	if ((streamType==4) && !ctx->duration.num) {
		ctx->is_img = GF_TRUE;
		ctx->duration.num =ctx->duration.den;
	}

	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DURATION, & PROP_FRAC64(ctx->duration) );
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_PLAYBACK_MODE, &PROP_UINT(GF_PLAYBACK_MODE_FASTFORWARD ) );
	return e;