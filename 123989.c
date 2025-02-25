sc_pkcs15emu_oberthur_add_pubkey(struct sc_pkcs15_card *p15card,
		unsigned int file_id, unsigned int size)
{
	struct sc_context *ctx = p15card->card->ctx;
	struct sc_pkcs15_pubkey_info key_info;
	struct sc_pkcs15_object key_obj;
	char ch_tmp[0x100];
	unsigned char *info_blob = NULL;
	size_t len, info_len, offs;
	unsigned flags;
	int rv;

	LOG_FUNC_CALLED(ctx);
	sc_log(ctx, "public key(file-id:%04X,size:%X)", file_id, size);

	memset(&key_info, 0, sizeof(key_info));
	memset(&key_obj, 0, sizeof(key_obj));

	snprintf(ch_tmp, sizeof(ch_tmp), "%s%04X", AWP_OBJECTS_DF_PUB, file_id | 0x100);
	rv = sc_oberthur_read_file(p15card, ch_tmp, &info_blob, &info_len, 1);
	LOG_TEST_RET(ctx, rv, "Failed to add public key: read oberthur file error");

	/* Flags */
	offs = 2;
	if (offs > info_len) {
		free(info_blob);
		LOG_TEST_RET(ctx, SC_ERROR_UNKNOWN_DATA_RECEIVED, "Failed to add public key: no 'tag'");
	}
	flags = *(info_blob + 0) * 0x100 + *(info_blob + 1);
	key_info.usage = sc_oberthur_decode_usage(flags);
	if (flags & OBERTHUR_ATTR_MODIFIABLE)
		key_obj.flags = SC_PKCS15_CO_FLAG_MODIFIABLE;
	sc_log(ctx, "Public key key-usage:%04X", key_info.usage);

	/* Label */
	if (offs + 2 > info_len) {
		free(info_blob);
		LOG_TEST_RET(ctx, SC_ERROR_UNKNOWN_DATA_RECEIVED, "Failed to add public key: no 'Label'");
	}
	len = *(info_blob + offs + 1) + *(info_blob + offs) * 0x100;
	if (offs + 2 + len > info_len) {
		free(info_blob);
		LOG_TEST_RET(ctx, SC_ERROR_INVALID_DATA, "Failed to add public key: invalid 'Label' length");
	} else if (len) {
		if (len > sizeof(key_obj.label) - 1)
			len = sizeof(key_obj.label) - 1;
		memcpy(key_obj.label, info_blob + offs + 2, len);
	}
	offs += 2 + len;

	/* ID */
	if (offs + 2 > info_len) {
		free(info_blob);
		LOG_TEST_RET(ctx, SC_ERROR_UNKNOWN_DATA_RECEIVED, "Failed to add public key: no 'ID'");
	}
	len = *(info_blob + offs + 1) + *(info_blob + offs) * 0x100;
	if (len == 0
			|| len > sizeof(key_info.id.value)
			|| offs + 2 + len > info_len) {
		free(info_blob);
		LOG_TEST_RET(ctx, SC_ERROR_INVALID_DATA, "Failed to add public key: invalid 'ID' length");
	}
	memcpy(key_info.id.value, info_blob + offs + 2, len);
	key_info.id.len = len;

	free(info_blob);

	/* Ignore Start/End dates */

	snprintf(ch_tmp, sizeof(ch_tmp), "%s%04X", AWP_OBJECTS_DF_PUB, file_id);
	sc_format_path(ch_tmp, &key_info.path);

	key_info.native = 1;
	key_info.key_reference = file_id & 0xFF;
	key_info.modulus_length = size;

	rv = sc_pkcs15emu_add_rsa_pubkey(p15card, &key_obj, &key_info);

	LOG_FUNC_RETURN(ctx, rv);
}