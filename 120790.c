static int idprime_fill_prkey_info(list_t *list, idprime_object_t **entry, sc_pkcs15_prkey_info_t *prkey_info)
{
	memset(prkey_info, 0, sizeof(sc_pkcs15_prkey_info_t));
	if (*entry == NULL) {
		return SC_ERROR_FILE_END_REACHED;
	}

	prkey_info->path.len = sizeof((*entry)->df);
	memcpy(prkey_info->path.value, (*entry)->df, sizeof((*entry)->df));
	prkey_info->path.type = SC_PATH_TYPE_FILE_ID;
	/* Do not specify the length -- it will be read from the FCI */
	prkey_info->path.count = -1;

	/* TODO figure out the IDs as the original driver? */
	prkey_info->id.value[0] = ((*entry)->fd >> 8) & 0xff;
	prkey_info->id.value[1] = (*entry)->fd & 0xff;
	prkey_info->id.len = 2;
	prkey_info->key_reference = (*entry)->key_reference;
	*entry = list_iterator_next(list);
	return SC_SUCCESS;
}