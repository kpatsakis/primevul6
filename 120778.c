static int idprime_init(sc_card_t *card)
{
	int r;
	unsigned long flags;
	idprime_private_data_t *priv = NULL;
	struct sc_apdu apdu;
	u8 rbuf[CPLC_LENGTH];
	size_t rbuflen = sizeof(rbuf);

	/* We need to differentiate the OS version since they behave slightly differently */
	sc_format_apdu(card, &apdu, SC_APDU_CASE_2, 0xCA, 0x9F, 0x7F);
	apdu.resp = rbuf;
	apdu.resplen = rbuflen;
	apdu.le = rbuflen;
	r = sc_transmit_apdu(card, &apdu);
	card->type = SC_CARD_TYPE_IDPRIME_GENERIC;
	if (r == SC_SUCCESS && apdu.resplen == CPLC_LENGTH) {
		/* We are interested in the OS release level here */
		switch (rbuf[11]) {
		case 0x01:
			card->type = SC_CARD_TYPE_IDPRIME_V1;
			sc_log(card->ctx, "Detected IDPrime applet version 1");
			break;
		case 0x02:
			card->type = SC_CARD_TYPE_IDPRIME_V2;
			sc_log(card->ctx, "Detected IDPrime applet version 2");
			break;
		default:
			sc_log(card->ctx, "Unknown OS version received: %d", rbuf[11]);
			break;
		}
	} else {
		sc_log(card->ctx, "Failed to get CPLC data or invalid length returned, "
			"err=%d, len=%"SC_FORMAT_LEN_SIZE_T"u",
			r, apdu.resplen);
	}

	/* Now, select and process the index file */
	r = idprime_select_index(card);
	if (r <= 0) {
		LOG_FUNC_RETURN(card->ctx, r);
	}

	sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE, "Index file found");

	priv = idprime_new_private_data();
	if (!priv) {
		return SC_ERROR_OUT_OF_MEMORY;
	}

	r = idprime_process_index(card, priv, r);
	if (r != SC_SUCCESS) {
		idprime_free_private_data(priv);
		LOG_FUNC_RETURN(card->ctx, r);
	}

	card->drv_data = priv;

	switch (card->type) {
	case SC_CARD_TYPE_IDPRIME_V1:
		card->name = "Gemalto IDPrime (OSv1)";
		break;
	case SC_CARD_TYPE_IDPRIME_V2:
		card->name = "Gemalto IDPrime (OSv2)";
		break;
	case SC_CARD_TYPE_IDPRIME_GENERIC:
	default:
		card->name = "Gemalto IDPrime (generic)";
		break;
	}
	card->cla = 0x00;

	/* Set up algorithm info. */
	flags = SC_ALGORITHM_RSA_PAD_PKCS1
		| SC_ALGORITHM_RSA_PAD_PSS
		| SC_ALGORITHM_RSA_PAD_OAEP
		/* SHA-1 mechanisms are not allowed in the card I have */
		| (SC_ALGORITHM_RSA_HASH_SHA256 | SC_ALGORITHM_RSA_HASH_SHA384 | SC_ALGORITHM_RSA_HASH_SHA512)
		| (SC_ALGORITHM_MGF1_SHA256 | SC_ALGORITHM_MGF1_SHA384 | SC_ALGORITHM_MGF1_SHA512)
		;

	_sc_card_add_rsa_alg(card, 1024, flags, 0);
	_sc_card_add_rsa_alg(card, 2048, flags, 0);

	card->caps |= SC_CARD_CAP_ISO7816_PIN_INFO;

	LOG_FUNC_RETURN(card->ctx, 0);
}