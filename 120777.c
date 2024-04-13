static int idprime_read_binary(sc_card_t *card, unsigned int offset,
	unsigned char *buf, size_t count, unsigned long flags)
{
	struct idprime_private_data *priv = card->drv_data;
	int r = 0;
	int size;

	sc_log(card->ctx, "called; %"SC_FORMAT_LEN_SIZE_T"u bytes at offset %d",
		count, offset);

	if (!priv->cached && offset == 0) {
		/* Read what was reported by FCI from select command */
		int left = priv->file_size;
		size_t read = 0;

		// this function is called to read and uncompress the certificate
		u8 buffer[SC_MAX_EXT_APDU_BUFFER_SIZE];
		if (sizeof(buffer) < count || sizeof(buffer) < priv->file_size) {
			LOG_FUNC_RETURN(card->ctx, SC_ERROR_INTERNAL);
		}
		while (left > 0) {
			r = iso_ops->read_binary(card, read, buffer + read, priv->file_size - read, flags);
			if (r <= 0) {
				LOG_FUNC_RETURN(card->ctx, r);
			}
			left -= r;
			read += r;
		}
		if (read < 4 || read != priv->file_size) {
			LOG_FUNC_RETURN(card->ctx, SC_ERROR_INVALID_DATA);
		}
		if (buffer[0] == 1 && buffer[1] == 0) {
#ifdef ENABLE_ZLIB
			size_t expectedsize = buffer[2] + buffer[3] * 0x100;
			r = sc_decompress_alloc(&priv->cache_buf, &(priv->cache_buf_len),
				buffer+4, priv->file_size-4, COMPRESSION_AUTO);
			if (r != SC_SUCCESS) {
				sc_log(card->ctx, "Zlib error: %d", r);
				LOG_FUNC_RETURN(card->ctx, r);
			}
			if (priv->cache_buf_len != expectedsize) {
				sc_log(card->ctx,
					 "expected size: %"SC_FORMAT_LEN_SIZE_T"u real size: %"SC_FORMAT_LEN_SIZE_T"u",
					 expectedsize, priv->cache_buf_len);
				LOG_FUNC_RETURN(card->ctx, SC_ERROR_INVALID_DATA);
			}
#else
			sc_log(card->ctx, "compression not supported, no zlib");
			return SC_ERROR_NOT_SUPPORTED;
#endif /* ENABLE_ZLIB */
		} else {
			/* assuming uncompressed certificate */
			priv->cache_buf = malloc(r);
			if (priv->cache_buf == NULL) {
				return SC_ERROR_OUT_OF_MEMORY;
			}
			memcpy(priv->cache_buf, buffer, r);
			priv->cache_buf_len = r;
		}
		priv->cached = 1;
	}
	if (offset >= priv->cache_buf_len) {
		return 0;
	}
	size = (int) MIN((priv->cache_buf_len - offset), count);
	memcpy(buf, priv->cache_buf + offset, size);
	return size;
}