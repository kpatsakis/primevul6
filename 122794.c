 int correct = (size >= sizeof(renddata)) ? sizeof(renddata) : size;
  memcpy(&(p->RenderData), data, correct);
  return 0;
}

// -----------------------------------------------------------------------------
int TNEFVersion STD_ARGLIST {
  WORD major;
  WORD minor;
  minor = SwapWord((BYTE*)data, size);
  major = SwapWord((BYTE*)data + 2, size - 2);

  snprintf(TNEF->version, sizeof(TNEF->version), "TNEF%i.%i", major, minor);
  return 0;
}

// -----------------------------------------------------------------------------
int TNEFIcon STD_ARGLIST {
  Attachment *p;
  // Find the last attachment.
  p = &(TNEF->starting_attach);
  while (p->next != NULL) p = p->next;

  p->IconData.size = size;
  p->IconData.data = calloc(size, sizeof(BYTE));
  ALLOCCHECK(p->IconData.data);
  memcpy(p->IconData.data, data, size);
  return 0;
}

// -----------------------------------------------------------------------------
int TNEFRecipTable STD_ARGLIST {
  DWORD count;
  BYTE *d;
  int current_row;
  int propcount;
  int current_prop;

  d = (BYTE*)data;
  count = SwapDWord((BYTE*)d, 4);
  d += 4;
//    printf("Recipient Table containing %u rows\n", count);

  return 0;

  for (current_row = 0; current_row < count; current_row++) {
    propcount = SwapDWord((BYTE*)d, 4);
    if (TNEF->Debug >= 1)
      printf("> Row %i contains %i properties\n", current_row, propcount);
    d += 4;
    for (current_prop = 0; current_prop < propcount; current_prop++) {


    }
  }
  return 0;
}
// -----------------------------------------------------------------------------
int TNEFAttachmentMAPI STD_ARGLIST {
  Attachment *p;
  // Find the last attachment.
  //
  p = &(TNEF->starting_attach);
  while (p->next != NULL) p = p->next;
  return TNEFFillMapi(TNEF, (BYTE*)data, size, &(p->MAPI));
}
// -----------------------------------------------------------------------------
int TNEFMapiProperties STD_ARGLIST {
  if (TNEFFillMapi(TNEF, (BYTE*)data, size, &(TNEF->MapiProperties)) < 0) {
    printf("ERROR Parsing MAPI block\n");
    return -1;
  };
  if (TNEF->Debug >= 3) {
    MAPIPrint(&(TNEF->MapiProperties));
  }
  return 0;
}

int TNEFFillMapi(TNEFStruct *TNEF, BYTE *data, DWORD size, MAPIProps *p) {
  int i, j;
  DWORD num;
  BYTE *d;
  MAPIProperty *mp;
  DWORD type;
  DWORD length;
  variableLength *vl;

  WORD temp_word;
  DWORD temp_dword;
  DDWORD temp_ddword;
  int count = -1;
  int offset;

  d = data;
  p->count = SwapDWord((BYTE*)data, 4);
  d += 4;
  p->properties = calloc(p->count, sizeof(MAPIProperty));
  ALLOCCHECK(p->properties);
  mp = p->properties;

  for (i = 0; i < p->count; i++) {
    if (count == -1) {
      mp->id = SwapDWord((BYTE*)d, 4);
      d += 4;
      mp->custom = 0;
      mp->count = 1;
      mp->namedproperty = 0;
      length = -1;
      if (PROP_ID(mp->id) >= 0x8000) {
        // Read the GUID
        SIZECHECK(16);
        memcpy(&(mp->guid[0]), d, 16);
        d += 16;

        SIZECHECK(4);
        length = SwapDWord((BYTE*)d, 4);
        d += sizeof(DWORD);
        if (length > 0) {
          mp->namedproperty = length;
          mp->propnames = calloc(length, sizeof(variableLength));
          ALLOCCHECK(mp->propnames);
          while (length > 0) {
            SIZECHECK(4);
            type = SwapDWord((BYTE*)d, 4);
            mp->propnames[length - 1].data = calloc(type, sizeof(BYTE));
            ALLOCCHECK(mp->propnames[length - 1].data);
            mp->propnames[length - 1].size = type;
            d += 4;
            for (j = 0; j < (type >> 1); j++) {
              SIZECHECK(j*2);
              mp->propnames[length - 1].data[j] = d[j * 2];
            }
            d += type + ((type % 4) ? (4 - type % 4) : 0);
            length--;
          }
        } else {
          // READ the type
          SIZECHECK(sizeof(DWORD));
          type = SwapDWord((BYTE*)d, sizeof(DWORD));
          d += sizeof(DWORD);
          mp->id = PROP_TAG(PROP_TYPE(mp->id), type);
        }
        mp->custom = 1;
      }

      DEBUG2(TNEF->Debug, 3, "Type id = %04x, Prop id = %04x", PROP_TYPE(mp->id),
             PROP_ID(mp->id));
      if (PROP_TYPE(mp->id) & MV_FLAG) {
        mp->id = PROP_TAG(PROP_TYPE(mp->id) - MV_FLAG, PROP_ID(mp->id));
        SIZECHECK(4);
        mp->count = SwapDWord((BYTE*)d, 4);
        d += 4;
        count = 0;
      }
      mp->data = calloc(mp->count, sizeof(variableLength));
      ALLOCCHECK(mp->data);
      vl = mp->data;
    } else {
      i--;
      count++;
      vl = &(mp->data[count]);
    }

    switch (PROP_TYPE(mp->id)) {
      case PT_BINARY:
      case PT_OBJECT:
      case PT_STRING8:
      case PT_UNICODE:
        // First number of objects (assume 1 for now)
        if (count == -1) {
          SIZECHECK(4);
          vl->size = SwapDWord((BYTE*)d, 4);
          d += 4;
        }
        // now size of object
        SIZECHECK(4);
        vl->size = SwapDWord((BYTE*)d, 4);
        d += 4;

        // now actual object
        if (vl->size != 0) {    
         SIZECHECK(vl->size);
         if (PROP_TYPE(mp->id) == PT_UNICODE) {
                vl->data =(BYTE*) to_utf8(vl->size, (char*)d);
            } else {
              vl->data = calloc(vl->size, sizeof(BYTE));
              ALLOCCHECK(vl->data);
              memcpy(vl->data, d, vl->size);
            }
        } else {
          vl->data = NULL;
        }

        // Make sure to read in a multiple of 4
        num = vl->size;
        offset = ((num % 4) ? (4 - num % 4) : 0);
        d += num + ((num % 4) ? (4 - num % 4) : 0);
        break;

      case PT_I2:
        // Read in 2 bytes, but proceed by 4 bytes
        vl->size = 2;
        vl->data = calloc(vl->size, sizeof(WORD));
        ALLOCCHECK(vl->data);
        SIZECHECK(sizeof(WORD))
        temp_word = SwapWord((BYTE*)d, sizeof(WORD));
        memcpy(vl->data, &temp_word, vl->size);
        d += 4;
        break;
      case PT_BOOLEAN:
      case PT_LONG:
      case PT_R4:
      case PT_CURRENCY:
      case PT_APPTIME:
      case PT_ERROR:
        vl->size = 4;
        vl->data = calloc(vl->size, sizeof(BYTE));
        ALLOCCHECK(vl->data);
        SIZECHECK(4);
        temp_dword = SwapDWord((BYTE*)d, 4);
        memcpy(vl->data, &temp_dword, vl->size);
        d += 4;
        break;
      case PT_DOUBLE:
      case PT_I8:
      case PT_SYSTIME:
        vl->size = 8;
        vl->data = calloc(vl->size, sizeof(BYTE));
        ALLOCCHECK(vl->data);
        SIZECHECK(8);
        temp_ddword = SwapDDWord(d, 8);
        memcpy(vl->data, &temp_ddword, vl->size);
        d += 8;
        break;
      case PT_CLSID:
        vl->size = 16;
        vl->data = calloc(vl->size, sizeof(BYTE));
        ALLOCCHECK(vl->data);
        SIZECHECK(vl->size);
        memcpy(vl->data, d, vl->size);
        d+=16;
        break;
      default:
        printf("Bad file\n");
        exit(-1);
    }

    switch (PROP_ID(mp->id)) {
      case PR_SUBJECT:
      case PR_SUBJECT_IPM:
      case PR_ORIGINAL_SUBJECT:
      case PR_NORMALIZED_SUBJECT:
      case PR_CONVERSATION_TOPIC:
        DEBUG(TNEF->Debug, 3, "Got a Subject");
        if (TNEF->subject.size == 0) {
          int i;
          DEBUG(TNEF->Debug, 3, "Assigning a Subject");
          TNEF->subject.data = calloc(size, sizeof(BYTE));
          ALLOCCHECK(TNEF->subject.data);
          TNEF->subject.size = vl->size;
          memcpy(TNEF->subject.data, vl->data, vl->size);
          //  Unfortunately, we have to normalize out some invalid
          //  characters, or else the file won't write
          for (i = 0; i != TNEF->subject.size; i++) {
            switch (TNEF->subject.data[i]) {
              case '\\':
              case '/':
              case '\0':
                TNEF->subject.data[i] = '_';
                break;
            }
          }
        }
        break;
    }

    if (count == (mp->count - 1)) {
      count = -1;
    }
    if (count == -1) {
      mp++;
    }

  }
  if ((d - data) < size) {
    if (TNEF->Debug >= 1)  {
      printf("ERROR DURING MAPI READ\n");
      printf("Read %td bytes, Expected %u bytes\n", (d - data), size);
      printf("%td bytes missing\n", size - (d - data));
    }
  } else if ((d - data) > size) {
    if (TNEF->Debug >= 1)  {
      printf("ERROR DURING MAPI READ\n");
      printf("Read %td bytes, Expected %u bytes\n", (d - data), size);
      printf("%li bytes extra\n", (d - data) - size);
    }
  }
  return 0;
}