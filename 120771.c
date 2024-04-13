int pkey_gost_encrypt(EVP_PKEY_CTX *pctx, unsigned char *out,
                      size_t *out_len, const unsigned char *key, size_t key_len)
{
  struct gost_pmeth_data *gctx = EVP_PKEY_CTX_get_data(pctx);
  switch (gctx->cipher_nid)
  {
    case NID_id_Gost28147_89:
    case NID_undef: /* FIXME */
      return pkey_GOST_ECcp_encrypt(pctx, out, out_len, key, key_len);
      break;
    case NID_kuznyechik_ctr:
    case NID_magma_ctr:
      return pkey_gost2018_encrypt(pctx, out, out_len, key, key_len);
      break;
    default:
      GOSTerr(GOST_F_PKEY_GOST_ENCRYPT, ERR_R_INTERNAL_ERROR);
      return -1;
  }
}