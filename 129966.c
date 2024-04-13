mono_image_get_generic_field_token (MonoDynamicImage *assembly, MonoReflectionFieldBuilder *fb)
{
	MonoDynamicTable *table;
	MonoClass *klass;
	MonoType *custom = NULL;
	guint32 *values;
	guint32 token, pclass, parent, sig;
	gchar *name;

	token = GPOINTER_TO_UINT (mono_g_hash_table_lookup (assembly->handleref_managed, fb));
	if (token)
		return token;

	klass = mono_class_from_mono_type (mono_reflection_type_get_handle (fb->typeb));
	name = mono_string_to_utf8 (fb->name);

	/* fb->type does not include the custom modifiers */
	/* FIXME: We should do this in one place when a fieldbuilder is created */
	if (fb->modreq || fb->modopt) {
		custom = add_custom_modifiers (assembly, mono_reflection_type_get_handle ((MonoReflectionType*)fb->type), fb->modreq, fb->modopt);
		sig = fieldref_encode_signature (assembly, NULL, custom);
		g_free (custom);
	} else {
		sig = fieldref_encode_signature (assembly, NULL, mono_reflection_type_get_handle ((MonoReflectionType*)fb->type));
	}

	parent = create_generic_typespec (assembly, (MonoReflectionTypeBuilder *) fb->typeb);
	g_assert ((parent & MONO_TYPEDEFORREF_MASK) == MONO_TYPEDEFORREF_TYPESPEC);
	
	pclass = MONO_MEMBERREF_PARENT_TYPESPEC;
	parent >>= MONO_TYPEDEFORREF_BITS;

	table = &assembly->tables [MONO_TABLE_MEMBERREF];

	if (assembly->save) {
		alloc_table (table, table->rows + 1);
		values = table->values + table->next_idx * MONO_MEMBERREF_SIZE;
		values [MONO_MEMBERREF_CLASS] = pclass | (parent << MONO_MEMBERREF_PARENT_BITS);
		values [MONO_MEMBERREF_NAME] = string_heap_insert (&assembly->sheap, name);
		values [MONO_MEMBERREF_SIGNATURE] = sig;
	}

	token = MONO_TOKEN_MEMBER_REF | table->next_idx;
	table->next_idx ++;
	mono_g_hash_table_insert (assembly->handleref_managed, fb, GUINT_TO_POINTER(token));
	g_free (name);
	return token;
}