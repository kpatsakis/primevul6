reflected_equal (gconstpointer a, gconstpointer b) {
	const ReflectedEntry *ea = a;
	const ReflectedEntry *eb = b;

	return (ea->item == eb->item) && (ea->refclass == eb->refclass);
}