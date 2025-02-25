int TNEFParse(TNEFStruct *TNEF) {
  WORD key;
  DWORD type;
  DWORD size;
  DWORD signature;
  BYTE *data;
  WORD checksum, header_checksum;
  int i;

  if (TNEF->IO.ReadProc == NULL) {
    printf("ERROR: Setup incorrectly: No ReadProc\n");
    return YTNEF_INCORRECT_SETUP;
  }

  if (TNEF->IO.InitProc != NULL) {
    DEBUG(TNEF->Debug, 2, "About to initialize");
    if (TNEF->IO.InitProc(&TNEF->IO) != 0) {
      return YTNEF_CANNOT_INIT_DATA;
    }
    DEBUG(TNEF->Debug, 2, "Initialization finished");
  }

  DEBUG(TNEF->Debug, 2, "Reading Signature");
  if (TNEF->IO.ReadProc(&TNEF->IO, sizeof(DWORD), 1, &signature) < 1) {
    printf("ERROR: Error reading signature\n");
    if (TNEF->IO.CloseProc != NULL) {
      TNEF->IO.CloseProc(&TNEF->IO);
    }
    return YTNEF_ERROR_READING_DATA;
  }

  DEBUG(TNEF->Debug, 2, "Checking Signature");
  if (TNEFCheckForSignature(signature) < 0) {
    printf("ERROR: Signature does not match. Not TNEF.\n");
    if (TNEF->IO.CloseProc != NULL) {
      TNEF->IO.CloseProc(&TNEF->IO);
    }
    return YTNEF_NOT_TNEF_STREAM;
  }

  DEBUG(TNEF->Debug, 2, "Reading Key.");

  if (TNEFGetKey(TNEF, &key) < 0) {
    printf("ERROR: Unable to retrieve key.\n");
    if (TNEF->IO.CloseProc != NULL) {
      TNEF->IO.CloseProc(&TNEF->IO);
    }
    return YTNEF_NO_KEY;
  }

  DEBUG(TNEF->Debug, 2, "Starting Full Processing.");

  while (TNEFGetHeader(TNEF, &type, &size) == 0) {
    DEBUG2(TNEF->Debug, 2, "Header says type=0x%X, size=%u", type, size);
    DEBUG2(TNEF->Debug, 2, "Header says type=%u, size=%u", type, size);
    if(size == 0) {
      printf("ERROR: Field with size of 0\n");
      return YTNEF_ERROR_READING_DATA;
    }
    data = calloc(size, sizeof(BYTE));
    ALLOCCHECK(data);
    if (TNEFRawRead(TNEF, data, size, &header_checksum) < 0) {
      printf("ERROR: Unable to read data.\n");
      if (TNEF->IO.CloseProc != NULL) {
        TNEF->IO.CloseProc(&TNEF->IO);
      }
      free(data);
      return YTNEF_ERROR_READING_DATA;
    }
    if (TNEFRawRead(TNEF, (BYTE *)&checksum, 2, NULL) < 0) {
      printf("ERROR: Unable to read checksum.\n");
      if (TNEF->IO.CloseProc != NULL) {
        TNEF->IO.CloseProc(&TNEF->IO);
      }
      free(data);
      return YTNEF_ERROR_READING_DATA;
    }
    checksum = SwapWord((BYTE *)&checksum, sizeof(WORD));
    if (checksum != header_checksum) {
      printf("ERROR: Checksum mismatch. Data corruption?:\n");
      if (TNEF->IO.CloseProc != NULL) {
        TNEF->IO.CloseProc(&TNEF->IO);
      }
      free(data);
      return YTNEF_BAD_CHECKSUM;
    }
    for (i = 0; i < (sizeof(TNEFList) / sizeof(TNEFHandler)); i++) {
      if (TNEFList[i].id == type) {
        if (TNEFList[i].handler != NULL) {
          if (TNEFList[i].handler(TNEF, i, (char*)data, size) < 0) {
            free(data);
            if (TNEF->IO.CloseProc != NULL) {
              TNEF->IO.CloseProc(&TNEF->IO);
            }
            return YTNEF_ERROR_IN_HANDLER;
          } else {
            //  Found our handler and processed it.  now time to get out
            break;
          }
        } else {
          DEBUG2(TNEF->Debug, 1, "No handler for %s: %u bytes",
                 TNEFList[i].name, size);
        }
      }
    }

    free(data);
  }

  if (TNEF->IO.CloseProc != NULL) {
    TNEF->IO.CloseProc(&TNEF->IO);
  }
  return 0;

}