static gboolean
verify_class_for_overlapping_reference_fields (MonoClass *class)
{
	int i = 0, j;
	gpointer iter = NULL;
	MonoClassField *field;
	gboolean is_fulltrust = mono_verifier_is_class_full_trust (class);
	/*We can't skip types with !has_references since this is calculated after we have run.*/
	if (!((class->flags & TYPE_ATTRIBUTE_LAYOUT_MASK) == TYPE_ATTRIBUTE_EXPLICIT_LAYOUT))
		return TRUE;


	/*We must check for stuff overlapping reference fields.
	  The outer loop uses mono_class_get_fields to ensure that MonoClass:fields get inited.
	*/
	while ((field = mono_class_get_fields (class, &iter))) {
		int fieldEnd = get_field_end (field);
		gboolean is_valuetype = !MONO_TYPE_IS_REFERENCE (field->type);
		++i;

		if (mono_field_is_deleted (field) || (field->type->attrs & FIELD_ATTRIBUTE_STATIC))
			continue;

		for (j = i; j < class->field.count; ++j) {
			MonoClassField *other = &class->fields [j];
			int otherEnd = get_field_end (other);
			if (mono_field_is_deleted (other) || (is_valuetype && !MONO_TYPE_IS_REFERENCE (other->type)) || (other->type->attrs & FIELD_ATTRIBUTE_STATIC))
				continue;

			if (!is_valuetype && MONO_TYPE_IS_REFERENCE (other->type) && field->offset == other->offset && is_fulltrust)
				continue;

			if ((otherEnd > field->offset && otherEnd <= fieldEnd) || (other->offset >= field->offset && other->offset < fieldEnd))
				return FALSE;
		}
	}
	return TRUE;