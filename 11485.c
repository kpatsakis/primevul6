ciKlass* ciEnv::get_klass_by_index_impl(constantPoolHandle cpool,
                                        int index,
                                        bool& is_accessible,
                                        ciInstanceKlass* accessor) {
  EXCEPTION_CONTEXT;
  KlassHandle klass; // = NULL;
  Symbol* klass_name = NULL;

  if (cpool->tag_at(index).is_symbol()) {
    klass_name = cpool->symbol_at(index);
  } else {
    // Check if it's resolved if it's not a symbol constant pool entry.
    klass = KlassHandle(THREAD, ConstantPool::klass_at_if_loaded(cpool, index));

  if (klass.is_null()) {
    // The klass has not been inserted into the constant pool.
    // Try to look it up by name.
    {
      // We have to lock the cpool to keep the oop from being resolved
      // while we are accessing it.
        MonitorLockerEx ml(cpool->lock());
      constantTag tag = cpool->tag_at(index);
      if (tag.is_klass()) {
        // The klass has been inserted into the constant pool
        // very recently.
        klass = KlassHandle(THREAD, cpool->resolved_klass_at(index));
      } else {
        assert(cpool->tag_at(index).is_unresolved_klass(), "wrong tag");
        klass_name = cpool->unresolved_klass_at(index);
      }
    }
  }
  }

  if (klass.is_null()) {
    // Not found in constant pool.  Use the name to do the lookup.
    ciKlass* k = get_klass_by_name_impl(accessor,
                                        cpool,
                                        get_symbol(klass_name),
                                        false);
    // Calculate accessibility the hard way.
    if (!k->is_loaded()) {
      is_accessible = false;
    } else if (k->loader() != accessor->loader() &&
               get_klass_by_name_impl(accessor, cpool, k->name(), true) == NULL) {
      // Loaded only remotely.  Not linked yet.
      is_accessible = false;
    } else {
      // Linked locally, and we must also check public/private, etc.
      is_accessible = check_klass_accessibility(accessor, k->get_Klass());
    }
    return k;
  }

  // Check for prior unloaded klass.  The SystemDictionary's answers
  // can vary over time but the compiler needs consistency.
  ciSymbol* name = get_symbol(klass()->name());
  ciKlass* unloaded_klass = check_get_unloaded_klass(accessor, name);
  if (unloaded_klass != NULL) {
    is_accessible = false;
    return unloaded_klass;
  }

  // It is known to be accessible, since it was found in the constant pool.
  is_accessible = true;
  return get_klass(klass());
}