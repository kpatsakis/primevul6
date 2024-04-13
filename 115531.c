auto make_scope_exit(EF &&exit_function) -> scope_exit<EF> {
  return scope_exit<typename std::remove_reference<EF>::type>(
      std::forward<EF>(exit_function));
}