int TNEFGetKey(TNEFStruct *TNEF, WORD *key) {
  if (TNEF->IO.ReadProc(&(TNEF->IO), sizeof(WORD), 1, key) < 1) {
    if (TNEF->Debug >= 1)
      printf("Error reading Key\n");
    return YTNEF_ERROR_READING_DATA;
  }
  *key = SwapWord((BYTE *)key, sizeof(WORD));

  DEBUG1(TNEF->Debug, 2, "Key = 0x%X", *key);
  DEBUG1(TNEF->Debug, 2, "Key = %i", *key);
  return 0;
}