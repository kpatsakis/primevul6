lconv *localeconv() {
  return LocaleMock::instance ?
        LocaleMock::instance->localeconv() : ::std::localeconv();
}