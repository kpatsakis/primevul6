  void moduleInit() override {
    HHVM_FE(exif_imagetype);
    HHVM_FE(exif_read_data);
    HHVM_FE(exif_tagname);
    HHVM_FE(exif_thumbnail);

    HHVM_RC_INT(EXIF_USE_MBSTRING, 0);

    loadSystemlib();
  }