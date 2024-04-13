QPDFWriter::closeObject(int objid)
{
    // Write a newline before endobj as it makes the file easier to
    // repair.
    writeString("\nendobj\n");
    writeStringQDF("\n");
    this->lengths[objid] = pipeline->getCount() - this->xref[objid].getOffset();
}