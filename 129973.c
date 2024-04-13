static gboolean
verify_valuetype_layout_with_target (MonoClass *class, MonoClass *target_class)
{
	int type;
	gpointer iter = NULL;
	MonoClassField *field;
	MonoClass *field_class;

	if (!class->valuetype)
		return TRUE;

	type = class->byval_arg.type;
	/*primitive type fields are not properly decoded*/
	if ((type >= MONO_TYPE_BOOLEAN && type <= MONO_TYPE_R8) || (type >= MONO_TYPE_I && type <= MONO_TYPE_U))
		return TRUE;

	while ((field = mono_class_get_fields (class, &iter)) != NULL) {
		if (!field->type)
			return FALSE;

		if (field->type->attrs & (FIELD_ATTRIBUTE_STATIC | FIELD_ATTRIBUTE_HAS_FIELD_RVA))
			continue;

		field_class = mono_class_get_generic_type_definition (mono_class_from_mono_type (field->type));

		if (field_class == target_class || class == field_class || !verify_valuetype_layout_with_target (field_class, target_class))
			return FALSE;
	}

	return TRUE;