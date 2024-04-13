void TNEFInitialize(TNEFStruct *TNEF) {
  INITSTR(TNEF->version);
  INITVARLENGTH(TNEF->from);
  INITVARLENGTH(TNEF->subject);
  INITDTR(TNEF->dateSent);
  INITDTR(TNEF->dateReceived);

  INITSTR(TNEF->messageStatus);
  INITSTR(TNEF->messageClass);
  INITSTR(TNEF->messageID);
  INITSTR(TNEF->parentID);
  INITSTR(TNEF->conversationID);
  INITVARLENGTH(TNEF->body);
  INITSTR(TNEF->priority);
  TNEFInitAttachment(&(TNEF->starting_attach));
  INITDTR(TNEF->dateModified);
  TNEFInitMapi(&(TNEF->MapiProperties));
  INITVARLENGTH(TNEF->CodePage);
  INITVARLENGTH(TNEF->OriginalMessageClass);
  INITVARLENGTH(TNEF->Owner);
  INITVARLENGTH(TNEF->SentFor);
  INITVARLENGTH(TNEF->Delegate);
  INITDTR(TNEF->DateStart);
  INITDTR(TNEF->DateEnd);
  INITVARLENGTH(TNEF->AidOwner);
  TNEF->RequestRes = 0;
  TNEF->IO.data = NULL;
  TNEF->IO.InitProc = NULL;
  TNEF->IO.ReadProc = NULL;
  TNEF->IO.CloseProc = NULL;
}