idprime_decipher(struct sc_card *card,
	const u8 * crgram, size_t crgram_len,
	u8 * out, size_t outlen)
{
	int r;
	struct sc_apdu apdu;
	u8 *sbuf = NULL;

	if (card == NULL || crgram == NULL || out == NULL) {
		return SC_ERROR_INVALID_ARGUMENTS;
	}
	LOG_FUNC_CALLED(card->ctx);
	sc_log(card->ctx,
		"IDPrime decipher: in-len %"SC_FORMAT_LEN_SIZE_T"u, out-len %"SC_FORMAT_LEN_SIZE_T"u",
		crgram_len, outlen);

	sbuf = malloc(crgram_len + 1);
	if (sbuf == NULL)
		return SC_ERROR_OUT_OF_MEMORY;

	/* INS: 0x2A  PERFORM SECURITY OPERATION
	 * P1:  0x80  Resp: Plain value
	 * P2:  0x86  Cmd: Padding indicator byte followed by cryptogram */
	sc_format_apdu(card, &apdu, SC_APDU_CASE_4, 0x2A, 0x80, 0x86);
	apdu.resp    = out;
	apdu.resplen = outlen;
	apdu.le      = outlen;

	sbuf[0] = 0x81; /* padding indicator byte, 0x81 = Proprietary */
	memcpy(sbuf + 1, crgram, crgram_len);
	apdu.data = sbuf;
	apdu.lc = crgram_len + 1;
	if (apdu.lc > sc_get_max_send_size(card)) {
		/* The lower layers will automatically do chaining */
		apdu.flags |= SC_APDU_FLAGS_CHAINING;
	}
	if (apdu.le > sc_get_max_recv_size(card)) {
		/* The lower layers will automatically do a GET RESPONSE, if possible.
		 * All other workarounds must be carried out by the upper layers. */
		apdu.le = sc_get_max_recv_size(card);
	}
	apdu.datalen = crgram_len + 1;

	r = sc_transmit_apdu(card, &apdu);
	sc_mem_clear(sbuf, crgram_len + 1);
	free(sbuf);
	LOG_TEST_RET(card->ctx, r, "APDU transmit failed");

	if (apdu.sw1 == 0x90 && apdu.sw2 == 0x00)
		LOG_FUNC_RETURN(card->ctx, apdu.resplen);
	else
		LOG_FUNC_RETURN(card->ctx, sc_check_sw(card, apdu.sw1, apdu.sw2));
}