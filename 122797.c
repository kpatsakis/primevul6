void TNEFInitAttachment(Attachment *p) {
  INITDTR(p->Date);
  INITVARLENGTH(p->Title);
  INITVARLENGTH(p->MetaFile);
  INITDTR(p->CreateDate);
  INITDTR(p->ModifyDate);
  INITVARLENGTH(p->TransportFilename);
  INITVARLENGTH(p->FileData);
  INITVARLENGTH(p->IconData);
  memset(&(p->RenderData), 0, sizeof(renddata));
  TNEFInitMapi(&(p->MAPI));
  p->next = NULL;
}