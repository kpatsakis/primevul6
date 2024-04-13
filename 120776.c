int idprime_add_object_to_list(list_t *list, const idprime_object_t *object)
{
	if (list_append(list, object) < 0)
		return SC_ERROR_INTERNAL;
	return SC_SUCCESS;
}