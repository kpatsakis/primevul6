QPDFWriter::writeTrailer(trailer_e which, int size, bool xref_stream,
                         qpdf_offset_t prev, int linearization_pass)
{
    QPDFObjectHandle trailer = getTrimmedTrailer();
    if (! xref_stream)
    {
	writeString("trailer <<");
    }
    writeStringQDF("\n");
    if (which == t_lin_second)
    {
	writeString(" /Size ");
	writeString(QUtil::int_to_string(size));
    }
    else
    {
	std::set<std::string> keys = trailer.getKeys();
	for (std::set<std::string>::iterator iter = keys.begin();
	     iter != keys.end(); ++iter)
	{
	    std::string const& key = *iter;
	    writeStringQDF("  ");
	    writeStringNoQDF(" ");
	    writeString(QPDF_Name::normalizeName(key));
	    writeString(" ");
	    if (key == "/Size")
	    {
		writeString(QUtil::int_to_string(size));
		if (which == t_lin_first)
		{
		    writeString(" /Prev ");
		    qpdf_offset_t pos = this->pipeline->getCount();
		    writeString(QUtil::int_to_string(prev));
		    int nspaces = pos - this->pipeline->getCount() + 21;
		    if (nspaces < 0)
                    {
                        throw std::logic_error(
                            "QPDFWriter: no padding required in trailer");
                    }
		    writePad(nspaces);
		}
	    }
	    else
	    {
		unparseChild(trailer.getKey(key), 1, 0);
	    }
	    writeStringQDF("\n");
	}
    }

    // Write ID
    writeStringQDF(" ");
    writeString(" /ID [");
    if (linearization_pass == 1)
    {
        writeString("<00000000000000000000000000000000>"
                    "<00000000000000000000000000000000>");
    }
    else
    {
        if ((linearization_pass == 0) && (this->deterministic_id))
        {
            computeDeterministicIDData();
        }
        generateID();
        writeString(QPDF_String(this->id1).unparse(true));
        writeString(QPDF_String(this->id2).unparse(true));
    }
    writeString("]");

    if (which != t_lin_second)
    {
	// Write reference to encryption dictionary
	if (this->encrypted)
	{
	    writeString(" /Encrypt ");
	    writeString(QUtil::int_to_string(this->encryption_dict_objid));
	    writeString(" 0 R");
	}
    }

    writeStringQDF("\n");
    writeStringNoQDF(" ");
    writeString(">>");
}