QPDFWriter::writeXRefTable(trailer_e which, int first, int last, int size,
			   qpdf_offset_t prev, bool suppress_offsets,
			   int hint_id, qpdf_offset_t hint_offset,
                           qpdf_offset_t hint_length, int linearization_pass)
{
    writeString("xref\n");
    writeString(QUtil::int_to_string(first));
    writeString(" ");
    writeString(QUtil::int_to_string(last - first + 1));
    qpdf_offset_t space_before_zero = this->pipeline->getCount();
    writeString("\n");
    for (int i = first; i <= last; ++i)
    {
	if (i == 0)
	{
	    writeString("0000000000 65535 f \n");
	}
	else
	{
	    qpdf_offset_t offset = 0;
	    if (! suppress_offsets)
	    {
		offset = this->xref[i].getOffset();
		if ((hint_id != 0) &&
		    (i != hint_id) &&
		    (offset >= hint_offset))
		{
		    offset += hint_length;
		}
	    }
	    writeString(QUtil::int_to_string(offset, 10));
	    writeString(" 00000 n \n");
	}
    }
    writeTrailer(which, size, false, prev, linearization_pass);
    writeString("\n");
    return space_before_zero;
}