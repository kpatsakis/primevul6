void TNEFFree(TNEFStruct *TNEF) {
  Attachment *p, *store;

  FREEVARLENGTH(TNEF->from);
  FREEVARLENGTH(TNEF->subject);
  FREEVARLENGTH(TNEF->body);
  FREEVARLENGTH(TNEF->CodePage);
  FREEVARLENGTH(TNEF->OriginalMessageClass);
  FREEVARLENGTH(TNEF->Owner);
  FREEVARLENGTH(TNEF->SentFor);
  FREEVARLENGTH(TNEF->Delegate);
  FREEVARLENGTH(TNEF->AidOwner);
  TNEFFreeMapiProps(&(TNEF->MapiProperties));

  p = TNEF->starting_attach.next;
  while (p != NULL) {
    TNEFFreeAttachment(p);
    store = p->next;
    free(p);
    p = store;
  }
}