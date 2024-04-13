int fractalheapRead(struct READER *reader, struct DATAOBJECT *dataobject,
		struct FRACTALHEAP *fractalheap) {
	int err;
	char buf[4];

	/* read signature */
	if (fread(buf, 1, 4, reader->fhd) != 4 || strncmp(buf, "FRHP", 4)) {
		log("cannot read signature of fractal heap\n");
		return MYSOFA_UNSUPPORTED_FORMAT;
	}
	log("%" PRIX64 " %.4s\n", (uint64_t )ftell(reader->fhd) - 4, buf);

	if (fgetc(reader->fhd) != 0) {
		log("object fractal heap must have version 0\n");
		return MYSOFA_UNSUPPORTED_FORMAT;
	}

	fractalheap->heap_id_length = (uint16_t) readValue(reader, 2);
	fractalheap->encoded_length = (uint16_t) readValue(reader, 2);
	if (fractalheap->encoded_length > 0x8000)
		return MYSOFA_UNSUPPORTED_FORMAT;
	fractalheap->flags = (uint8_t) fgetc(reader->fhd);
	fractalheap->maximum_size = (uint32_t) readValue(reader, 4);

	fractalheap->next_huge_object_id = readValue(reader,
			reader->superblock.size_of_lengths);
	fractalheap->btree_address_of_huge_objects = readValue(reader,
			reader->superblock.size_of_offsets);
	fractalheap->free_space = readValue(reader,
			reader->superblock.size_of_lengths);
	fractalheap->address_free_space = readValue(reader,
			reader->superblock.size_of_offsets);
	fractalheap->amount_managed_space = readValue(reader,
			reader->superblock.size_of_lengths);
	fractalheap->amount_allocated_space = readValue(reader,
			reader->superblock.size_of_lengths);
	fractalheap->offset_managed_space = readValue(reader,
			reader->superblock.size_of_lengths);
	fractalheap->number_managed_objects = readValue(reader,
			reader->superblock.size_of_lengths);
	fractalheap->size_huge_objects = readValue(reader,
			reader->superblock.size_of_lengths);
	fractalheap->number_huge_objects = readValue(reader,
			reader->superblock.size_of_lengths);
	fractalheap->size_tiny_objects = readValue(reader,
			reader->superblock.size_of_lengths);
	fractalheap->number_tiny_objects = readValue(reader,
			reader->superblock.size_of_lengths);

	fractalheap->table_width = (uint16_t) readValue(reader, 2);

	fractalheap->starting_block_size = readValue(reader,
			reader->superblock.size_of_lengths);
	fractalheap->maximum_direct_block_size = readValue(reader,
			reader->superblock.size_of_lengths);

	fractalheap->maximum_heap_size = (uint16_t) readValue(reader, 2);
	fractalheap->starting_row = (uint16_t) readValue(reader, 2);

	fractalheap->address_of_root_block = readValue(reader,
			reader->superblock.size_of_offsets);

	fractalheap->current_row = (uint16_t) readValue(reader, 2);

	if (fractalheap->encoded_length > 0) {

		fractalheap->size_of_filtered_block = readValue(reader,
				reader->superblock.size_of_lengths);
		fractalheap->fitler_mask = (uint32_t) readValue(reader, 4);

		fractalheap->filter_information = malloc(fractalheap->encoded_length);
		if (!fractalheap->filter_information)
			return MYSOFA_NO_MEMORY;

		if (fread(fractalheap->filter_information, 1,
				fractalheap->encoded_length, reader->fhd)
				!= fractalheap->encoded_length) {
			free(fractalheap->filter_information);
			return MYSOFA_READ_ERROR;
		}
	}

	if (fseek(reader->fhd, 4, SEEK_CUR) < 0) { /* skip checksum */
		return MYSOFA_READ_ERROR;
	}

	if (fractalheap->number_huge_objects) {
		log("cannot handle huge objects\n");
		return MYSOFA_UNSUPPORTED_FORMAT;
	}

	if (fractalheap->number_tiny_objects) {
		log("cannot handle tiny objects\n");
		return MYSOFA_UNSUPPORTED_FORMAT;
	}

	if (validAddress(reader, fractalheap->address_of_root_block)) {

		if (fseek(reader->fhd, fractalheap->address_of_root_block, SEEK_SET)
				< 0)
			return errno;
		if (fractalheap->current_row)
			err = indirectblockRead(reader, dataobject, fractalheap,
					fractalheap->starting_block_size);
		else {
			err = directblockRead(reader, dataobject, fractalheap);

		}
		if (err)
			return err;
	}

	return MYSOFA_OK;
}