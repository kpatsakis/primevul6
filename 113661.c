  virtual void moduleInit() {
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_3DES.get(), StaticString("tripledes").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_ARCFOUR.get(), StaticString("arcfour").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_ARCFOUR_IV.get(), StaticString("arcfour-iv").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_BLOWFISH.get(), StaticString("blowfish").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_BLOWFISH_COMPAT.get(), StaticString("blowfish-compat").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_CAST_128.get(), StaticString("cast-128").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_CAST_256.get(), StaticString("cast-256").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_CRYPT.get(), StaticString("crypt").get()
    );
    Native::registerConstant<KindOfInt64>(
      s_MCRYPT_DECRYPT.get(), 1
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_DES.get(), StaticString("des").get()
    );
    Native::registerConstant<KindOfInt64>(
      s_MCRYPT_DEV_RANDOM.get(), RANDOM
    );
    Native::registerConstant<KindOfInt64>(
      s_MCRYPT_DEV_URANDOM.get(), URANDOM
    );
    Native::registerConstant<KindOfInt64>(
      s_MCRYPT_ENCRYPT.get(), 0
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_ENIGNA.get(), StaticString("crypt").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_GOST.get(), StaticString("gost").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_IDEA.get(), StaticString("idea").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_LOKI97.get(), StaticString("loki97").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_MARS.get(), StaticString("mars").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_MODE_CBC.get(), StaticString("cbc").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_MODE_CFB.get(), StaticString("cfb").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_MODE_ECB.get(), StaticString("ecb").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_MODE_NOFB.get(), StaticString("nofb").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_MODE_OFB.get(), StaticString("ofb").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_MODE_STREAM.get(), StaticString("stream").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_PANAMA.get(), StaticString("panama").get()
    );
    Native::registerConstant<KindOfInt64>(
      s_MCRYPT_RAND.get(), RAND
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_RC2.get(), StaticString("rc2").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_RC6.get(), StaticString("rc6").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_RIJNDAEL_128.get(), StaticString("rijndael-128").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_RIJNDAEL_192.get(), StaticString("rijndael-192").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_RIJNDAEL_256.get(), StaticString("rijndael-256").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_SAFER128.get(), StaticString("safer-sk128").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_SAFER64.get(), StaticString("safer-sk64").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_SAFERPLUS.get(), StaticString("saferplus").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_SERPENT.get(), StaticString("serpent").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_SKIPJACK.get(), StaticString("skipjack").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_THREEWAY.get(), StaticString("threeway").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_TRIPLEDES.get(), StaticString("tripledes").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_TWOFISH.get(), StaticString("twofish").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_WAKE.get(), StaticString("wake").get()
    );
    Native::registerConstant<KindOfStaticString>(
      s_MCRYPT_XTEA.get(), StaticString("xtea").get()
    );

    HHVM_FE(mcrypt_module_open);
    HHVM_FE(mcrypt_module_close);
    HHVM_FE(mcrypt_list_algorithms);
    HHVM_FE(mcrypt_list_modes);
    HHVM_FE(mcrypt_module_get_algo_block_size);
    HHVM_FE(mcrypt_module_get_algo_key_size);
    HHVM_FE(mcrypt_module_get_supported_key_sizes);
    HHVM_FE(mcrypt_module_is_block_algorithm_mode);
    HHVM_FE(mcrypt_module_is_block_algorithm);
    HHVM_FE(mcrypt_module_is_block_mode);
    HHVM_FE(mcrypt_module_self_test);
    HHVM_FE(mcrypt_create_iv);
    HHVM_FE(mcrypt_encrypt);
    HHVM_FE(mcrypt_decrypt);
    HHVM_FE(mcrypt_cbc);
    HHVM_FE(mcrypt_cfb);
    HHVM_FE(mcrypt_ecb);
    HHVM_FE(mcrypt_ofb);
    HHVM_FE(mcrypt_get_block_size);
    HHVM_FE(mcrypt_get_cipher_name);
    HHVM_FE(mcrypt_get_iv_size);
    HHVM_FE(mcrypt_get_key_size);
    HHVM_FE(mcrypt_enc_get_algorithms_name);
    HHVM_FE(mcrypt_enc_get_block_size);
    HHVM_FE(mcrypt_enc_get_iv_size);
    HHVM_FE(mcrypt_enc_get_key_size);
    HHVM_FE(mcrypt_enc_get_modes_name);
    HHVM_FE(mcrypt_enc_get_supported_key_sizes);
    HHVM_FE(mcrypt_enc_is_block_algorithm_mode);
    HHVM_FE(mcrypt_enc_is_block_algorithm);
    HHVM_FE(mcrypt_enc_is_block_mode);
    HHVM_FE(mcrypt_enc_self_test);
    HHVM_FE(mcrypt_generic_init);
    HHVM_FE(mcrypt_generic);
    HHVM_FE(mdecrypt_generic);
    HHVM_FE(mcrypt_generic_deinit);
    HHVM_FE(mcrypt_generic_end);

    loadSystemlib();
  }