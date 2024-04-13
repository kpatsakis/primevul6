mono_image_get_type_info (MonoDomain *domain, MonoReflectionTypeBuilder *tb, MonoDynamicImage *assembly)
{
	MonoDynamicTable *table;
	guint *values;
	int i, is_object = 0, is_system = 0;
	char *n;

	table = &assembly->tables [MONO_TABLE_TYPEDEF];
	values = table->values + tb->table_idx * MONO_TYPEDEF_SIZE;
	values [MONO_TYPEDEF_FLAGS] = tb->attrs;
	n = mono_string_to_utf8 (tb->name);
	if (strcmp (n, "Object") == 0)
		is_object++;
	values [MONO_TYPEDEF_NAME] = string_heap_insert (&assembly->sheap, n);
	g_free (n);
	n = mono_string_to_utf8 (tb->nspace);
	if (strcmp (n, "System") == 0)
		is_system++;
	values [MONO_TYPEDEF_NAMESPACE] = string_heap_insert (&assembly->sheap, n);
	g_free (n);
	if (tb->parent && !(is_system && is_object) && 
			!(tb->attrs & TYPE_ATTRIBUTE_INTERFACE)) { /* interfaces don't have a parent */
		values [MONO_TYPEDEF_EXTENDS] = mono_image_typedef_or_ref (assembly, mono_reflection_type_get_handle ((MonoReflectionType*)tb->parent));
	} else {
		values [MONO_TYPEDEF_EXTENDS] = 0;
	}
	values [MONO_TYPEDEF_FIELD_LIST] = assembly->tables [MONO_TABLE_FIELD].next_idx;
	values [MONO_TYPEDEF_METHOD_LIST] = assembly->tables [MONO_TABLE_METHOD].next_idx;

	/*
	 * if we have explicitlayout or sequentiallayouts, output data in the
	 * ClassLayout table.
	 */
	if (((tb->attrs & TYPE_ATTRIBUTE_LAYOUT_MASK) != TYPE_ATTRIBUTE_AUTO_LAYOUT) &&
			((tb->class_size > 0) || (tb->packing_size > 0))) {
		table = &assembly->tables [MONO_TABLE_CLASSLAYOUT];
		table->rows++;
		alloc_table (table, table->rows);
		values = table->values + table->rows * MONO_CLASS_LAYOUT_SIZE;
		values [MONO_CLASS_LAYOUT_PARENT] = tb->table_idx;
		values [MONO_CLASS_LAYOUT_CLASS_SIZE] = tb->class_size;
		values [MONO_CLASS_LAYOUT_PACKING_SIZE] = tb->packing_size;
	}

	/* handle interfaces */
	if (tb->interfaces) {
		table = &assembly->tables [MONO_TABLE_INTERFACEIMPL];
		i = table->rows;
		table->rows += mono_array_length (tb->interfaces);
		alloc_table (table, table->rows);
		values = table->values + (i + 1) * MONO_INTERFACEIMPL_SIZE;
		for (i = 0; i < mono_array_length (tb->interfaces); ++i) {
			MonoReflectionType* iface = (MonoReflectionType*) mono_array_get (tb->interfaces, gpointer, i);
			values [MONO_INTERFACEIMPL_CLASS] = tb->table_idx;
			values [MONO_INTERFACEIMPL_INTERFACE] = mono_image_typedef_or_ref (assembly, mono_reflection_type_get_handle (iface));
			values += MONO_INTERFACEIMPL_SIZE;
		}
	}

	/* handle fields */
	if (tb->fields) {
		table = &assembly->tables [MONO_TABLE_FIELD];
		table->rows += tb->num_fields;
		alloc_table (table, table->rows);
		for (i = 0; i < tb->num_fields; ++i)
			mono_image_get_field_info (
				mono_array_get (tb->fields, MonoReflectionFieldBuilder*, i), assembly);
	}

	/* handle constructors */
	if (tb->ctors) {
		table = &assembly->tables [MONO_TABLE_METHOD];
		table->rows += mono_array_length (tb->ctors);
		alloc_table (table, table->rows);
		for (i = 0; i < mono_array_length (tb->ctors); ++i)
			mono_image_get_ctor_info (domain,
				mono_array_get (tb->ctors, MonoReflectionCtorBuilder*, i), assembly);
	}

	/* handle methods */
	if (tb->methods) {
		table = &assembly->tables [MONO_TABLE_METHOD];
		table->rows += tb->num_methods;
		alloc_table (table, table->rows);
		for (i = 0; i < tb->num_methods; ++i)
			mono_image_get_method_info (
				mono_array_get (tb->methods, MonoReflectionMethodBuilder*, i), assembly);
	}

	/* Do the same with properties etc.. */
	if (tb->events && mono_array_length (tb->events)) {
		table = &assembly->tables [MONO_TABLE_EVENT];
		table->rows += mono_array_length (tb->events);
		alloc_table (table, table->rows);
		table = &assembly->tables [MONO_TABLE_EVENTMAP];
		table->rows ++;
		alloc_table (table, table->rows);
		values = table->values + table->rows * MONO_EVENT_MAP_SIZE;
		values [MONO_EVENT_MAP_PARENT] = tb->table_idx;
		values [MONO_EVENT_MAP_EVENTLIST] = assembly->tables [MONO_TABLE_EVENT].next_idx;
		for (i = 0; i < mono_array_length (tb->events); ++i)
			mono_image_get_event_info (
				mono_array_get (tb->events, MonoReflectionEventBuilder*, i), assembly);
	}
	if (tb->properties && mono_array_length (tb->properties)) {
		table = &assembly->tables [MONO_TABLE_PROPERTY];
		table->rows += mono_array_length (tb->properties);
		alloc_table (table, table->rows);
		table = &assembly->tables [MONO_TABLE_PROPERTYMAP];
		table->rows ++;
		alloc_table (table, table->rows);
		values = table->values + table->rows * MONO_PROPERTY_MAP_SIZE;
		values [MONO_PROPERTY_MAP_PARENT] = tb->table_idx;
		values [MONO_PROPERTY_MAP_PROPERTY_LIST] = assembly->tables [MONO_TABLE_PROPERTY].next_idx;
		for (i = 0; i < mono_array_length (tb->properties); ++i)
			mono_image_get_property_info (
				mono_array_get (tb->properties, MonoReflectionPropertyBuilder*, i), assembly);
	}

	/* handle generic parameters */
	if (tb->generic_params) {
		table = &assembly->tables [MONO_TABLE_GENERICPARAM];
		table->rows += mono_array_length (tb->generic_params);
		alloc_table (table, table->rows);
		for (i = 0; i < mono_array_length (tb->generic_params); ++i) {
			guint32 owner = MONO_TYPEORMETHOD_TYPE | (tb->table_idx << MONO_TYPEORMETHOD_BITS);

			mono_image_get_generic_param_info (
				mono_array_get (tb->generic_params, MonoReflectionGenericParam*, i), owner, assembly);
		}
	}

	mono_image_add_decl_security (assembly, 
		mono_metadata_make_token (MONO_TABLE_TYPEDEF, tb->table_idx), tb->permissions);

	if (tb->subtypes) {
		MonoDynamicTable *ntable;
		
		ntable = &assembly->tables [MONO_TABLE_NESTEDCLASS];
		ntable->rows += mono_array_length (tb->subtypes);
		alloc_table (ntable, ntable->rows);
		values = ntable->values + ntable->next_idx * MONO_NESTED_CLASS_SIZE;

		for (i = 0; i < mono_array_length (tb->subtypes); ++i) {
			MonoReflectionTypeBuilder *subtype = mono_array_get (tb->subtypes, MonoReflectionTypeBuilder*, i);

			values [MONO_NESTED_CLASS_NESTED] = subtype->table_idx;
			values [MONO_NESTED_CLASS_ENCLOSING] = tb->table_idx;
			/*g_print ("nesting %s (%d) in %s (%d) (rows %d/%d)\n",
				mono_string_to_utf8 (subtype->name), subtype->table_idx,
				mono_string_to_utf8 (tb->name), tb->table_idx,
				ntable->next_idx, ntable->rows);*/
			values += MONO_NESTED_CLASS_SIZE;
			ntable->next_idx++;
		}
	}
}