int ciEnv::comp_level() {
  if (task() == NULL)  return CompLevel_highest_tier;
  return task()->comp_level();
}