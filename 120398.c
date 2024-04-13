GF_Err stsz_Read(GF_Box *s, GF_BitStream *bs)
{
	u32 i, estSize;
	GF_SampleSizeBox *ptr = (GF_SampleSizeBox *)s;
	if (ptr == NULL) return GF_BAD_PARAM;

	//support for CompactSizes
	if (s->type == GF_ISOM_BOX_TYPE_STSZ) {
		ptr->sampleSize = gf_bs_read_u32(bs);
		ptr->sampleCount = gf_bs_read_u32(bs);

		ISOM_DECREASE_SIZE(ptr, 8);
	} else {
		//24-reserved
		gf_bs_read_int(bs, 24);
		i = gf_bs_read_u8(bs);
		ptr->sampleCount = gf_bs_read_u32(bs);
		ISOM_DECREASE_SIZE(ptr, 8);
		switch (i) {
		case 4:
		case 8:
		case 16:
			ptr->sampleSize = i;
			break;
		default:
			//try to fix the file
			//no samples, no parsing pb
			if (!ptr->sampleCount) {
				ptr->sampleSize = 16;
				return GF_OK;
			}
			estSize = (u32) (ptr->size) / ptr->sampleCount;
			if (!estSize && ((ptr->sampleCount+1)/2 == (ptr->size)) ) {
				ptr->sampleSize = 4;
				break;
			} else if (estSize == 1 || estSize == 2) {
				ptr->sampleSize = 8 * estSize;
			} else {
				return GF_ISOM_INVALID_FILE;
			}
		}
	}
	if (s->type == GF_ISOM_BOX_TYPE_STSZ) {
		if (! ptr->sampleSize && ptr->sampleCount) {
			if (ptr->sampleCount > ptr->size / 4) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid number of entries %d in stsz\n", ptr->sampleCount));
				return GF_ISOM_INVALID_FILE;
			}
			ptr->sizes = (u32 *) gf_malloc(ptr->sampleCount * sizeof(u32));
			ptr->alloc_size = ptr->sampleCount;
			if (! ptr->sizes) return GF_OUT_OF_MEM;
			for (i = 0; i < ptr->sampleCount; i++) {
				ptr->sizes[i] = gf_bs_read_u32(bs);
			}
		}
	} else {
		if (ptr->sampleSize==4) {
			if (ptr->sampleCount / 2 > ptr->size) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid number of entries %d in stsz\n", ptr->sampleCount));
				return GF_ISOM_INVALID_FILE;
			}
		} else {
			if (ptr->sampleCount > ptr->size / (ptr->sampleSize/8)) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid number of entries %d in stsz\n", ptr->sampleCount));
				return GF_ISOM_INVALID_FILE;
			}
		}
		//note we could optimize the mem usage by keeping the table compact
		//in memory. But that would complicate both caching and editing
		//we therefore keep all sizes as u32 and uncompress the table
		ptr->sizes = (u32 *) gf_malloc(ptr->sampleCount * sizeof(u32));
		if (! ptr->sizes) return GF_OUT_OF_MEM;
		ptr->alloc_size = ptr->sampleCount;

		for (i = 0; i < ptr->sampleCount; ) {
			switch (ptr->sampleSize) {
			case 4:
				ptr->sizes[i] = gf_bs_read_int(bs, 4);
				if (i+1 < ptr->sampleCount) {
					ptr->sizes[i+1] = gf_bs_read_int(bs, 4);
				} else {
					//0 padding in odd sample count
					gf_bs_read_int(bs, 4);
				}
				i += 2;
				break;
			default:
				ptr->sizes[i] = gf_bs_read_int(bs, ptr->sampleSize);
				i += 1;
				break;
			}
		}
	}
	return GF_OK;
}