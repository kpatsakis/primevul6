static gboolean
field_equals (gconstpointer _a, gconstpointer _b)
{
	const MonoClassField *a = _a;
	const MonoClassField *b = _b;
	return !strcmp (a->name, b->name) && mono_metadata_type_equal (a->type, b->type);