reflected_hash (gconstpointer a) {
	const ReflectedEntry *ea = a;
	return mono_aligned_addr_hash (ea->item);
}