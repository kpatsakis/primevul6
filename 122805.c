int TNEFCheckForSignature(DWORD sig) {
  DWORD signature = 0x223E9F78;

  sig = SwapDWord((BYTE *)&sig, sizeof(DWORD));

  if (signature == sig) {
    return 0;
  } else {
    return YTNEF_NOT_TNEF_STREAM;
  }
}