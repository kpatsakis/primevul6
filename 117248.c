RecordDeleteClientFromContext(RecordContextPtr pContext, XID clientspec)
{
    RecordClientsAndProtocolPtr pRCAP;
    int position;

    if ((pRCAP = RecordFindClientOnContext(pContext, clientspec, &position)))
        RecordDeleteClientFromRCAP(pRCAP, position);
}                               /* RecordDeleteClientFromContext */