compare_field_marshal (const void *a, const void *b)
{
	const guint32 *a_values = a;
	const guint32 *b_values = b;

	return a_values [MONO_FIELD_MARSHAL_PARENT] - b_values [MONO_FIELD_MARSHAL_PARENT];
}