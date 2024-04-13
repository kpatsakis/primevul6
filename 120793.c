static int idprime_process_index(sc_card_t *card, idprime_private_data_t *priv, int length)
{
	u8 *buf = NULL;
	int r = SC_ERROR_OUT_OF_MEMORY;
	int i, num_entries;
	idprime_object_t new_object;

	buf = malloc(length);
	if (buf == NULL) {
		goto done;
	}

	r = iso_ops->read_binary(card, 0, buf, length, 0);
	if (r < 1) {
		r = SC_ERROR_WRONG_LENGTH;
		goto done;
	}

	/* First byte shows the number of entries, each of them 21 bytes long */
	num_entries = buf[0];
	if (r < num_entries*21 + 1) {
		r = SC_ERROR_INVALID_DATA;
		goto done;
	}
	new_object.fd = 0;
	for (i = 0; i < num_entries; i++) {
		u8 *start = &buf[i*21+1];

		/* First two bytes specify the object DF */
		new_object.df[0] = start[0];
		new_object.df[1] = start[1];
		/* Second two bytes refer to the object size */
		new_object.length = bebytes2ushort(&start[2]);
		sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE, "df=%s, len=%u",
			sc_dump_hex(new_object.df, sizeof(new_object.df)), new_object.length);
		/* in minidriver, mscp/kxcNN or kscNN lists certificates */
		if (((memcmp(&start[4], "ksc", 3) == 0) || memcmp(&start[4], "kxc", 3) == 0)
			&& (memcmp(&start[12], "mscp", 5) == 0)) {
			new_object.fd++;
			if (card->type == SC_CARD_TYPE_IDPRIME_V2) {
				/* The key reference starts from 0x11 and increments by the key id (ASCII) */
				int key_id = 0;
				if (start[8] >= '0' && start[8] <= '9') {
					key_id = start[8] - '0';
				}
				new_object.key_reference = 0x11 + key_id;
			} else {
				/* The key reference is one bigger than the value found here for some reason */
				new_object.key_reference = start[8] + 1;
			}
			sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE, "Found certificate with fd=%d, key_ref=%d",
				new_object.fd, new_object.key_reference);
			idprime_add_object_to_list(&priv->pki_list, &new_object);

		/* This looks like non-standard extension listing pkcs11 token info label in my card */
		} else if ((memcmp(&start[4], "tinfo", 6) == 0) && (memcmp(&start[12], "p11", 4) == 0)) {
			memcpy(priv->tinfo_df, new_object.df, sizeof(priv->tinfo_df));
			priv->tinfo_present = 1;
			sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE, "Found p11/tinfo object");
		}
	}
	r = SC_SUCCESS;
done:
	free(buf);
	LOG_FUNC_RETURN(card->ctx, r);
}