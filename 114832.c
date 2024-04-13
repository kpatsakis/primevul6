void ndpi_md5(const u_char *data, size_t data_len, u_char hash[16]) {
  ndpi_MD5_CTX ctx;

  ndpi_MD5Init(&ctx);
  ndpi_MD5Update(&ctx, data, data_len);
  ndpi_MD5Final(hash, &ctx);
}