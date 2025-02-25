sc_pkcs15emu_oberthur_add_data(struct sc_pkcs15_card *p15card,
		unsigned int file_id, unsigned int size, int private)
{
	struct sc_context *ctx = p15card->card->ctx;
	struct sc_pkcs15_data_info dinfo;
	struct sc_pkcs15_object dobj;
	unsigned flags;
	unsigned char *info_blob = NULL, *label = NULL, *app = NULL, *oid = NULL;
	size_t info_len, label_len, app_len, oid_len, offs;
	char ch_tmp[0x100];
	int rv;

	SC_FUNC_CALLED(ctx, SC_LOG_DEBUG_VERBOSE);
	sc_log(ctx, "Add data(file-id:%04X,size:%i,is-private:%i)", file_id, size, private);
	memset(&dinfo, 0, sizeof(dinfo));
	memset(&dobj, 0, sizeof(dobj));

	snprintf(ch_tmp, sizeof(ch_tmp), "%s%04X", private ? AWP_OBJECTS_DF_PRV : AWP_OBJECTS_DF_PUB, file_id | 0x100);

	rv = sc_oberthur_read_file(p15card, ch_tmp, &info_blob, &info_len, 1);
	LOG_TEST_RET(ctx, rv, "Failed to add data: read oberthur file error");

	if (info_len < 2) {
		free(info_blob);
		LOG_TEST_RET(ctx, SC_ERROR_UNKNOWN_DATA_RECEIVED, "Failed to add certificate: no 'tag'");
	}
	flags = *(info_blob + 0) * 0x100 + *(info_blob + 1);
	offs = 2;

	/* Label */
	if (offs + 2 > info_len) {
		free(info_blob);
		LOG_TEST_RET(ctx, SC_ERROR_UNKNOWN_DATA_RECEIVED, "Failed to add data: no 'label'");
	}
	label = info_blob + offs + 2;
	label_len = *(info_blob + offs + 1) + *(info_blob + offs) * 0x100;
	if (offs + 2 + label_len > info_len) {
		free(info_blob);
		LOG_TEST_RET(ctx, SC_ERROR_UNKNOWN_DATA_RECEIVED, "Invalid length of 'label' received");
	}
	if (label_len > sizeof(dobj.label) - 1)
		label_len = sizeof(dobj.label) - 1;
	offs += 2 + *(info_blob + offs + 1);

	/* Application */
	if (offs + 2 > info_len) {
		free(info_blob);
		LOG_TEST_RET(ctx, SC_ERROR_UNKNOWN_DATA_RECEIVED, "Failed to add data: no 'application'");
	}
	app = info_blob + offs + 2;
	app_len = *(info_blob + offs + 1) + *(info_blob + offs) * 0x100;
	if (offs + 2 + app_len > info_len) {
		free(info_blob);
		LOG_TEST_RET(ctx, SC_ERROR_UNKNOWN_DATA_RECEIVED, "Invalid length of 'application' received");
	}
	if (app_len > sizeof(dinfo.app_label) - 1)
		app_len = sizeof(dinfo.app_label) - 1;
	offs += 2 + app_len;

	/* OID encode like DER(ASN.1(oid)) */
	if (offs + 2 > info_len) {
		free(info_blob);
		LOG_TEST_RET(ctx, SC_ERROR_UNKNOWN_DATA_RECEIVED, "Failed to add data: no 'OID'");
	}
	oid_len = *(info_blob + offs + 1) + *(info_blob + offs) * 0x100;
	if (offs + 2 + oid_len > info_len) {
		free(info_blob);
		LOG_TEST_RET(ctx, SC_ERROR_UNKNOWN_DATA_RECEIVED, "Invalid length of 'oid' received");
	}
	if (oid_len > 2) {
		oid = info_blob + offs + 2;
		if (*oid != 0x06 || (*(oid + 1) != oid_len - 2)) {
			free(info_blob);
			LOG_TEST_RET(ctx, SC_ERROR_UNKNOWN_DATA_RECEIVED, "Failed to add data: invalid 'OID' format");
		}
		oid += 2;
		oid_len -= 2;
	}

	snprintf(ch_tmp, sizeof(ch_tmp), "%s%04X", private ? AWP_OBJECTS_DF_PRV : AWP_OBJECTS_DF_PUB, file_id);

	sc_format_path(ch_tmp, &dinfo.path);

	memcpy(dobj.label, label, label_len);
	memcpy(dinfo.app_label, app, app_len);
	if (oid_len)
		sc_asn1_decode_object_id(oid, oid_len, &dinfo.app_oid);

	if (flags & OBERTHUR_ATTR_MODIFIABLE)
		dobj.flags |= SC_PKCS15_CO_FLAG_MODIFIABLE;

	if (private)   {
		dobj.auth_id.len = sizeof(PinDomainID) > sizeof(dobj.auth_id.value)
				? sizeof(dobj.auth_id.value) : sizeof(PinDomainID);
		memcpy(dobj.auth_id.value, PinDomainID, dobj.auth_id.len);

		dobj.flags |= SC_PKCS15_CO_FLAG_PRIVATE;
	}

	rv = sc_pkcs15emu_add_data_object(p15card, &dobj, &dinfo);

	free(info_blob);
	LOG_FUNC_RETURN(p15card->card->ctx, rv);
}