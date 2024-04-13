Variant HHVM_FUNCTION(mcrypt_module_open, const String& algorithm,
                                          const String& algorithm_directory,
                             const String& mode, const String& mode_directory) {
  MCRYPT td = mcrypt_module_open
    ((char*)algorithm.data(),
     (char*)(algorithm_directory.empty() ? MCG(algorithms_dir).data() :
             algorithm_directory.data()),
     (char*)mode.data(),
     (char*)(mode_directory.empty() ? (char*)MCG(modes_dir).data() :
             mode_directory.data()));

  if (td == MCRYPT_FAILED) {
    raise_warning("Could not open encryption module");
    return false;
  }

  return Resource(new MCrypt(td));
}