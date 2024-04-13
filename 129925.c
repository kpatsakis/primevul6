mono_reflection_get_type_with_rootimage (MonoImage *rootimage, MonoImage* image, MonoTypeNameParse *info, gboolean ignorecase, gboolean *type_resolve)
{
	MonoType *type;
	MonoReflectionAssembly *assembly;
	GString *fullName;
	GList *mod;

	if (image && image->dynamic)
		type = mono_reflection_get_type_internal_dynamic (rootimage, image->assembly, info, ignorecase);
	else
		type = mono_reflection_get_type_internal (rootimage, image, info, ignorecase);
	if (type)
		return type;
	if (!mono_domain_has_type_resolve (mono_domain_get ()))
		return NULL;

	if (type_resolve) {
		if (*type_resolve) 
			return NULL;
		else
			*type_resolve = TRUE;
	}
	
	/* Reconstruct the type name */
	fullName = g_string_new ("");
	if (info->name_space && (info->name_space [0] != '\0'))
		g_string_printf (fullName, "%s.%s", info->name_space, info->name);
	else
		g_string_printf (fullName, "%s", info->name);
	for (mod = info->nested; mod; mod = mod->next)
		g_string_append_printf (fullName, "+%s", (char*)mod->data);

	assembly = mono_domain_try_type_resolve ( mono_domain_get (), fullName->str, NULL);
	if (assembly) {
		if (assembly->assembly->dynamic)
			type = mono_reflection_get_type_internal_dynamic (rootimage, assembly->assembly, info, ignorecase);
		else
			type = mono_reflection_get_type_internal (rootimage, assembly->assembly->image, 
													  info, ignorecase);
	}
	g_string_free (fullName, TRUE);
	return type;
}