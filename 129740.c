add_custom_modifiers (MonoDynamicImage *assembly, MonoType *type, MonoArray *modreq, MonoArray *modopt)
{
	int i, count, len, pos;
	MonoType *t;

	count = 0;
	if (modreq)
		count += mono_array_length (modreq);
	if (modopt)
		count += mono_array_length (modopt);

	if (count == 0)
		return mono_metadata_type_dup (NULL, type);

	len = MONO_SIZEOF_TYPE + ((gint32)count) * sizeof (MonoCustomMod);
	t = g_malloc (len);
	memcpy (t, type, MONO_SIZEOF_TYPE);

	t->num_mods = count;
	pos = 0;
	if (modreq) {
		for (i = 0; i < mono_array_length (modreq); ++i) {
			MonoType *mod = mono_type_array_get_and_resolve (modreq, i);
			t->modifiers [pos].required = 1;
			t->modifiers [pos].token = mono_image_typedef_or_ref (assembly, mod);
			pos ++;
		}
	}
	if (modopt) {
		for (i = 0; i < mono_array_length (modopt); ++i) {
			MonoType *mod = mono_type_array_get_and_resolve (modopt, i);
			t->modifiers [pos].required = 0;
			t->modifiers [pos].token = mono_image_typedef_or_ref (assembly, mod);
			pos ++;
		}
	}

	return t;
}