int TNEFRawRead(TNEFStruct *TNEF, BYTE *data, DWORD size, WORD *checksum) {
  WORD temp;
  int i;

  if (TNEF->IO.ReadProc(&TNEF->IO, sizeof(BYTE), size, data) < size) {
    if (TNEF->Debug >= 1)
      printf("ERROR: Error reading data\n");
    return YTNEF_ERROR_READING_DATA;
  }


  if (checksum != NULL) {
    *checksum = 0;
    for (i = 0; i < size; i++) {
      temp = data[i];
      *checksum = (*checksum + temp);
    }
  }
  return 0;
}