QPDFWriter::openObject(int objid)
{
    if (objid == 0)
    {
	objid = this->next_objid++;
    }
    this->xref[objid] = QPDFXRefEntry(1, pipeline->getCount(), 0);
    writeString(QUtil::int_to_string(objid));
    writeString(" 0 obj\n");
    return objid;
}