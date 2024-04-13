uint ciEnv::compile_id() {
  if (task() == NULL)  return 0;
  return task()->compile_id();
}