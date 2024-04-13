mono_custom_attrs_from_param (MonoMethod *method, guint32 param)
{
	MonoTableInfo *ca;
	guint32 i, idx, method_index;
	guint32 param_list, param_last, param_pos, found;
	MonoImage *image;
	MonoReflectionMethodAux *aux;

	/*
	 * An instantiated method has the same cattrs as the generic method definition.
	 *
	 * LAMESPEC: The .NET SRE throws an exception for instantiations of generic method builders
	 *           Note that this stanza is not necessary for non-SRE types, but it's a micro-optimization
	 */
	if (method->is_inflated)
		method = ((MonoMethodInflated *) method)->declaring;

	if (method->klass->image->dynamic) {
		MonoCustomAttrInfo *res, *ainfo;
		int size;

		aux = g_hash_table_lookup (((MonoDynamicImage*)method->klass->image)->method_aux_hash, method);
		if (!aux || !aux->param_cattr)
			return NULL;

		/* Need to copy since it will be freed later */
		ainfo = aux->param_cattr [param];
		if (!ainfo)
			return NULL;
		size = MONO_SIZEOF_CUSTOM_ATTR_INFO + sizeof (MonoCustomAttrEntry) * ainfo->num_attrs;
		res = g_malloc0 (size);
		memcpy (res, ainfo, size);
		return res;
	}

	image = method->klass->image;
	method_index = mono_method_get_index (method);
	if (!method_index)
		return NULL;
	ca = &image->tables [MONO_TABLE_METHOD];

	param_list = mono_metadata_decode_row_col (ca, method_index - 1, MONO_METHOD_PARAMLIST);
	if (method_index == ca->rows) {
		ca = &image->tables [MONO_TABLE_PARAM];
		param_last = ca->rows + 1;
	} else {
		param_last = mono_metadata_decode_row_col (ca, method_index, MONO_METHOD_PARAMLIST);
		ca = &image->tables [MONO_TABLE_PARAM];
	}
	found = FALSE;
	for (i = param_list; i < param_last; ++i) {
		param_pos = mono_metadata_decode_row_col (ca, i - 1, MONO_PARAM_SEQUENCE);
		if (param_pos == param) {
			found = TRUE;
			break;
		}
	}
	if (!found)
		return NULL;
	idx = i;
	idx <<= MONO_CUSTOM_ATTR_BITS;
	idx |= MONO_CUSTOM_ATTR_PARAMDEF;
	return mono_custom_attrs_from_index (image, idx);
}