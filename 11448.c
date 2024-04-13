bool ciEnv::system_dictionary_modification_counter_changed() {
  return _system_dictionary_modification_counter != SystemDictionary::number_of_modifications();
}