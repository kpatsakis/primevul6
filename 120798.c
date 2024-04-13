static int idprime_get_init_and_get_count(list_t *list, idprime_object_t **entry, int *countp)
{
	if (countp == NULL || entry == NULL) {
		return SC_ERROR_INVALID_ARGUMENTS;
	}
	*countp = list_size(list);
	list_iterator_start(list);
	*entry = list_iterator_next(list);
	return SC_SUCCESS;
}