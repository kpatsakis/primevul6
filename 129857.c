clear_cached_object (MonoDomain *domain, gpointer o, MonoClass *klass)
{
	mono_domain_lock (domain);
	if (domain->refobject_hash) {
        ReflectedEntry pe;
		gpointer orig_pe, orig_value;

		pe.item = o;
		pe.refclass = klass;
		if (mono_g_hash_table_lookup_extended (domain->refobject_hash, &pe, &orig_pe, &orig_value)) {
			mono_g_hash_table_remove (domain->refobject_hash, &pe);
			FREE_REFENTRY (orig_pe);
		}
	}
	mono_domain_unlock (domain);
}