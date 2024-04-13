mono_reflection_type_from_name (char *name, MonoImage *image)
{
	MonoType *type = NULL;
	MonoTypeNameParse info;
	char *tmp;

	/* Make a copy since parse_type modifies its argument */
	tmp = g_strdup (name);
	
	/*g_print ("requested type %s\n", str);*/
	if (mono_reflection_parse_type (tmp, &info)) {
		type = _mono_reflection_get_type_from_info (&info, image, FALSE);
	}

	g_free (tmp);
	mono_reflection_free_type_info (&info);
	return type;
}