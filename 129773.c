mono_image_load_module_dynamic (MonoReflectionAssemblyBuilder *ab, MonoString *fileName)
{
	char *name;
	MonoImage *image;
	MonoImageOpenStatus status;
	MonoDynamicAssembly *assembly;
	guint32 module_count;
	MonoImage **new_modules;
	gboolean *new_modules_loaded;
	
	name = mono_string_to_utf8 (fileName);

	image = mono_image_open (name, &status);
	if (!image) {
		MonoException *exc;
		if (status == MONO_IMAGE_ERROR_ERRNO)
			exc = mono_get_exception_file_not_found (fileName);
		else
			exc = mono_get_exception_bad_image_format (name);
		g_free (name);
		mono_raise_exception (exc);
	}

	g_free (name);

	assembly = ab->dynamic_assembly;
	image->assembly = (MonoAssembly*)assembly;

	module_count = image->assembly->image->module_count;
	new_modules = g_new0 (MonoImage *, module_count + 1);
	new_modules_loaded = g_new0 (gboolean, module_count + 1);

	if (image->assembly->image->modules)
		memcpy (new_modules, image->assembly->image->modules, module_count * sizeof (MonoImage *));
	if (image->assembly->image->modules_loaded)
		memcpy (new_modules_loaded, image->assembly->image->modules_loaded, module_count * sizeof (gboolean));
	new_modules [module_count] = image;
	new_modules_loaded [module_count] = TRUE;
	mono_image_addref (image);

	g_free (image->assembly->image->modules);
	image->assembly->image->modules = new_modules;
	image->assembly->image->modules_loaded = new_modules_loaded;
	image->assembly->image->module_count ++;

	mono_assembly_load_references (image, &status);
	if (status) {
		mono_image_close (image);
		mono_raise_exception (mono_get_exception_file_not_found (fileName));
	}

	return mono_module_get_object (mono_domain_get (), image);
}