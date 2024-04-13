ciInstanceKlass* ciEnv::get_instance_klass_for_declared_method_holder(ciKlass* method_holder) {
  // For the case of <array>.clone(), the method holder can be a ciArrayKlass
  // instead of a ciInstanceKlass.  For that case simply pretend that the
  // declared holder is Object.clone since that's where the call will bottom out.
  // A more correct fix would trickle out through many interfaces in CI,
  // requiring ciInstanceKlass* to become ciKlass* and many more places would
  // require checks to make sure the expected type was found.  Given that this
  // only occurs for clone() the more extensive fix seems like overkill so
  // instead we simply smear the array type into Object.
  guarantee(method_holder != NULL, "no method holder");
  if (method_holder->is_instance_klass()) {
    return method_holder->as_instance_klass();
  } else if (method_holder->is_array_klass()) {
    return current()->Object_klass();
  } else {
    ShouldNotReachHere();
  }
  return NULL;
}