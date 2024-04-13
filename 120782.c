void idprime_free_private_data(idprime_private_data_t *priv)
{
	free(priv->cache_buf);
	list_destroy(&priv->pki_list);
	free(priv);
	return;
}