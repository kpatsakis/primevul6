static int idprime_finish(sc_card_t *card)
{
	idprime_private_data_t * priv = card->drv_data;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);
	if (priv) {
		idprime_free_private_data(priv);
	}
	return SC_SUCCESS;
}