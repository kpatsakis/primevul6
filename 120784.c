static int idprime_card_ctl(sc_card_t *card, unsigned long cmd, void *ptr)
{
	idprime_private_data_t * priv = card->drv_data;

	LOG_FUNC_CALLED(card->ctx);
	sc_log(card->ctx, "cmd=%ld ptr=%p", cmd, ptr);

	if (priv == NULL) {
		LOG_FUNC_RETURN(card->ctx, SC_ERROR_INTERNAL);
	}
	switch (cmd) {
		case SC_CARDCTL_GET_SERIALNR:
			return idprime_get_serial(card, (sc_serial_number_t *) ptr);
		case SC_CARDCTL_IDPRIME_GET_TOKEN_NAME:
			return idprime_get_token_name(card, (char **) ptr);
		case SC_CARDCTL_IDPRIME_INIT_GET_OBJECTS:
			return idprime_get_init_and_get_count(&priv->pki_list, &priv->pki_current,
				(int *)ptr);
		case SC_CARDCTL_IDPRIME_GET_NEXT_OBJECT:
			return idprime_fill_prkey_info(&priv->pki_list, &priv->pki_current,
				(sc_pkcs15_prkey_info_t *)ptr);
		case SC_CARDCTL_IDPRIME_FINAL_GET_OBJECTS:
			return idprime_final_iterator(&priv->pki_list);
	}

	LOG_FUNC_RETURN(card->ctx, SC_ERROR_NOT_SUPPORTED);
}