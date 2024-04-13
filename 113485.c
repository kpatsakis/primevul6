static Variant php_mcrypt_do_crypt(const String& cipher, const String& key,
                                   const String& data, const String& mode,
                                   const String& iv, bool dencrypt,
                                   char *name) {
  MCRYPT td = mcrypt_module_open((char*)cipher.data(),
                                 (char*)MCG(algorithms_dir).data(),
                                 (char*)mode.data(),
                                 (char*)MCG(modes_dir).data());
  if (td == MCRYPT_FAILED) {
    MCRYPT_OPEN_MODULE_FAILED(name);
    return false;
  }

  /* Checking for key-length */
  int max_key_length = mcrypt_enc_get_key_size(td);
  if (key.size() > max_key_length) {
    raise_warning("Size of key is too large for this algorithm");
  }
  int count;
  int *key_length_sizes = mcrypt_enc_get_supported_key_sizes(td, &count);
  int use_key_length;
  char *key_s = nullptr;
  if (count == 0 && key_length_sizes == nullptr) { // all lengths 1 - k_l_s = OK
    use_key_length = key.size();
    key_s = (char*)malloc(use_key_length);
    memcpy(key_s, key.data(), use_key_length);
  } else if (count == 1) {  /* only m_k_l = OK */
    key_s = (char*)malloc(key_length_sizes[0]);
    memset(key_s, 0, key_length_sizes[0]);
    memcpy(key_s, key.data(), MIN(key.size(), key_length_sizes[0]));
    use_key_length = key_length_sizes[0];
  } else { /* dertermine smallest supported key > length of requested key */
    use_key_length = max_key_length; /* start with max key length */
    for (int i = 0; i < count; i++) {
      if (key_length_sizes[i] >= key.size() &&
          key_length_sizes[i] < use_key_length) {
        use_key_length = key_length_sizes[i];
      }
    }
    key_s = (char*)malloc(use_key_length);
    memset(key_s, 0, use_key_length);
    memcpy(key_s, key.data(), MIN(key.size(), use_key_length));
  }
  mcrypt_free(key_length_sizes);

  /* Check IV */
  char *iv_s = nullptr;
  int iv_size = mcrypt_enc_get_iv_size(td);

  /* IV is required */
  if (mcrypt_enc_mode_has_iv(td) == 1) {
    if (!iv.empty()) {
      if (iv_size != iv.size()) {
        raise_warning("%s(): The IV parameter must be as long as "
                      "the blocksize", name);
      } else {
        iv_s = (char*)malloc(iv_size + 1);
        memcpy(iv_s, iv.data(), iv_size);
      }
    } else {
      raise_warning("%s(): The IV parameter must be as long as "
                    "the blocksize", name);
      iv_s = (char*)malloc(iv_size + 1);
      memset(iv_s, 0, iv_size + 1);
    }
  }

  int block_size;
  unsigned long int data_size;
  String s;
  char *data_s;
  /* Check blocksize */
  if (mcrypt_enc_is_block_mode(td) == 1) { /* It's a block algorithm */
    block_size = mcrypt_enc_get_block_size(td);
    data_size = (((data.size() - 1) / block_size) + 1) * block_size;
    s = String(data_size, ReserveString);
    data_s = (char*)s.mutableData();
    memset(data_s, 0, data_size);
    memcpy(data_s, data.data(), data.size());
  } else { /* It's not a block algorithm */
    data_size = data.size();
    s = String(data_size, ReserveString);
    data_s = (char*)s.mutableData();
    memcpy(data_s, data.data(), data.size());
  }

  if (mcrypt_generic_init(td, key_s, use_key_length, iv_s) < 0) {
    raise_warning("Mcrypt initialisation failed");
    return false;
  }
  if (dencrypt) {
    mdecrypt_generic(td, data_s, data_size);
  } else {
    mcrypt_generic(td, data_s, data_size);
  }

  /* freeing vars */
  mcrypt_generic_end(td);
  if (key_s != nullptr) {
    free(key_s);
  }
  if (iv_s != nullptr) {
    free(iv_s);
  }
  s.setSize(data_size);
  return s;
}