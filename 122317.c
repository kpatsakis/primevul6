QPDFWriter::writeStandard()
{
    if (this->deterministic_id)
    {
        pushMD5Pipeline();
    }

    // Start writing

    writeHeader();
    writeString(this->extra_header_text);

    if (this->preserve_unreferenced_objects)
    {
        QTC::TC("qpdf", "QPDFWriter preserve unreferenced standard");
        std::vector<QPDFObjectHandle> all = this->pdf.getAllObjects();
        for (std::vector<QPDFObjectHandle>::iterator iter = all.begin();
             iter != all.end(); ++iter)
        {
            enqueueObject(*iter);
        }
    }

    // Put root first on queue.
    QPDFObjectHandle trailer = getTrimmedTrailer();
    enqueueObject(trailer.getKey("/Root"));

    // Next place any other objects referenced from the trailer
    // dictionary into the queue, handling direct objects recursively.
    // Root is already there, so enqueuing it a second time is a
    // no-op.
    std::set<std::string> keys = trailer.getKeys();
    for (std::set<std::string>::iterator iter = keys.begin();
	 iter != keys.end(); ++iter)
    {
	enqueueObject(trailer.getKey(*iter));
    }

    // Now start walking queue, output each object
    while (this->object_queue.size())
    {
	QPDFObjectHandle cur_object = this->object_queue.front();
	this->object_queue.pop_front();
	writeObject(cur_object);
    }

    // Write out the encryption dictionary, if any
    if (this->encrypted)
    {
	writeEncryptionDictionary();
    }

    // Now write out xref.  next_objid is now the number of objects.
    qpdf_offset_t xref_offset = this->pipeline->getCount();
    if (this->object_stream_to_objects.empty())
    {
	// Write regular cross-reference table
	writeXRefTable(t_normal, 0, this->next_objid - 1, this->next_objid);
    }
    else
    {
	// Write cross-reference stream.
	int xref_id = this->next_objid++;
	writeXRefStream(xref_id, xref_id, xref_offset, t_normal,
			0, this->next_objid - 1, this->next_objid);
    }
    writeString("startxref\n");
    writeString(QUtil::int_to_string(xref_offset));
    writeString("\n%%EOF\n");

    if (this->deterministic_id)
    {
	QTC::TC("qpdf", "QPDFWriter standard deterministic ID",
                this->object_stream_to_objects.empty() ? 0 : 1);
        popPipelineStack();
        assert(this->md5_pipeline == 0);
    }
}