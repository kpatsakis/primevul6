static Variant mcrypt_generic(const Resource& td, const String& data,
                              bool dencrypt) {
  MCrypt *pm = td.getTyped<MCrypt>();
  if (!pm->m_init) {
    raise_warning("Operation disallowed prior to mcrypt_generic_init().");
    return false;
  }

  if (data.empty()) {
    raise_warning("An empty string was passed");
    return false;
  }

  String s;
  unsigned char* data_s;
  int block_size, data_size;
  /* Check blocksize */
  if (mcrypt_enc_is_block_mode(pm->m_td) == 1) { /* It's a block algorithm */
    block_size = mcrypt_enc_get_block_size(pm->m_td);
    data_size = (((data.size() - 1) / block_size) + 1) * block_size;
    s = String(data_size, ReserveString);
    data_s = (unsigned char *)s.bufferSlice().ptr;
    memset(data_s, 0, data_size);
    memcpy(data_s, data.data(), data.size());
  } else { /* It's not a block algorithm */
    data_size = data.size();
    s = String(data_size, ReserveString);
    data_s = (unsigned char *)s.bufferSlice().ptr;
    memcpy(data_s, data.data(), data.size());
  }

  if (dencrypt) {
    mdecrypt_generic(pm->m_td, data_s, data_size);
  } else {
    mcrypt_generic(pm->m_td, data_s, data_size);
  }
  s.setSize(data_size);
  return s;
}