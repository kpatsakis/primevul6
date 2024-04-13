  HashEngineMapInitializer() {
    HashEngines["md2"]        = HashEnginePtr(new hash_md2());
    HashEngines["md4"]        = HashEnginePtr(new hash_md4());
    HashEngines["md5"]        = HashEnginePtr(new hash_md5());
    HashEngines["sha1"]       = HashEnginePtr(new hash_sha1());
    HashEngines["sha224"]     = HashEnginePtr(new hash_sha224());
    HashEngines["sha256"]     = HashEnginePtr(new hash_sha256());
    HashEngines["sha384"]     = HashEnginePtr(new hash_sha384());
    HashEngines["sha512"]     = HashEnginePtr(new hash_sha512());
    HashEngines["ripemd128"]  = HashEnginePtr(new hash_ripemd128());
    HashEngines["ripemd160"]  = HashEnginePtr(new hash_ripemd160());
    HashEngines["ripemd256"]  = HashEnginePtr(new hash_ripemd256());
    HashEngines["ripemd320"]  = HashEnginePtr(new hash_ripemd320());
    HashEngines["whirlpool"]  = HashEnginePtr(new hash_whirlpool());
#ifdef FACEBOOK
    // The original version of tiger got the endianness backwards
    // This fb-specific version remains for backward compatibility
    HashEngines["tiger128,3-fb"]
                              = HashEnginePtr(new hash_tiger(true, 128, true));
#endif
    HashEngines["tiger128,3"] = HashEnginePtr(new hash_tiger(true, 128));
    HashEngines["tiger160,3"] = HashEnginePtr(new hash_tiger(true, 160));
    HashEngines["tiger192,3"] = HashEnginePtr(new hash_tiger(true, 192));
    HashEngines["tiger128,4"] = HashEnginePtr(new hash_tiger(false, 128));
    HashEngines["tiger160,4"] = HashEnginePtr(new hash_tiger(false, 160));
    HashEngines["tiger192,4"] = HashEnginePtr(new hash_tiger(false, 192));

    HashEngines["snefru"]     = HashEnginePtr(new hash_snefru());
    HashEngines["gost"]       = HashEnginePtr(new hash_gost());
#ifdef FACEBOOK
    // Temporarily leave adler32 algo inverting its hash output
    // to retain BC pending conversion of user code to correct endianness
    // sgolemon(2014-01-30)
    HashEngines["adler32-fb"] = HashEnginePtr(new hash_adler32(true));
    HashEngines["adler32"]    = HashEnginePtr(new hash_adler32(true));
#else
    HashEngines["adler32"]    = HashEnginePtr(new hash_adler32());
#endif
    HashEngines["crc32"]      = HashEnginePtr(new hash_crc32(false));
    HashEngines["crc32b"]     = HashEnginePtr(new hash_crc32(true));
    HashEngines["haval128,3"] = HashEnginePtr(new hash_haval(3,128));
    HashEngines["haval160,3"] = HashEnginePtr(new hash_haval(3,160));
    HashEngines["haval192,3"] = HashEnginePtr(new hash_haval(3,192));
    HashEngines["haval224,3"] = HashEnginePtr(new hash_haval(3,224));
    HashEngines["haval256,3"] = HashEnginePtr(new hash_haval(3,256));
    HashEngines["haval128,4"] = HashEnginePtr(new hash_haval(4,128));
    HashEngines["haval160,4"] = HashEnginePtr(new hash_haval(4,160));
    HashEngines["haval192,4"] = HashEnginePtr(new hash_haval(4,192));
    HashEngines["haval224,4"] = HashEnginePtr(new hash_haval(4,224));
    HashEngines["haval256,4"] = HashEnginePtr(new hash_haval(4,256));
    HashEngines["haval128,5"] = HashEnginePtr(new hash_haval(5,128));
    HashEngines["haval160,5"] = HashEnginePtr(new hash_haval(5,160));
    HashEngines["haval192,5"] = HashEnginePtr(new hash_haval(5,192));
    HashEngines["haval224,5"] = HashEnginePtr(new hash_haval(5,224));
    HashEngines["haval256,5"] = HashEnginePtr(new hash_haval(5,256));
    HashEngines["fnv132"]     = HashEnginePtr(new hash_fnv132(false));
    HashEngines["fnv1a32"]    = HashEnginePtr(new hash_fnv132(true));
    HashEngines["fnv164"]     = HashEnginePtr(new hash_fnv164(false));
    HashEngines["fnv1a64"]    = HashEnginePtr(new hash_fnv164(true));
  }