int sc_pkcs15_decode_prkdf_entry(struct sc_pkcs15_card *p15card,
				 struct sc_pkcs15_object *obj,
				 const u8 ** buf, size_t *buflen)
{
	sc_context_t *ctx = p15card->card->ctx;
	struct sc_pkcs15_prkey_info info;
	int r, i, gostr3410_params[3];
	struct sc_pkcs15_keyinfo_gostparams *keyinfo_gostparams;
	size_t usage_len = sizeof(info.usage);
	size_t af_len = sizeof(info.access_flags);
	struct sc_asn1_entry asn1_com_key_attr[C_ASN1_COM_KEY_ATTR_SIZE];
	struct sc_asn1_entry asn1_com_prkey_attr[C_ASN1_COM_PRKEY_ATTR_SIZE];
	struct sc_asn1_entry asn1_rsakey_attr[C_ASN1_RSAKEY_ATTR_SIZE];
	struct sc_asn1_entry asn1_prk_rsa_attr[C_ASN1_PRK_RSA_ATTR_SIZE];
	struct sc_asn1_entry asn1_dsakey_attr[C_ASN1_DSAKEY_ATTR_SIZE];
	struct sc_asn1_entry asn1_prk_dsa_attr[C_ASN1_PRK_DSA_ATTR_SIZE];
	struct sc_asn1_entry asn1_dsakey_i_p_attr[C_ASN1_DSAKEY_I_P_ATTR_SIZE];
	struct sc_asn1_entry asn1_dsakey_value_attr[C_ASN1_DSAKEY_VALUE_ATTR_SIZE];
	struct sc_asn1_entry asn1_gostr3410key_attr[C_ASN1_GOSTR3410KEY_ATTR_SIZE];
	struct sc_asn1_entry asn1_prk_gostr3410_attr[C_ASN1_PRK_GOSTR3410_ATTR_SIZE];
	struct sc_asn1_entry asn1_ecckey_attr[C_ASN1_ECCKEY_ATTR];
	struct sc_asn1_entry asn1_prk_ecc_attr[C_ASN1_PRK_ECC_ATTR];
	struct sc_asn1_entry asn1_prkey[C_ASN1_PRKEY_SIZE];
	struct sc_asn1_entry asn1_supported_algorithms[C_ASN1_SUPPORTED_ALGORITHMS_SIZE];
	struct sc_asn1_pkcs15_object rsa_prkey_obj = {obj, asn1_com_key_attr, asn1_com_prkey_attr, asn1_prk_rsa_attr};
	struct sc_asn1_pkcs15_object dsa_prkey_obj = {obj, asn1_com_key_attr, asn1_com_prkey_attr, asn1_prk_dsa_attr};
	struct sc_asn1_pkcs15_object gostr3410_prkey_obj = {obj, asn1_com_key_attr, asn1_com_prkey_attr, asn1_prk_gostr3410_attr};
	struct sc_asn1_pkcs15_object ecc_prkey_obj = { obj, asn1_com_key_attr, asn1_com_prkey_attr, asn1_prk_ecc_attr };

	sc_copy_asn1_entry(c_asn1_prkey, asn1_prkey);
	sc_copy_asn1_entry(c_asn1_supported_algorithms, asn1_supported_algorithms);

	sc_copy_asn1_entry(c_asn1_prk_rsa_attr, asn1_prk_rsa_attr);
	sc_copy_asn1_entry(c_asn1_rsakey_attr, asn1_rsakey_attr);
	sc_copy_asn1_entry(c_asn1_prk_dsa_attr, asn1_prk_dsa_attr);
	sc_copy_asn1_entry(c_asn1_dsakey_attr, asn1_dsakey_attr);
	sc_copy_asn1_entry(c_asn1_dsakey_value_attr, asn1_dsakey_value_attr);
	sc_copy_asn1_entry(c_asn1_dsakey_i_p_attr, asn1_dsakey_i_p_attr);
	sc_copy_asn1_entry(c_asn1_prk_gostr3410_attr, asn1_prk_gostr3410_attr);
	sc_copy_asn1_entry(c_asn1_gostr3410key_attr, asn1_gostr3410key_attr);
	sc_copy_asn1_entry(c_asn1_prk_ecc_attr, asn1_prk_ecc_attr);
	sc_copy_asn1_entry(c_asn1_ecckey_attr, asn1_ecckey_attr);

	sc_copy_asn1_entry(c_asn1_com_prkey_attr, asn1_com_prkey_attr);
	sc_copy_asn1_entry(c_asn1_com_key_attr, asn1_com_key_attr);

	sc_format_asn1_entry(asn1_prkey + 0, &rsa_prkey_obj, NULL, 0);
	sc_format_asn1_entry(asn1_prkey + 1, &ecc_prkey_obj, NULL, 0);
	sc_format_asn1_entry(asn1_prkey + 2, &dsa_prkey_obj, NULL, 0);
	sc_format_asn1_entry(asn1_prkey + 3, &gostr3410_prkey_obj, NULL, 0);

	sc_format_asn1_entry(asn1_prk_rsa_attr + 0, asn1_rsakey_attr, NULL, 0);
	sc_format_asn1_entry(asn1_prk_dsa_attr + 0, asn1_dsakey_attr, NULL, 0);
	sc_format_asn1_entry(asn1_prk_gostr3410_attr + 0, asn1_gostr3410key_attr, NULL, 0);
	sc_format_asn1_entry(asn1_prk_ecc_attr + 0, asn1_ecckey_attr, NULL, 0);

	sc_format_asn1_entry(asn1_rsakey_attr + 0, &info.path, NULL, 0);
	sc_format_asn1_entry(asn1_rsakey_attr + 1, &info.modulus_length, NULL, 0);

	sc_format_asn1_entry(asn1_dsakey_attr + 0, asn1_dsakey_value_attr, NULL, 0);
	sc_format_asn1_entry(asn1_dsakey_value_attr + 0, &info.path, NULL, 0);
	sc_format_asn1_entry(asn1_dsakey_value_attr + 1, asn1_dsakey_i_p_attr, NULL, 0);
	sc_format_asn1_entry(asn1_dsakey_i_p_attr + 0, &info.path, NULL, 0);

	sc_format_asn1_entry(asn1_gostr3410key_attr + 0, &info.path, NULL, 0);
	sc_format_asn1_entry(asn1_gostr3410key_attr + 1, &gostr3410_params[0], NULL, 0);
	sc_format_asn1_entry(asn1_gostr3410key_attr + 2, &gostr3410_params[1], NULL, 0);
	sc_format_asn1_entry(asn1_gostr3410key_attr + 3, &gostr3410_params[2], NULL, 0);

	sc_format_asn1_entry(asn1_ecckey_attr + 0, &info.path, NULL, 0);
	sc_format_asn1_entry(asn1_ecckey_attr + 1, &info.field_length, NULL, 0);

	sc_format_asn1_entry(asn1_com_key_attr + 0, &info.id, NULL, 0);
	sc_format_asn1_entry(asn1_com_key_attr + 1, &info.usage, &usage_len, 0);
	sc_format_asn1_entry(asn1_com_key_attr + 2, &info.native, NULL, 0);
	sc_format_asn1_entry(asn1_com_key_attr + 3, &info.access_flags, &af_len, 0);
	sc_format_asn1_entry(asn1_com_key_attr + 4, &info.key_reference, NULL, 0);

	for (i=0; i<SC_MAX_SUPPORTED_ALGORITHMS && (asn1_supported_algorithms + i)->name; i++)
		sc_format_asn1_entry(asn1_supported_algorithms + i, &info.algo_refs[i], NULL, 0);
	sc_format_asn1_entry(asn1_com_key_attr + 5, asn1_supported_algorithms, NULL, 0);

	sc_format_asn1_entry(asn1_com_prkey_attr + 0, &info.subject.value, &info.subject.len, 0);

	/* Fill in defaults */
	memset(&info, 0, sizeof(info));
	info.key_reference = -1;
	info.native = 1;
	memset(gostr3410_params, 0, sizeof(gostr3410_params));

	r = sc_asn1_decode_choice(ctx, asn1_prkey, *buf, *buflen, buf, buflen);
	if (r == SC_ERROR_ASN1_END_OF_CONTENTS)
		return r;
	LOG_TEST_RET(ctx, r, "PrKey DF ASN.1 decoding failed");
	if (asn1_prkey[0].flags & SC_ASN1_PRESENT) {
		obj->type = SC_PKCS15_TYPE_PRKEY_RSA;
	}
	else if (asn1_prkey[1].flags & SC_ASN1_PRESENT) {
		obj->type = SC_PKCS15_TYPE_PRKEY_EC;
	}
	else if (asn1_prkey[2].flags & SC_ASN1_PRESENT) {
		obj->type = SC_PKCS15_TYPE_PRKEY_DSA;
		/* If the value was indirect-protected, mark the path */
		if (asn1_dsakey_i_p_attr[0].flags & SC_ASN1_PRESENT)
			info.path.type = SC_PATH_TYPE_PATH_PROT;
	}
	else if (asn1_prkey[3].flags & SC_ASN1_PRESENT) {
		obj->type = SC_PKCS15_TYPE_PRKEY_GOSTR3410;
		assert(info.modulus_length == 0);
		info.modulus_length = SC_PKCS15_GOSTR3410_KEYSIZE;
		assert(info.params.len == 0);
		info.params.len = sizeof(struct sc_pkcs15_keyinfo_gostparams);
		info.params.data = malloc(info.params.len);
		if (info.params.data == NULL)
			LOG_FUNC_RETURN(ctx, SC_ERROR_OUT_OF_MEMORY);
		assert(sizeof(*keyinfo_gostparams) == info.params.len);
		keyinfo_gostparams = info.params.data;
		keyinfo_gostparams->gostr3410 = gostr3410_params[0];
		keyinfo_gostparams->gostr3411 = gostr3410_params[1];
		keyinfo_gostparams->gost28147 = gostr3410_params[2];
	}
	else {
		sc_log(ctx, "Neither RSA or DSA or GOSTR3410 or ECC key in PrKDF entry.");
		LOG_FUNC_RETURN(ctx, SC_ERROR_INVALID_ASN1_OBJECT);
	}

	if (!p15card->app || !p15card->app->ddo.aid.len)   {
		r = sc_pkcs15_make_absolute_path(&p15card->file_app->path, &info.path);
		if (r < 0) {
			sc_pkcs15_free_key_params(&info.params);
			return r;
		}
	}
	else   {
		info.path.aid = p15card->app->ddo.aid;
	}
	sc_log(ctx, "PrivKey path '%s'", sc_print_path(&info.path));

	/* OpenSC 0.11.4 and older encoded "keyReference" as a negative value.
	 * Fixed in 0.11.5 we need to add a hack, so old cards continue to work. */
	if (info.key_reference < -1)
		info.key_reference += 256;

	/* Check the auth_id - if not present, try and find it in access rules */
	if ((obj->flags & SC_PKCS15_CO_FLAG_PRIVATE) && (obj->auth_id.len == 0)) {
		sc_log(ctx, "Private key %s has no auth ID - checking AccessControlRules",
				sc_pkcs15_print_id(&info.id));

		/* Search in the access_rules for an appropriate auth ID */
		for (i = 0; i < SC_PKCS15_MAX_ACCESS_RULES; i++) {
			/* If access_mode is one of the private key usage modes */
			if (obj->access_rules[i].access_mode &
					(SC_PKCS15_ACCESS_RULE_MODE_EXECUTE |
					 SC_PKCS15_ACCESS_RULE_MODE_PSO_CDS |
					 SC_PKCS15_ACCESS_RULE_MODE_PSO_DECRYPT |
					 SC_PKCS15_ACCESS_RULE_MODE_INT_AUTH)) {
				if (obj->access_rules[i].auth_id.len != 0) {
					/* Found an auth ID to use for private key access */
					obj->auth_id = obj->access_rules[i].auth_id;
					sc_log(ctx, "Auth ID found - %s",
						 sc_pkcs15_print_id(&obj->auth_id));
					break;
				}
			}
		}

		/* No auth ID found */
		if (i == SC_PKCS15_MAX_ACCESS_RULES)
			sc_log(ctx, "Warning: No auth ID found");
	}

	obj->data = malloc(sizeof(info));
	if (obj->data == NULL) {
		sc_pkcs15_free_key_params(&info.params);
		LOG_FUNC_RETURN(ctx, SC_ERROR_OUT_OF_MEMORY);
	}
	memcpy(obj->data, &info, sizeof(info));

	sc_log(ctx, "Key Subject %s", sc_dump_hex(info.subject.value, info.subject.len));
	sc_log(ctx, "Key path %s", sc_print_path(&info.path));
	return 0;
}