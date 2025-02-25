zipx_lzma_alone_init(struct archive_read *a, struct zip *zip)
{
	lzma_ret r;
	const uint8_t* p;

#pragma pack(push)
#pragma pack(1)
	struct _alone_header {
	    uint8_t bytes[5];
	    uint64_t uncompressed_size;
	} alone_header;
#pragma pack(pop)

	/* To unpack ZIPX's "LZMA" (id 14) stream we can use standard liblzma that
	 * is a part of XZ Utils. The stream format stored inside ZIPX file is a
	 * modified "lzma alone" file format, that was used by the `lzma` utility
	 * which was later deprecated in favour of `xz` utility. Since those
	 * formats are nearly the same, we can use a standard "lzma alone" decoder
	 * from XZ Utils. */

	memset(&zip->zipx_lzma_stream, 0, sizeof(zip->zipx_lzma_stream));
	r = lzma_alone_decoder(&zip->zipx_lzma_stream, UINT64_MAX);
	if (r != LZMA_OK) {
		archive_set_error(&(a->archive), ARCHIVE_ERRNO_MISC,
		    "lzma initialization failed(%d)", r);

		return (ARCHIVE_FAILED);
	}

	/* Flag the cleanup function that we want our lzma-related structures
	 * to be freed later. */
	zip->zipx_lzma_valid = 1;

	/* The "lzma alone" file format and the stream format inside ZIPx are
	 * almost the same. Here's an example of a structure of "lzma alone"
	 * format:
	 *
	 * $ cat /bin/ls | lzma | xxd | head -n 1
	 * 00000000: 5d00 0080 00ff ffff ffff ffff ff00 2814
	 *
	 *    5 bytes        8 bytes        n bytes
	 * <lzma_params><uncompressed_size><data...>
	 *
	 * lzma_params is a 5-byte blob that has to be decoded to extract
	 * parameters of this LZMA stream. The uncompressed_size field is an
	 * uint64_t value that contains information about the size of the
	 * uncompressed file, or UINT64_MAX if this value is unknown. The <data...>
	 * part is the actual lzma-compressed data stream.
	 *
	 * Now here's the structure of the stream inside the ZIPX file:
	 *
	 * $ cat stream_inside_zipx | xxd | head -n 1
	 * 00000000: 0914 0500 5d00 8000 0000 2814 .... ....
	 *
	 *  2byte   2byte    5 bytes     n bytes
	 * <magic1><magic2><lzma_params><data...>
	 *
	 * This means that the ZIPX file contains an additional magic1 and magic2
	 * headers, the lzma_params field contains the same parameter set as in the
	 * "lzma alone" format, and the <data...> field is the same as in the "lzma
	 * alone" format as well. Note that also the zipx format is missing the
	 * uncompressed_size field.
	 *
	 * So, in order to use the "lzma alone" decoder for the zipx lzma stream,
	 * we simply need to shuffle around some fields, prepare a new lzma alone
	 * header, feed it into lzma alone decoder so it will initialize itself
	 * properly, and then we can start feeding normal zipx lzma stream into the
	 * decoder.
	 */

	/* Read magic1,magic2,lzma_params from the ZIPX stream. */
	if((p = __archive_read_ahead(a, 9, NULL)) == NULL) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
		    "Truncated lzma data");
		return (ARCHIVE_FATAL);
	}

	if(p[2] != 0x05 || p[3] != 0x00) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
		    "Invalid lzma data");
		return (ARCHIVE_FATAL);
	}

	/* Prepare an lzma alone header: copy the lzma_params blob into a proper
	 * place into the lzma alone header. */
	memcpy(&alone_header.bytes[0], p + 4, 5);

	/* Initialize the 'uncompressed size' field to unknown; we'll manually
	 * monitor how many bytes there are still to be uncompressed. */
	alone_header.uncompressed_size = UINT64_MAX;

	if(!zip->uncompressed_buffer) {
		zip->uncompressed_buffer_size = 256 * 1024;
		zip->uncompressed_buffer =
			(uint8_t*) malloc(zip->uncompressed_buffer_size);

		if (zip->uncompressed_buffer == NULL) {
			archive_set_error(&a->archive, ENOMEM,
			    "No memory for lzma decompression");
			return (ARCHIVE_FATAL);
		}
	}

	zip->zipx_lzma_stream.next_in = (void*) &alone_header;
	zip->zipx_lzma_stream.avail_in = sizeof(alone_header);
	zip->zipx_lzma_stream.total_in = 0;
	zip->zipx_lzma_stream.next_out = zip->uncompressed_buffer;
	zip->zipx_lzma_stream.avail_out = zip->uncompressed_buffer_size;
	zip->zipx_lzma_stream.total_out = 0;

	/* Feed only the header into the lzma alone decoder. This will effectively
	 * initialize the decoder, and will not produce any output bytes yet. */
	r = lzma_code(&zip->zipx_lzma_stream, LZMA_RUN);
	if (r != LZMA_OK) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_PROGRAMMER,
		    "lzma stream initialization error");
		return ARCHIVE_FATAL;
	}

	/* We've already consumed some bytes, so take this into account. */
	__archive_read_consume(a, 9);
	zip->entry_bytes_remaining -= 9;
	zip->entry_compressed_bytes_read += 9;

	zip->decompress_init = 1;
	return (ARCHIVE_OK);
}