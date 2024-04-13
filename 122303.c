QPDFWriter::writeLinearized()
{
    // Optimize file and enqueue objects in order

    discardGeneration(this->object_to_object_stream,
                      this->object_to_object_stream_no_gen);

    bool need_xref_stream = (! this->object_to_object_stream.empty());
    pdf.optimize(this->object_to_object_stream_no_gen);

    std::vector<QPDFObjectHandle> part4;
    std::vector<QPDFObjectHandle> part6;
    std::vector<QPDFObjectHandle> part7;
    std::vector<QPDFObjectHandle> part8;
    std::vector<QPDFObjectHandle> part9;
    QPDF::Writer::getLinearizedParts(
        this->pdf, this->object_to_object_stream_no_gen,
        part4, part6, part7, part8, part9);

    // Object number sequence:
    //
    //  second half
    //    second half uncompressed objects
    //    second half xref stream, if any
    //    second half compressed objects
    //  first half
    //    linearization dictionary
    //    first half xref stream, if any
    //    part 4 uncompresesd objects
    //    encryption dictionary, if any
    //    hint stream
    //    part 6 uncompressed objects
    //    first half compressed objects
    //

    // Second half objects
    int second_half_uncompressed = part7.size() + part8.size() + part9.size();
    int second_half_first_obj = 1;
    int after_second_half = 1 + second_half_uncompressed;
    this->next_objid = after_second_half;
    int second_half_xref = 0;
    if (need_xref_stream)
    {
	second_half_xref = this->next_objid++;
    }
    // Assign numbers to all compressed objects in the second half.
    std::vector<QPDFObjectHandle>* vecs2[] = {&part7, &part8, &part9};
    for (int i = 0; i < 3; ++i)
    {
	for (std::vector<QPDFObjectHandle>::iterator iter = (*vecs2[i]).begin();
	     iter != (*vecs2[i]).end(); ++iter)
	{
	    assignCompressedObjectNumbers((*iter).getObjGen());
	}
    }
    int second_half_end = this->next_objid - 1;
    int second_trailer_size = this->next_objid;

    // First half objects
    int first_half_start = this->next_objid;
    int lindict_id = this->next_objid++;
    int first_half_xref = 0;
    if (need_xref_stream)
    {
	first_half_xref = this->next_objid++;
    }
    int part4_first_obj = this->next_objid;
    this->next_objid += part4.size();
    int after_part4 = this->next_objid;
    if (this->encrypted)
    {
	this->encryption_dict_objid = this->next_objid++;
    }
    int hint_id = this->next_objid++;
    int part6_first_obj = this->next_objid;
    this->next_objid += part6.size();
    int after_part6 = this->next_objid;
    // Assign numbers to all compressed objects in the first half
    std::vector<QPDFObjectHandle>* vecs1[] = {&part4, &part6};
    for (int i = 0; i < 2; ++i)
    {
	for (std::vector<QPDFObjectHandle>::iterator iter = (*vecs1[i]).begin();
	     iter != (*vecs1[i]).end(); ++iter)
	{
	    assignCompressedObjectNumbers((*iter).getObjGen());
	}
    }
    int first_half_end = this->next_objid - 1;
    int first_trailer_size = this->next_objid;

    int part4_end_marker = part4.back().getObjectID();
    int part6_end_marker = part6.back().getObjectID();
    qpdf_offset_t space_before_zero = 0;
    qpdf_offset_t file_size = 0;
    qpdf_offset_t part6_end_offset = 0;
    qpdf_offset_t first_half_max_obj_offset = 0;
    qpdf_offset_t second_xref_offset = 0;
    qpdf_offset_t first_xref_end = 0;
    qpdf_offset_t second_xref_end = 0;

    this->next_objid = part4_first_obj;
    enqueuePart(part4);
    assert(this->next_objid == after_part4);
    this->next_objid = part6_first_obj;
    enqueuePart(part6);
    assert(this->next_objid == after_part6);
    this->next_objid = second_half_first_obj;
    enqueuePart(part7);
    enqueuePart(part8);
    enqueuePart(part9);
    assert(this->next_objid == after_second_half);

    qpdf_offset_t hint_length = 0;
    PointerHolder<Buffer> hint_buffer;

    // Write file in two passes.  Part numbers refer to PDF spec 1.4.

    for (int pass = 1; pass <= 2; ++pass)
    {
	if (pass == 1)
	{
	    pushDiscardFilter();
            if (this->deterministic_id)
            {
                pushMD5Pipeline();
            }
	}

	// Part 1: header

	writeHeader();

	// Part 2: linearization parameter dictionary.  Save enough
	// space to write real dictionary.  200 characters is enough
	// space if all numerical values in the parameter dictionary
	// that contain offsets are 20 digits long plus a few extra
	// characters for safety.  The entire linearization parameter
	// dictionary must appear within the first 1024 characters of
	// the file.

	qpdf_offset_t pos = this->pipeline->getCount();
	openObject(lindict_id);
	writeString("<<");
	if (pass == 2)
	{
	    std::vector<QPDFObjectHandle> const& pages = pdf.getAllPages();
	    int first_page_object = obj_renumber[pages.at(0).getObjGen()];
	    int npages = pages.size();

	    writeString(" /Linearized 1 /L ");
	    writeString(QUtil::int_to_string(file_size + hint_length));
	    // Implementation note 121 states that a space is
	    // mandatory after this open bracket.
	    writeString(" /H [ ");
	    writeString(QUtil::int_to_string(this->xref[hint_id].getOffset()));
	    writeString(" ");
	    writeString(QUtil::int_to_string(hint_length));
	    writeString(" ] /O ");
	    writeString(QUtil::int_to_string(first_page_object));
	    writeString(" /E ");
	    writeString(QUtil::int_to_string(part6_end_offset + hint_length));
	    writeString(" /N ");
	    writeString(QUtil::int_to_string(npages));
	    writeString(" /T ");
	    writeString(QUtil::int_to_string(space_before_zero + hint_length));
	}
	writeString(" >>");
	closeObject(lindict_id);
	static int const pad = 200;
	int spaces = (pos - this->pipeline->getCount() + pad);
	assert(spaces >= 0);
	writePad(spaces);
	writeString("\n");

        // If the user supplied any additional header text, write it
        // here after the linearization parameter dictionary.
        writeString(this->extra_header_text);

	// Part 3: first page cross reference table and trailer.

	qpdf_offset_t first_xref_offset = this->pipeline->getCount();
	qpdf_offset_t hint_offset = 0;
	if (pass == 2)
	{
	    hint_offset = this->xref[hint_id].getOffset();
	}
	if (need_xref_stream)
	{
	    // Must pad here too.
	    if (pass == 1)
	    {
		// Set first_half_max_obj_offset to a value large
		// enough to force four bytes to be reserved for each
		// file offset.  This would provide adequate space for
		// the xref stream as long as the last object in page
		// 1 starts with in the first 4 GB of the file, which
		// is extremely likely.  In the second pass, we will
		// know the actual value for this, but it's okay if
		// it's smaller.
		first_half_max_obj_offset = 1 << 25;
	    }
	    pos = this->pipeline->getCount();
	    writeXRefStream(first_half_xref, first_half_end,
			    first_half_max_obj_offset,
			    t_lin_first, first_half_start, first_half_end,
			    first_trailer_size,
			    hint_length + second_xref_offset,
			    hint_id, hint_offset, hint_length,
			    (pass == 1), pass);
	    qpdf_offset_t endpos = this->pipeline->getCount();
	    if (pass == 1)
	    {
		// Pad so we have enough room for the real xref
		// stream.
		writePad(calculateXrefStreamPadding(endpos - pos));
		first_xref_end = this->pipeline->getCount();
	    }
	    else
	    {
		// Pad so that the next object starts at the same
		// place as in pass 1.
		writePad(first_xref_end - endpos);

		if (this->pipeline->getCount() != first_xref_end)
                {
                    throw std::logic_error(
                        "insufficient padding for first pass xref stream");
                }
	    }
	    writeString("\n");
	}
	else
	{
	    writeXRefTable(t_lin_first, first_half_start, first_half_end,
			   first_trailer_size, hint_length + second_xref_offset,
			   (pass == 1), hint_id, hint_offset, hint_length,
                           pass);
	    writeString("startxref\n0\n%%EOF\n");
	}

	// Parts 4 through 9

	for (std::list<QPDFObjectHandle>::iterator iter =
		 this->object_queue.begin();
	     iter != this->object_queue.end(); ++iter)
	{
	    QPDFObjectHandle cur_object = (*iter);
	    if (cur_object.getObjectID() == part6_end_marker)
	    {
		first_half_max_obj_offset = this->pipeline->getCount();
	    }
	    writeObject(cur_object);
	    if (cur_object.getObjectID() == part4_end_marker)
	    {
		if (this->encrypted)
		{
		    writeEncryptionDictionary();
		}
		if (pass == 1)
		{
		    this->xref[hint_id] =
			QPDFXRefEntry(1, this->pipeline->getCount(), 0);
		}
		else
		{
		    // Part 5: hint stream
		    writeBuffer(hint_buffer);
		}
	    }
	    if (cur_object.getObjectID() == part6_end_marker)
	    {
		part6_end_offset = this->pipeline->getCount();
	    }
	}

	// Part 10: overflow hint stream -- not used

	// Part 11: main cross reference table and trailer

	second_xref_offset = this->pipeline->getCount();
	if (need_xref_stream)
	{
	    pos = this->pipeline->getCount();
	    space_before_zero =
		writeXRefStream(second_half_xref,
				second_half_end, second_xref_offset,
				t_lin_second, 0, second_half_end,
				second_trailer_size,
				0, 0, 0, 0, (pass == 1), pass);
	    qpdf_offset_t endpos = this->pipeline->getCount();

	    if (pass == 1)
	    {
		// Pad so we have enough room for the real xref
		// stream.  See comments for previous xref stream on
		// how we calculate the padding.
		writePad(calculateXrefStreamPadding(endpos - pos));
		writeString("\n");
		second_xref_end = this->pipeline->getCount();
	    }
	    else
	    {
		// Make the file size the same.
		qpdf_offset_t pos = this->pipeline->getCount();
		writePad(second_xref_end + hint_length - 1 - pos);
		writeString("\n");

		// If this assertion fails, maybe we didn't have
		// enough padding above.
		if (this->pipeline->getCount() !=
                    second_xref_end + hint_length)
                {
                    throw std::logic_error(
                        "count mismatch after xref stream;"
                        " possible insufficient padding?");
                }
	    }
	}
	else
	{
	    space_before_zero =
		writeXRefTable(t_lin_second, 0, second_half_end,
			       second_trailer_size, 0, false, 0, 0, 0, pass);
	}
	writeString("startxref\n");
	writeString(QUtil::int_to_string(first_xref_offset));
	writeString("\n%%EOF\n");

        discardGeneration(this->obj_renumber, this->obj_renumber_no_gen);

	if (pass == 1)
	{
            if (this->deterministic_id)
            {
                QTC::TC("qpdf", "QPDFWriter linearized deterministic ID",
                        need_xref_stream ? 0 : 1);
                computeDeterministicIDData();
                popPipelineStack();
                assert(this->md5_pipeline == 0);
            }

	    // Close first pass pipeline
	    file_size = this->pipeline->getCount();
	    popPipelineStack();

	    // Save hint offset since it will be set to zero by
	    // calling openObject.
	    qpdf_offset_t hint_offset = this->xref[hint_id].getOffset();

	    // Write hint stream to a buffer
	    pushPipeline(new Pl_Buffer("hint buffer"));
	    activatePipelineStack();
	    writeHintStream(hint_id);
	    popPipelineStack(&hint_buffer);
	    hint_length = hint_buffer->getSize();

	    // Restore hint offset
	    this->xref[hint_id] = QPDFXRefEntry(1, hint_offset, 0);
	}
    }
}