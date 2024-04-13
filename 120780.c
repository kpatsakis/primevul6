idprime_private_data_t *idprime_new_private_data(void)
{
	idprime_private_data_t *priv;

	priv = calloc(1, sizeof(idprime_private_data_t));
	if (priv == NULL)
		return NULL;

	/* Initialize PKI Applets list */
	if (list_init(&priv->pki_list) != 0 ||
	    list_attributes_copy(&priv->pki_list, idprime_list_meter, 1) != 0) {
		idprime_free_private_data(priv);
		return NULL;
	}

	return priv;
}