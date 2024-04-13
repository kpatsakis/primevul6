ciKlass* ciEnv::get_klass_by_name_impl(ciKlass* accessing_klass,
                                       constantPoolHandle cpool,
                                       ciSymbol* name,
                                       bool require_local) {
  ASSERT_IN_VM;
  EXCEPTION_CONTEXT;

  // Now we need to check the SystemDictionary
  Symbol* sym = name->get_symbol();
  if (sym->byte_at(0) == 'L' &&
    sym->byte_at(sym->utf8_length()-1) == ';') {
    // This is a name from a signature.  Strip off the trimmings.
    // Call recursive to keep scope of strippedsym.
    TempNewSymbol strippedsym = SymbolTable::new_symbol(sym->as_utf8()+1,
                    sym->utf8_length()-2,
                    KILL_COMPILE_ON_FATAL_(_unloaded_ciinstance_klass));
    ciSymbol* strippedname = get_symbol(strippedsym);
    return get_klass_by_name_impl(accessing_klass, cpool, strippedname, require_local);
  }

  // Check for prior unloaded klass.  The SystemDictionary's answers
  // can vary over time but the compiler needs consistency.
  ciKlass* unloaded_klass = check_get_unloaded_klass(accessing_klass, name);
  if (unloaded_klass != NULL) {
    if (require_local)  return NULL;
    return unloaded_klass;
  }

  Handle loader(THREAD, (oop)NULL);
  Handle domain(THREAD, (oop)NULL);
  if (accessing_klass != NULL) {
    loader = Handle(THREAD, accessing_klass->loader());
    domain = Handle(THREAD, accessing_klass->protection_domain());
  }

  // setup up the proper type to return on OOM
  ciKlass* fail_type;
  if (sym->byte_at(0) == '[') {
    fail_type = _unloaded_ciobjarrayklass;
  } else {
    fail_type = _unloaded_ciinstance_klass;
  }
  KlassHandle found_klass;
  {
    ttyUnlocker ttyul;  // release tty lock to avoid ordering problems
    MutexLocker ml(Compile_lock);
    Klass* kls;
    if (!require_local) {
      kls = SystemDictionary::find_constrained_instance_or_array_klass(sym, loader,
                                                                       KILL_COMPILE_ON_FATAL_(fail_type));
    } else {
      kls = SystemDictionary::find_instance_or_array_klass(sym, loader, domain,
                                                           KILL_COMPILE_ON_FATAL_(fail_type));
    }
    found_klass = KlassHandle(THREAD, kls);
  }

  // If we fail to find an array klass, look again for its element type.
  // The element type may be available either locally or via constraints.
  // In either case, if we can find the element type in the system dictionary,
  // we must build an array type around it.  The CI requires array klasses
  // to be loaded if their element klasses are loaded, except when memory
  // is exhausted.
  if (sym->byte_at(0) == '[' &&
      (sym->byte_at(1) == '[' || sym->byte_at(1) == 'L')) {
    // We have an unloaded array.
    // Build it on the fly if the element class exists.
    TempNewSymbol elem_sym = SymbolTable::new_symbol(sym->as_utf8()+1,
                                                 sym->utf8_length()-1,
                                                 KILL_COMPILE_ON_FATAL_(fail_type));

    // Get element ciKlass recursively.
    ciKlass* elem_klass =
      get_klass_by_name_impl(accessing_klass,
                             cpool,
                             get_symbol(elem_sym),
                             require_local);
    if (elem_klass != NULL && elem_klass->is_loaded()) {
      // Now make an array for it
      return ciObjArrayKlass::make_impl(elem_klass);
    }
  }

  if (found_klass() == NULL && !cpool.is_null() && cpool->has_preresolution()) {
    // Look inside the constant pool for pre-resolved class entries.
    for (int i = cpool->length() - 1; i >= 1; i--) {
      if (cpool->tag_at(i).is_klass()) {
        Klass* kls = cpool->resolved_klass_at(i);
        if (kls->name() == sym) {
          found_klass = KlassHandle(THREAD, kls);
          break;
        }
      }
    }
  }

  if (found_klass() != NULL) {
    // Found it.  Build a CI handle.
    return get_klass(found_klass());
  }

  if (require_local)  return NULL;

  // Not yet loaded into the VM, or not governed by loader constraints.
  // Make a CI representative for it.
  return get_unloaded_klass(accessing_klass, name);
}