GF_BitStream *gf_bs_from_file(FILE *f, u32 mode)
{
	GF_BitStream *tmp;
	if (!f) return NULL;

	tmp = (GF_BitStream *)gf_malloc(sizeof(GF_BitStream));
	if (!tmp) return NULL;
	memset(tmp, 0, sizeof(GF_BitStream));
	/*switch to internal mode*/
	mode = (mode==GF_BITSTREAM_READ) ? GF_BITSTREAM_FILE_READ : GF_BITSTREAM_FILE_WRITE;
	tmp->bsmode = mode;
	tmp->current = 0;
	tmp->nbBits = (mode == GF_BITSTREAM_FILE_READ) ? 8 : 0;
	tmp->original = NULL;
	tmp->position = 0;
	tmp->stream = f;

	/*get the size of this file (for read streams)*/
	tmp->position = gf_ftell(f);
	tmp->size = gf_fsize(f);
	gf_fseek(f, tmp->position, SEEK_SET);


	if (mode==GF_BITSTREAM_FILE_READ) {
		tmp->cache_read_alloc = gf_opts_get_int("core", "bs-cache-size");
		if (tmp->cache_read_alloc) {
			tmp->cache_read_pos = tmp->cache_read_size = tmp->cache_read_alloc;
			tmp->cache_read = gf_malloc(tmp->cache_read_alloc);
			if (!tmp->cache_read) {
				gf_free(tmp);
				return NULL;
			}
		}
	}
	if (mode == GF_BITSTREAM_FILE_WRITE) {
		tmp->cache_write_size = gf_opts_get_int("core", "bs-cache-size");
		if (tmp->cache_write_size) {
			tmp->cache_write = (char*)gf_malloc(tmp->cache_write_size);
			if (!tmp->cache_write) {
				gf_free(tmp);
				return NULL;
			}
			tmp->buffer_written = 0;
		}
	}

	return tmp;
}