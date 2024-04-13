static gboolean
verify_class_fields (MonoClass *class)
{
	gpointer iter = NULL;
	MonoClassField *field;
	MonoGenericContext *context = mono_class_get_context (class);
	GHashTable *unique_fields = g_hash_table_new_full (&field_hash, &field_equals, NULL, NULL);
	if (class->generic_container)
		context = &class->generic_container->context;

	while ((field = mono_class_get_fields (class, &iter)) != NULL) {
		if (!mono_type_is_valid_type_in_context (field->type, context)) {
			g_hash_table_destroy (unique_fields);
			return FALSE;
		}
		if (g_hash_table_lookup (unique_fields, field)) {
			g_hash_table_destroy (unique_fields);
			return FALSE;
		}
		g_hash_table_insert (unique_fields, field, field);
	}
	g_hash_table_destroy (unique_fields);
	return TRUE;