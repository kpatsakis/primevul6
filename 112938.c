  void sweep() FOLLY_OVERRIDE {
    /* Just in case the algo has internally allocated resources */
    if (context) {
      assert(ops->digest_size >= 0);
      unsigned char dummy[ops->digest_size];
      ops->hash_final(dummy, context);
      free(context);
    }

    free(key);
  }