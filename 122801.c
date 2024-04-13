  switch (TNEFList[id].id) {
    case attDateSent: Date = &(TNEF->dateSent); break;
    case attDateRecd: Date = &(TNEF->dateReceived); break;
    case attDateModified: Date = &(TNEF->dateModified); break;
    case attDateStart: Date = &(TNEF->DateStart); break;
    case attDateEnd:  Date = &(TNEF->DateEnd); break;
    case attAttachCreateDate:
      while (p->next != NULL) p = p->next;
      Date = &(p->CreateDate);
      break;
    case attAttachModifyDate:
      while (p->next != NULL) p = p->next;
      Date = &(p->ModifyDate);
      break;
    default:
      if (TNEF->Debug >= 1)
        printf("MISSING CASE\n");
      return YTNEF_UNKNOWN_PROPERTY;
  }