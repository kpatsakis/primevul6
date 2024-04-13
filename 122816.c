int TNEFGetHeader(TNEFStruct *TNEF, DWORD *type, DWORD *size) {
  BYTE component;

  DEBUG(TNEF->Debug, 2, "About to read Component");
  if (TNEF->IO.ReadProc(&(TNEF->IO), sizeof(BYTE), 1, &component) < 1) {
    return YTNEF_ERROR_READING_DATA;
  }


  DEBUG(TNEF->Debug, 2, "About to read type");
  if (TNEF->IO.ReadProc(&(TNEF->IO), sizeof(DWORD), 1, type)  < 1) {
    if (TNEF->Debug >= 1)
      printf("ERROR: Error reading type\n");
    return YTNEF_ERROR_READING_DATA;
  }
  DEBUG1(TNEF->Debug, 2, "Type = 0x%X", *type);
  DEBUG1(TNEF->Debug, 2, "Type = %u", *type);


  DEBUG(TNEF->Debug, 2, "About to read size");
  if (TNEF->IO.ReadProc(&(TNEF->IO), sizeof(DWORD), 1, size) < 1) {
    if (TNEF->Debug >= 1)
      printf("ERROR: Error reading size\n");
    return YTNEF_ERROR_READING_DATA;
  }


  DEBUG1(TNEF->Debug, 2, "Size = %u", *size);

  *type = SwapDWord((BYTE *)type, sizeof(DWORD));
  *size = SwapDWord((BYTE *)size, sizeof(DWORD));

  return 0;
}