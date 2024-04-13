static MonoMarshalSpec*
mono_marshal_spec_from_builder (MonoImage *image, MonoAssembly *assembly,
								MonoReflectionMarshal *minfo)
{
	MonoMarshalSpec *res;

	res = image_g_new0 (image, MonoMarshalSpec, 1);
	res->native = minfo->type;

	switch (minfo->type) {
	case MONO_NATIVE_LPARRAY:
		res->data.array_data.elem_type = minfo->eltype;
		if (minfo->has_size) {
			res->data.array_data.param_num = minfo->param_num;
			res->data.array_data.num_elem = minfo->count;
			res->data.array_data.elem_mult = minfo->param_num == -1 ? 0 : 1;
		}
		else {
			res->data.array_data.param_num = -1;
			res->data.array_data.num_elem = -1;
			res->data.array_data.elem_mult = -1;
		}
		break;

	case MONO_NATIVE_BYVALTSTR:
	case MONO_NATIVE_BYVALARRAY:
		res->data.array_data.num_elem = minfo->count;
		break;

	case MONO_NATIVE_CUSTOM:
		if (minfo->marshaltyperef)
			res->data.custom_data.custom_name =
				type_get_fully_qualified_name (mono_reflection_type_get_handle ((MonoReflectionType*)minfo->marshaltyperef));
		if (minfo->mcookie)
			res->data.custom_data.cookie = mono_string_to_utf8 (minfo->mcookie);
		break;

	default:
		break;
	}

	return res;