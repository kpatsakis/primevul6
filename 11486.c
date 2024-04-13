ciInstance* ciEnv::get_or_create_exception(jobject& handle, Symbol* name) {
  VM_ENTRY_MARK;
  if (handle == NULL) {
    // Cf. universe.cpp, creation of Universe::_null_ptr_exception_instance.
    Klass* k = SystemDictionary::find(name, Handle(), Handle(), THREAD);
    jobject objh = NULL;
    if (!HAS_PENDING_EXCEPTION && k != NULL) {
      oop obj = InstanceKlass::cast(k)->allocate_instance(THREAD);
      if (!HAS_PENDING_EXCEPTION)
        objh = JNIHandles::make_global(obj);
    }
    if (HAS_PENDING_EXCEPTION) {
      CLEAR_PENDING_EXCEPTION;
    } else {
      handle = objh;
    }
  }
  oop obj = JNIHandles::resolve(handle);
  return obj == NULL? NULL: get_object(obj)->as_instance();
}