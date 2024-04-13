get_default_param_value_blobs (MonoMethod *method, char **blobs, guint32 *types)
{
	guint32 param_index, i, lastp, crow = 0;
	guint32 param_cols [MONO_PARAM_SIZE], const_cols [MONO_CONSTANT_SIZE];
	gint32 idx;

	MonoClass *klass = method->klass;
	MonoImage *image = klass->image;
	MonoMethodSignature *methodsig = mono_method_signature (method);

	MonoTableInfo *constt;
	MonoTableInfo *methodt;
	MonoTableInfo *paramt;

	if (!methodsig->param_count)
		return;

	mono_class_init (klass);

	if (klass->image->dynamic) {
		MonoReflectionMethodAux *aux;
		if (method->is_inflated)
			method = ((MonoMethodInflated*)method)->declaring;
		aux = g_hash_table_lookup (((MonoDynamicImage*)method->klass->image)->method_aux_hash, method);
		if (aux && aux->param_defaults) {
			memcpy (blobs, &(aux->param_defaults [1]), methodsig->param_count * sizeof (char*));
			memcpy (types, &(aux->param_default_types [1]), methodsig->param_count * sizeof (guint32));
		}
		return;
	}

	methodt = &klass->image->tables [MONO_TABLE_METHOD];
	paramt = &klass->image->tables [MONO_TABLE_PARAM];
	constt = &image->tables [MONO_TABLE_CONSTANT];

	idx = mono_method_get_index (method) - 1;
	g_assert (idx != -1);

	param_index = mono_metadata_decode_row_col (methodt, idx, MONO_METHOD_PARAMLIST);
	if (idx + 1 < methodt->rows)
		lastp = mono_metadata_decode_row_col (methodt, idx + 1, MONO_METHOD_PARAMLIST);
	else
		lastp = paramt->rows + 1;

	for (i = param_index; i < lastp; ++i) {
		guint32 paramseq;

		mono_metadata_decode_row (paramt, i - 1, param_cols, MONO_PARAM_SIZE);
		paramseq = param_cols [MONO_PARAM_SEQUENCE];

		if (!(param_cols [MONO_PARAM_FLAGS] & PARAM_ATTRIBUTE_HAS_DEFAULT))
			continue;

		crow = mono_metadata_get_constant_index (image, MONO_TOKEN_PARAM_DEF | i, crow + 1);
		if (!crow) {
			continue;
		}
	
		mono_metadata_decode_row (constt, crow - 1, const_cols, MONO_CONSTANT_SIZE);
		blobs [paramseq - 1] = (gpointer) mono_metadata_blob_heap (image, const_cols [MONO_CONSTANT_VALUE]);
		types [paramseq - 1] = const_cols [MONO_CONSTANT_TYPE];
	}

	return;
}