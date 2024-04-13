static unsigned writeSignature(ucvector* out)
{
  /*8 bytes PNG signature, aka the magic bytes*/
  if (!ucvector_push_back(out, 137)) return 83;
  if (!ucvector_push_back(out, 80)) return 83;
  if (!ucvector_push_back(out, 78)) return 83;
  if (!ucvector_push_back(out, 71)) return 83;
  if (!ucvector_push_back(out, 13)) return 83;
  if (!ucvector_push_back(out, 10)) return 83;
  if (!ucvector_push_back(out, 26)) return 83;
  if (!ucvector_push_back(out, 10)) return 83;

  return 0;
}