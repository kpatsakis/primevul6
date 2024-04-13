void TNEFFreeAttachment(Attachment *p) {
  FREEVARLENGTH(p->Title);
  FREEVARLENGTH(p->MetaFile);
  FREEVARLENGTH(p->TransportFilename);
  FREEVARLENGTH(p->FileData);
  FREEVARLENGTH(p->IconData);
  TNEFFreeMapiProps(&(p->MAPI));
}