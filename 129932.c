find_event_index (MonoClass *klass, MonoEvent *event) {
	int i;

	for (i = 0; i < klass->ext->event.count; ++i) {
		if (event == &klass->ext->events [i])
			return klass->ext->event.first + 1 + i;
	}
	return 0;
}