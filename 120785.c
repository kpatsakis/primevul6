static int idprime_get_serial(sc_card_t* card, sc_serial_number_t* serial)
{
	sc_path_t cardid_path;
	sc_file_t *file = NULL;
	u8 buf[IDPRIME_CARDID_LEN];
	int r;

	LOG_FUNC_CALLED(card->ctx);

	/* XXX this is assumed to be cardid for windows. It can be read from the index file */
	sc_format_path("0201", &cardid_path);
	r = iso_ops->select_file(card, &cardid_path, &file);
	if (r != SC_SUCCESS || file->size != IDPRIME_CARDID_LEN) { /* The cardid is always 16 B */
		sc_file_free(file);
		LOG_FUNC_RETURN(card->ctx, SC_ERROR_WRONG_LENGTH);
	}

	r = iso_ops->read_binary(card, 0, buf, file->size, 0);
	sc_file_free(file);
	if (r < 1) {
		LOG_FUNC_RETURN(card->ctx, r);
	} else if (r != IDPRIME_CARDID_LEN) {
		LOG_FUNC_RETURN(card->ctx, SC_ERROR_INVALID_DATA);
	}

	serial->len = MIN(IDPRIME_CARDID_LEN, SC_MAX_SERIALNR);
	memcpy(serial->value, buf, serial->len);
	LOG_FUNC_RETURN(card->ctx, SC_SUCCESS);
}