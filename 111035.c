Bool gf_vorbis_parse_header(GF_VorbisParser *vp, u8 *data, u32 data_len)
{
	u32 pack_type, i, j, k, times, nb_part, nb_books, nb_modes;
	u32 l;
	char szNAME[8];
	oggpack_buffer opb;

	oggpack_readinit(&opb, (u8*)data, data_len);
	pack_type = oggpack_read(&opb, 8);
	i = 0;
	while (i < 6) {
		szNAME[i] = oggpack_read(&opb, 8);
		i++;
	}
	szNAME[i] = 0;
	if (strcmp(szNAME, "vorbis")) {
		return GF_FALSE;
	}

	switch (pack_type) {
	case 0x01:
		vp->version = oggpack_read(&opb, 32);
		if (vp->version != 0) {
			return GF_FALSE;
		}
		vp->channels = oggpack_read(&opb, 8);
		vp->sample_rate = oggpack_read(&opb, 32);
		vp->max_r = oggpack_read(&opb, 32);
		vp->avg_r = oggpack_read(&opb, 32);
		vp->low_r = oggpack_read(&opb, 32);

		vp->min_block = 1<<oggpack_read(&opb, 4);
		vp->max_block = 1<<oggpack_read(&opb, 4);
		if (vp->sample_rate < 1 || vp->channels < 1 || vp->min_block < 8 || vp->max_block < vp->min_block
		    || oggpack_read(&opb, 1) != 1) {
			return GF_FALSE;
		}
		vp->nb_init=1;
		return GF_TRUE;

	case 0x03:
		/*trash comments*/
		vp->nb_init++;
		return GF_TRUE;
	case 0x05:
		/*need at least bitstream header to make sure we're parsing the right thing*/
		if (!vp->nb_init) return GF_FALSE;
		break;
	default:
		return GF_FALSE;
	}
	/*OK parse codebook*/
	nb_books = oggpack_read(&opb, 8) + 1;
	/*skip vorbis static books*/
	for (i = 0; i < nb_books; i++) {
		u32 map_type, qb, qq;
		u32 entries, dim;
		oggpack_read(&opb, 24);
		dim = oggpack_read(&opb, 16);
		entries = oggpack_read(&opb, 24);
		if ((s32)entries < 0) entries = 0;
		if (oggpack_read(&opb, 1) == 0) {
			if (oggpack_read(&opb, 1)) {
				for (j = 0; j < entries; j++) {
					if (oggpack_read(&opb, 1)) {
						oggpack_read(&opb, 5);
					}
				}
			}
			else {
				for (j = 0; j < entries; j++)
					oggpack_read(&opb, 5);
			}
		}
		else {
			oggpack_read(&opb, 5);
			for (j = 0; j < entries;) {
				u32 num = oggpack_read(&opb, ilog(entries - j, GF_FALSE));
				for (k = 0; k < num && j < entries; k++, j++) {
				}
			}
		}
		switch ((map_type = oggpack_read(&opb, 4))) {
		case 0:
			break;
		case 1:
		case 2:
			oggpack_read(&opb, 32);
			oggpack_read(&opb, 32);
			qq = oggpack_read(&opb, 4) + 1;
			oggpack_read(&opb, 1);
			if (map_type == 1) qb = vorbis_book_maptype1_quantvals(entries, dim);
			else if (map_type == 2) qb = entries * dim;
			else qb = 0;
			for (j = 0; j < qb; j++) oggpack_read(&opb, qq);
			break;
		}
	}
	times = oggpack_read(&opb, 6) + 1;
	for (i = 0; i < times; i++) oggpack_read(&opb, 16);
	times = oggpack_read(&opb, 6) + 1;
	for (i = 0; i < times; i++) {
		u32 type = oggpack_read(&opb, 16);
		if (type) {
			u32 *parts, *class_dims, count, rangebits;
			u32 max_class = 0;
			nb_part = oggpack_read(&opb, 5);
			parts = (u32*)gf_malloc(sizeof(u32) * nb_part);
			for (j = 0; j < nb_part; j++) {
				parts[j] = oggpack_read(&opb, 4);
				if (max_class < parts[j]) max_class = parts[j];
			}
			class_dims = (u32*)gf_malloc(sizeof(u32) * (max_class + 1));
			for (j = 0; j < max_class + 1; j++) {
				u32 class_sub;
				class_dims[j] = oggpack_read(&opb, 3) + 1;
				class_sub = oggpack_read(&opb, 2);
				if (class_sub) oggpack_read(&opb, 8);
				for (k = 0; k < (u32)(1 << class_sub); k++) oggpack_read(&opb, 8);
			}
			oggpack_read(&opb, 2);
			rangebits = oggpack_read(&opb, 4);
			count = 0;
			for (j = 0, k = 0; j < nb_part; j++) {
				count += class_dims[parts[j]];
				for (; k < count; k++) oggpack_read(&opb, rangebits);
			}
			gf_free(parts);
			gf_free(class_dims);
		}
		else {
			oggpack_read(&opb, 8 + 16 + 16 + 6 + 8);
			nb_books = oggpack_read(&opb, 4) + 1;
			for (j = 0; j < nb_books; j++)
				oggpack_read(&opb, 8);
		}
	}
	times = oggpack_read(&opb, 6) + 1;
	for (i = 0; i < times; i++) {
		u32 acc = 0;
		oggpack_read(&opb, 16);/*type*/
		oggpack_read(&opb, 24);
		oggpack_read(&opb, 24);
		oggpack_read(&opb, 24);
		nb_part = oggpack_read(&opb, 6) + 1;
		oggpack_read(&opb, 8);
		for (j = 0; j < nb_part; j++) {
			u32 cascade = oggpack_read(&opb, 3);
			if (oggpack_read(&opb, 1)) cascade |= (oggpack_read(&opb, 5) << 3);
			acc += icount(cascade);
		}
		for (j = 0; j < acc; j++) oggpack_read(&opb, 8);
	}
	times = oggpack_read(&opb, 6) + 1;
	for (i = 0; i < times; i++) {
		u32 sub_maps = 1;
		oggpack_read(&opb, 16);
		if (oggpack_read(&opb, 1)) sub_maps = oggpack_read(&opb, 4) + 1;
		if (oggpack_read(&opb, 1)) {
			u32 nb_steps = oggpack_read(&opb, 8) + 1;
			for (j = 0; j < nb_steps; j++) {
				oggpack_read(&opb, ilog(vp->channels, GF_TRUE));
				oggpack_read(&opb, ilog(vp->channels, GF_TRUE));
			}
		}
		oggpack_read(&opb, 2);
		if (sub_maps>1) {
			for(l=0; l<vp->channels; l++)
				oggpack_read(&opb, 4);
		}
		for (j = 0; j < sub_maps; j++) {
			oggpack_read(&opb, 8);
			oggpack_read(&opb, 8);
			oggpack_read(&opb, 8);
		}
	}
	nb_modes = oggpack_read(&opb, 6) + 1;
	for (i = 0; i < nb_modes; i++) {
		vp->mode_flag[i] = oggpack_read(&opb, 1);
		oggpack_read(&opb, 16);
		oggpack_read(&opb, 16);
		oggpack_read(&opb, 8);
	}

	vp->modebits = 0;
	j = nb_modes;
	while (j > 1) {
		vp->modebits++;
		j >>= 1;
	}

	return GF_TRUE;
}