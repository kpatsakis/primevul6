ca_x509_subjectaltname_do(X509 *cert, int mode, const char *logmsg,
    struct iked_static_id *id, struct iked_id *retid)
{
	STACK_OF(GENERAL_NAME) *stack = NULL;
	GENERAL_NAME *entry;
	ASN1_STRING *cstr;
	char idstr[IKED_ID_SIZE];
	int idx, ret, i, type, len;
	uint8_t *data;

	ret = -1;
	idx = -1;
	while ((stack = X509_get_ext_d2i(cert, NID_subject_alt_name,
	    NULL, &idx)) != NULL) {
		for (i = 0; i < sk_GENERAL_NAME_num(stack); i++) {
			entry = sk_GENERAL_NAME_value(stack, i);
			switch (entry->type) {
			case GEN_DNS:
				cstr = entry->d.dNSName;
				if (ASN1_STRING_type(cstr) != V_ASN1_IA5STRING)
					continue;
				type = IKEV2_ID_FQDN;
				break;
			case GEN_EMAIL:
				cstr = entry->d.rfc822Name;
				if (ASN1_STRING_type(cstr) != V_ASN1_IA5STRING)
					continue;
				type = IKEV2_ID_UFQDN;
				break;
			case GEN_IPADD:
				cstr = entry->d.iPAddress;
				switch (ASN1_STRING_length(cstr)) {
				case 4:
					type = IKEV2_ID_IPV4;
					break;
				case 16:
					type = IKEV2_ID_IPV6;
					break;
				default:
					log_debug("%s: invalid subjectAltName"
					   " IP address", __func__);
					continue;
				}
				break;
			default:
				continue;
			}
			len = ASN1_STRING_length(cstr);
			data = ASN1_STRING_data(cstr);
			if (mode == MODE_ALT_LOG) {
				struct iked_id sanid;

				bzero(&sanid, sizeof(sanid));
				sanid.id_offset = 0;
				sanid.id_type = type;
				if ((sanid.id_buf = ibuf_new(data, len))
				    == NULL) {
					log_debug("%s: failed to get id buffer",
					     __func__);
					continue;
				}
				ikev2_print_id(&sanid, idstr, sizeof(idstr));
				log_info("%s: altname: %s", logmsg, idstr);
				ibuf_release(sanid.id_buf);
				sanid.id_buf = NULL;
			}
			/* Compare length and data */
			if (mode == MODE_ALT_CMP) {
				if (type == id->id_type &&
				    (len == (id->id_length - id->id_offset)) &&
				    (memcmp(id->id_data + id->id_offset,
				    data, len)) == 0) {
					ret = 0;
					break;
				}
			}
			/* Get first ID */
			if (mode == MODE_ALT_GET) {
				ibuf_release(retid->id_buf);
				if ((retid->id_buf = ibuf_new(data, len)) == NULL) {
					log_debug("%s: failed to get id buffer",
					    __func__);
					ret = -2;
					break;
				}
				retid->id_offset = 0;
				ikev2_print_id(retid, idstr, sizeof(idstr));
				log_debug("%s: %s", __func__, idstr);
				ret = 0;
				break;
			}
		}
		sk_GENERAL_NAME_pop_free(stack, GENERAL_NAME_free);
		if (ret != -1)
			break;
	}
	if (idx == -1)
		log_debug("%s: did not find subjectAltName in certificate",
		    __func__);
	return ret;
}