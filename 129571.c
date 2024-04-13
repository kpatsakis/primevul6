int extract_one_using_blocks(struct archive* a, int block_size, uint32_t crc)
{
	uint8_t* buf;
	int ret = 1;
	uint32_t computed_crc = 0;
	la_ssize_t bytes_read;

	buf = malloc(block_size);
	if(buf == NULL)
		return 1;

	while(1) {
		bytes_read = archive_read_data(a, buf, block_size);
		if(bytes_read == ARCHIVE_RETRY)
			continue;
		else if(bytes_read == 0)
			break;
		else if(bytes_read < 0) {
			/* If we're here, it means the decompressor has failed
			 * to properly decode test file. */
			assertA(0);
			ret = 1;
			goto fn_exit;
		} else {
			/* ok */
		}

		computed_crc = crc32(computed_crc, buf, bytes_read);
	}

	assertEqualInt(computed_crc, crc);
	ret = 0;

fn_exit:
	free(buf);
	return ret;
}