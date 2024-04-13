init_verifier_stats (void)
{
	static gboolean inited;
	if (!inited) {
		inited = TRUE;
		mono_counters_register ("Maximum memory allocated during verification", MONO_COUNTER_METADATA | MONO_COUNTER_INT, &max_allocated_memory);
		mono_counters_register ("Maximum memory used during verification", MONO_COUNTER_METADATA | MONO_COUNTER_INT, &max_working_set);
		mono_counters_register ("Total memory allocated for verification", MONO_COUNTER_METADATA | MONO_COUNTER_INT, &total_allocated_memory);
	}
}