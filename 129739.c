add_exported_type (MonoReflectionAssemblyBuilder *assemblyb, MonoDynamicImage *assembly, MonoClass *klass, guint32 parent_index)
{
	MonoDynamicTable *table;
	guint32 *values;
	guint32 scope, scope_idx, impl, current_idx;
	gboolean forwarder = TRUE;
	gpointer iter = NULL;
	MonoClass *nested;

	if (klass->nested_in) {
		impl = (parent_index << MONO_IMPLEMENTATION_BITS) + MONO_IMPLEMENTATION_EXP_TYPE;
		forwarder = FALSE;
	} else {
		scope = resolution_scope_from_image (assembly, klass->image);
		g_assert ((scope & MONO_RESOLTION_SCOPE_MASK) == MONO_RESOLTION_SCOPE_ASSEMBLYREF);
		scope_idx = scope >> MONO_RESOLTION_SCOPE_BITS;
		impl = (scope_idx << MONO_IMPLEMENTATION_BITS) + MONO_IMPLEMENTATION_ASSEMBLYREF;
	}

	table = &assembly->tables [MONO_TABLE_EXPORTEDTYPE];

	table->rows++;
	alloc_table (table, table->rows);
	current_idx = table->next_idx;
	values = table->values + current_idx * MONO_EXP_TYPE_SIZE;

	values [MONO_EXP_TYPE_FLAGS] = forwarder ? TYPE_ATTRIBUTE_FORWARDER : 0;
	values [MONO_EXP_TYPE_TYPEDEF] = 0;
	values [MONO_EXP_TYPE_IMPLEMENTATION] = impl;
	values [MONO_EXP_TYPE_NAME] = string_heap_insert (&assembly->sheap, klass->name);
	values [MONO_EXP_TYPE_NAMESPACE] = string_heap_insert (&assembly->sheap, klass->name_space);

	table->next_idx++;

	while ((nested = mono_class_get_nested_types (klass, &iter)))
		add_exported_type (assemblyb, assembly, nested, current_idx);
}