mono_image_get_fieldref_token (MonoDynamicImage *assembly, MonoObject *f, MonoClassField *field)
{
	MonoType *type;
	guint32 token;

	g_assert (field);
	g_assert (field->parent);

	token = GPOINTER_TO_UINT (mono_g_hash_table_lookup (assembly->handleref_managed, f));
	if (token)
		return token;

	if (field->parent->generic_class && field->parent->generic_class->container_class && field->parent->generic_class->container_class->fields) {
		int index = field - field->parent->fields;
		type = field->parent->generic_class->container_class->fields [index].type;
	} else {
		if (is_field_on_inst (field))
			type = get_field_on_inst_generic_type (field);
		else
			type = field->type;
	}
	token = mono_image_get_memberref_token (assembly, &field->parent->byval_arg,
											mono_field_get_name (field),
											fieldref_encode_signature (assembly, field->parent->image, type));
	mono_g_hash_table_insert (assembly->handleref_managed, f, GUINT_TO_POINTER(token));
	return token;
}