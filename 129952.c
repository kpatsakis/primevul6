MonoReflectionEvent *
mono_reflection_event_builder_get_event_info (MonoReflectionTypeBuilder *tb, MonoReflectionEventBuilder *eb)
{
	MonoEvent *event = g_new0 (MonoEvent, 1);
	MonoClass *klass;

	klass = mono_class_from_mono_type (mono_reflection_type_get_handle ((MonoReflectionType*)tb));

	event->parent = klass;
	event->attrs = eb->attrs;
	event->name = mono_string_to_utf8 (eb->name);
	if (eb->add_method)
		event->add = eb->add_method->mhandle;
	if (eb->remove_method)
		event->remove = eb->remove_method->mhandle;
	if (eb->raise_method)
		event->raise = eb->raise_method->mhandle;

#ifndef MONO_SMALL_CONFIG
	if (eb->other_methods) {
		int j;
		event->other = g_new0 (MonoMethod*, mono_array_length (eb->other_methods) + 1);
		for (j = 0; j < mono_array_length (eb->other_methods); ++j) {
			MonoReflectionMethodBuilder *mb = 
				mono_array_get (eb->other_methods,
						MonoReflectionMethodBuilder*, j);
			event->other [j] = mb->mhandle;
		}
	}
#endif

	return mono_event_get_object (mono_object_domain (tb), klass, event);