static int indirectblockRead(struct READER *reader,
		struct DATAOBJECT *dataobject, struct FRACTALHEAP *fractalheap,
		uint64_t iblock_size) {
	int size, nrows, max_dblock_rows, k, n, err;
	uint32_t filter_mask;
	uint64_t heap_header_address, block_offset, child_direct_block = 0,
			size_filtered, child_indirect_block;
	long store;

	char buf[4];

	UNUSED(size_filtered);
	UNUSED(heap_header_address);
	UNUSED(filter_mask);

	/* read signature */
	if (fread(buf, 1, 4, reader->fhd) != 4 || strncmp(buf, "FHIB", 4)) {
		log("cannot read signature of fractal heap indirect block\n");
		return MYSOFA_INVALID_FORMAT;
	}
	log("%08" PRIX64 " %.4s\n", (uint64_t )ftell(reader->fhd) - 4, buf);

	if (fgetc(reader->fhd) != 0) {
		log("object FHIB must have version 0\n");
		return MYSOFA_UNSUPPORTED_FORMAT;
	}

	/* ignore it */
	heap_header_address = readValue(reader, reader->superblock.size_of_offsets);

	size = (fractalheap->maximum_heap_size + 7) / 8;
	block_offset = readValue(reader, size);

	if (block_offset) {
		log("FHIB block offset is not 0\n");
		return MYSOFA_UNSUPPORTED_FORMAT;
	}

	/*	 The number of rows of blocks, nrows, in an indirect block of size iblock_size is given by the following expression: */
	nrows = (log2i(iblock_size) - log2i(fractalheap->starting_block_size)) + 1;

	/* The maximum number of rows of direct blocks, max_dblock_rows, in any indirect block of a fractal heap is given by the following expression: */
	max_dblock_rows = (log2i(fractalheap->maximum_direct_block_size)
			- log2i(fractalheap->starting_block_size)) + 2;

	/* Using the computed values for nrows and max_dblock_rows, along with the Width of the doubling table, the number of direct and indirect block entries (K and N in the indirect block description, below) in an indirect block can be computed: */
	if (nrows < max_dblock_rows)
		k = nrows * fractalheap->table_width;
	else
		k = max_dblock_rows * fractalheap->table_width;

	/* If nrows is less than or equal to max_dblock_rows, N is 0. Otherwise, N is simply computed: */
	n = k - (max_dblock_rows * fractalheap->table_width);

	while (k > 0) {
		child_direct_block = readValue(reader,
				reader->superblock.size_of_offsets);
		if (fractalheap->encoded_length > 0) {
			size_filtered = readValue(reader,
					reader->superblock.size_of_lengths);
			filter_mask = readValue(reader, 4);
		}
		log(">> %d %" PRIX64 " %d\n",k,child_direct_block,size);
		if (validAddress(reader, child_direct_block)) {
			store = ftell(reader->fhd);
			if (fseek(reader->fhd, child_direct_block, SEEK_SET) < 0)
				return errno;
			err = directblockRead(reader, dataobject, fractalheap);
			if (err)
				return err;
			if (store < 0)
				return MYSOFA_READ_ERROR;
			if (fseek(reader->fhd, store, SEEK_SET) < 0)
				return errno;
		}

		k--;
	}

	while (n > 0) {
		child_indirect_block = readValue(reader,
				reader->superblock.size_of_offsets);

		if (validAddress(reader, child_direct_block)) {
			store = ftell(reader->fhd);
			if (fseek(reader->fhd, child_indirect_block, SEEK_SET) < 0)
				return errno;
			err = indirectblockRead(reader, dataobject, fractalheap,
					iblock_size * 2);
			if (err)
				return err;
			if (store < 0)
				return MYSOFA_READ_ERROR;
			if (fseek(reader->fhd, store, SEEK_SET) < 0)
				return errno;
		}

		n--;
	}

	return MYSOFA_OK;
}