char *ciEnv::name_buffer(int req_len) {
  if (_name_buffer_len < req_len) {
    if (_name_buffer == NULL) {
      _name_buffer = (char*)arena()->Amalloc(sizeof(char)*req_len);
      _name_buffer_len = req_len;
    } else {
      _name_buffer =
        (char*)arena()->Arealloc(_name_buffer, _name_buffer_len, req_len);
      _name_buffer_len = req_len;
    }
  }
  return _name_buffer;
}