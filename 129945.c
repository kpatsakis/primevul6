mono_reflection_get_type_internal (MonoImage *rootimage, MonoImage* image, MonoTypeNameParse *info, gboolean ignorecase)
{
	MonoClass *klass;
	GList *mod;
	int modval;
	gboolean bounded = FALSE;
	
	if (!image)
		image = mono_defaults.corlib;

	if (ignorecase)
		klass = mono_class_from_name_case (image, info->name_space, info->name);
	else
		klass = mono_class_from_name (image, info->name_space, info->name);
	if (!klass)
		return NULL;
	for (mod = info->nested; mod; mod = mod->next) {
		gpointer iter = NULL;
		MonoClass *parent;

		parent = klass;
		mono_class_init (parent);

		while ((klass = mono_class_get_nested_types (parent, &iter))) {
			if (ignorecase) {
				if (mono_utf8_strcasecmp (klass->name, mod->data) == 0)
					break;
			} else {
				if (strcmp (klass->name, mod->data) == 0)
					break;
			}
		}
		if (!klass)
			break;
	}
	if (!klass)
		return NULL;

	if (info->type_arguments) {
		MonoType **type_args = g_new0 (MonoType *, info->type_arguments->len);
		MonoReflectionType *the_type;
		MonoType *instance;
		int i;

		for (i = 0; i < info->type_arguments->len; i++) {
			MonoTypeNameParse *subinfo = g_ptr_array_index (info->type_arguments, i);

			type_args [i] = _mono_reflection_get_type_from_info (subinfo, rootimage, ignorecase);
			if (!type_args [i]) {
				g_free (type_args);
				return NULL;
			}
		}

		the_type = mono_type_get_object (mono_domain_get (), &klass->byval_arg);

		instance = mono_reflection_bind_generic_parameters (
			the_type, info->type_arguments->len, type_args);

		g_free (type_args);
		if (!instance)
			return NULL;

		klass = mono_class_from_mono_type (instance);
	}

	for (mod = info->modifiers; mod; mod = mod->next) {
		modval = GPOINTER_TO_UINT (mod->data);
		if (!modval) { /* byref: must be last modifier */
			return &klass->this_arg;
		} else if (modval == -1) {
			klass = mono_ptr_class_get (&klass->byval_arg);
		} else if (modval == -2) {
			bounded = TRUE;
		} else { /* array rank */
			klass = mono_bounded_array_class_get (klass, modval, bounded);
		}
	}

	return &klass->byval_arg;
}