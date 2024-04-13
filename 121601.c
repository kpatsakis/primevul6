bool write_ujpg(std::vector<ThreadHandoff> row_thread_handoffs,
                std::vector<uint8_t, Sirikata::JpegAllocator<uint8_t> >*jpeg_file_raw_bytes)
{
    unsigned char ujpg_mrk[ 64 ];
    bool has_lepton_entropy_coding = (ofiletype == LEPTON || filetype == LEPTON );
    Sirikata::JpegError err = Sirikata::JpegError::nil();

    if (!has_lepton_entropy_coding) {
        // UJG-Header
        err = ujg_out->Write( ujg_header, 2 ).second;
    } else {
        // lepton-Header
        err = ujg_out->Write( lepton_header, 2 ).second;
    }
    // store version number
    ujpg_mrk[ 0 ] = ujgversion;
    ujg_out->Write( ujpg_mrk, 1 );

    // discard meta information from header if needed
    if ( disc_meta )
        if ( !rebuild_header_jpg() )
            return false;
    if (start_byte) {
        std::vector<ThreadHandoff> local_row_thread_handoffs;
        for (std::vector<ThreadHandoff>::iterator i = row_thread_handoffs.begin(),
                 ie = row_thread_handoffs.end(); i != ie; ++i) {
            auto j = i;
            ++j;
            if ((j == ie || i->segment_size >= start_byte)
                && (max_file_size == 0 || i->segment_size <= max_file_size + start_byte)) {
                local_row_thread_handoffs.push_back(*i);
                //fprintf(stderr, "OK: %d (%d %d)\n", i->segment_size, i->luma_y_start, i->luma_y_end);
            } else {
                //fprintf(stderr, "XX: %d (%d %d)\n", i->segment_size, i->luma_y_start, i->luma_y_end);
            }
        }
        row_thread_handoffs.swap(local_row_thread_handoffs);
    }
    if (start_byte) {
        always_assert(jpeg_file_raw_bytes);
    }
    if (start_byte && jpeg_file_raw_bytes && !row_thread_handoffs.empty()) { // FIXME: respect jpeg_embedding?
        if (row_thread_handoffs[0].segment_size >= start_byte) {
            prefix_grbs = row_thread_handoffs[0].segment_size - start_byte;
            if (row_thread_handoffs.size() > 1) {
                if (prefix_grbs) {
                    --prefix_grbs; //FIXME why is this ?!
                }
            }
        } else {
            prefix_grbs = 0;
            custom_exit(ExitCode::ONLY_GARBAGE_NO_JPEG);
        }
        if (prefix_grbs > 0) {
            prefix_grbgdata = aligned_alloc(prefix_grbs);
            always_assert(jpeg_file_raw_bytes->size() >= (size_t)prefix_grbs + start_byte);
            memcpy(prefix_grbgdata,
                   &(*jpeg_file_raw_bytes)[start_byte],
                   std::min((size_t)prefix_grbs,
                            jpeg_file_raw_bytes->size() - start_byte));
        } else {
            prefix_grbgdata = aligned_alloc(1); // so it's nonnull
            prefix_grbgdata[0] = 0;
        }
    }
    Sirikata::MemReadWriter mrw((Sirikata::JpegAllocator<uint8_t>()));
#if 0
    for (uint32_t i = 0; i < row_thread_handoffs.size() ; ++ i) {
        fprintf(stderr,
                "Row [%d - %d], %d size %d overhang byte %d num overhang bits %d  dc %d %d %d\n",
                (int)row_thread_handoffs[i].luma_y_start,
                (int)row_thread_handoffs[i].luma_y_end,
                (int)i,
                (int)row_thread_handoffs[i].segment_size,
                (int)row_thread_handoffs[i].overhang_byte,
                (int)row_thread_handoffs[i].num_overhang_bits,
                (int)row_thread_handoffs[i].last_dc[0],
                (int)row_thread_handoffs[i].last_dc[1],
                (int)row_thread_handoffs[i].last_dc[2]);
    }
#endif
    uint32_t framebuffer_byte_size = row_thread_handoffs.back().segment_size - row_thread_handoffs.front().segment_size;
    uint32_t num_rows = row_thread_handoffs.size();
    NUM_THREADS = std::min(NUM_THREADS, (unsigned int)max_encode_threads);
    if (num_rows / 2 < NUM_THREADS) {
        unsigned int desired_count = std::max((unsigned int)num_rows / 2,
                                              (unsigned int)min_encode_threads);
        NUM_THREADS = std::min(std::max(desired_count, 1U), (unsigned int)NUM_THREADS);
    }
    if (framebuffer_byte_size < 125000) {
        NUM_THREADS = std::min(std::max(min_encode_threads, 1U), (unsigned int)NUM_THREADS);
    } else if (framebuffer_byte_size < 250000) {
        NUM_THREADS = std::min(std::max(min_encode_threads, 2U), (unsigned int)NUM_THREADS);
    } else if (framebuffer_byte_size < 500000) {
        NUM_THREADS = std::min(std::max(min_encode_threads, 4U), (unsigned int)NUM_THREADS);
    }
    //fprintf(stderr, "Byte size %d num_rows %d Using num threads %u\n", framebuffer_byte_size, num_rows, NUM_THREADS);
    std::vector<ThreadHandoff> selected_splits(NUM_THREADS);
    std::vector<int> split_indices(NUM_THREADS);
    for (uint32_t i = 0; g_even_thread_split == false && i < NUM_THREADS - 1 ; ++ i) {
        ThreadHandoff desired_handoff = row_thread_handoffs.back();
        if(max_file_size && max_file_size + start_byte < desired_handoff.segment_size) {
            desired_handoff.segment_size += row_thread_handoffs.front().segment_size;
        }
        desired_handoff.segment_size -= row_thread_handoffs.front().segment_size;
        
        desired_handoff.segment_size *= (i + 1);
        desired_handoff.segment_size /= NUM_THREADS;
        desired_handoff.segment_size += row_thread_handoffs.front().segment_size;
        auto split = std::lower_bound(row_thread_handoffs.begin() + 1, row_thread_handoffs.end(),
                                      desired_handoff,
                                      ThreadHandoffSegmentCompare());
        if (split == row_thread_handoffs.begin() && split != row_thread_handoffs.end()) {
            //++split;
        } else if (split != row_thread_handoffs.begin() + 1) {
            --split;
        }
        split_indices[i] = split - row_thread_handoffs.begin();
    }
    for (uint32_t i = 0; g_even_thread_split && i < NUM_THREADS - 1 ; ++ i) {
        split_indices[i] = row_thread_handoffs.size() * (i + 1) / NUM_THREADS;
    }
    for (uint32_t index = 0; index < NUM_THREADS - 1 ; ++ index) {
        if (split_indices[index] == split_indices[index + 1]) {
            for (uint32_t i = 0; i < NUM_THREADS - 1 ; ++ i) {
                split_indices[i] = (i + 1) * row_thread_handoffs.size() / NUM_THREADS;
            }
            break;
        }
    }
    split_indices[NUM_THREADS - 1] = row_thread_handoffs.size() - 1;
    size_t last_split_index = 0;
    for (size_t i = 0; i < selected_splits.size(); ++i) {
        size_t beginning_of_range = last_split_index;
        size_t end_of_range = split_indices[i];
        //fprintf(stderr, "Beginning %ld end %ld\n", beginning_of_range, end_of_range);
        last_split_index = end_of_range;
        always_assert( end_of_range < row_thread_handoffs.size() );
        selected_splits[i] = row_thread_handoffs[ end_of_range ] - row_thread_handoffs[ beginning_of_range ];
        if (i + 1 == selected_splits.size() && row_thread_handoffs[ end_of_range ].num_overhang_bits) {
            ++selected_splits[i].segment_size; // need room for that last byte to hold the overhang byte
        }
#if 0
        fprintf(stderr, "%d->%d) %d - %d {%ld}\n", selected_splits[i].luma_y_start,
                selected_splits[i].luma_y_end, 
                row_thread_handoffs[ beginning_of_range ].segment_size,
                row_thread_handoffs[ end_of_range ].segment_size, row_thread_handoffs.size());
#endif
/*
        if (i + 1 == selected_splits.size()) {
            int tmp = selected_splits[i].segment_size;
            selected_splits[i].segment_size = jpgfilesize - row_thread_handoffs[ beginning_of_range ].segment_size;
            fprintf(stderr, "Split size was %x and is %x - %x = %x\n", tmp, jpgfilesize, row_thread_handoffs[ beginning_of_range ].segment_size, selected_splits[i].segment_size);
        }
*/
    }
#if 0
    for (uint32_t i = 0; i < selected_splits.size() ; ++ i) {
        fprintf(stderr,
                "Row [%d - %d] %d size %d overhang byte %d num overhang bits %d  dc %d %d %d\n",
                (int)selected_splits[i].luma_y_start,
                (int)selected_splits[i].luma_y_end,

                (int)i,
                (int)selected_splits[i].segment_size,
                (int)selected_splits[i].overhang_byte,
                (int)selected_splits[i].num_overhang_bits,
                (int)selected_splits[i].last_dc[0],
                (int)selected_splits[i].last_dc[1],
                (int)selected_splits[i].last_dc[2]);
    }
#endif

    always_assert(start_byte||!selected_splits[0].luma_y_start);
    // write header to file
    // marker: "HDR" + [size of header]
    unsigned char hdr_mrk[] = {'H', 'D', 'R'};
    err = mrw.Write( hdr_mrk, sizeof(hdr_mrk) ).second;
    uint32toLE(hdrs, ujpg_mrk);
    err = mrw.Write( ujpg_mrk, 4).second;
    // data: data from header
    mrw.Write( hdrdata, hdrs );
    // beginning here: recovery information (needed for exact JPEG recovery)

    // marker: P0D"
    unsigned char pad_mrk[] = {'P', '0', 'D'};
    err = mrw.Write( pad_mrk, sizeof(pad_mrk) ).second;
    // data: padbit
    err = mrw.Write( (unsigned char*) &padbit, 1 ).second;

    // write luma splits
    unsigned char luma_mrk[1] = {'H'};
    err = mrw.Write( luma_mrk, sizeof(luma_mrk) ).second;
    // data: serialized luma splits
    auto serialized_splits = ThreadHandoff::serialize(&selected_splits[0], selected_splits.size());
    err = mrw.Write(&serialized_splits[0], serialized_splits.size()).second;

    if (!rst_cnt.empty()) {
        unsigned char frs_mrk[] = {'C', 'R', 'S'};
        err = mrw.Write( frs_mrk, 3 ).second;
        uint32toLE((uint32_t)rst_cnt.size(), ujpg_mrk);
        err = mrw.Write( ujpg_mrk, 4).second;
        for (size_t i = 0; i < rst_cnt.size(); ++i) {
            uint32toLE((uint32_t)rst_cnt.at(i), ujpg_mrk);
            err = mrw.Write( ujpg_mrk, 4).second;
        }
    }
    // write number of false set RST markers per scan (if available) to file
    if (!rst_err.empty()) {
        // marker: "FRS" + [number of scans]
        unsigned char frs_mrk[] = {'F', 'R', 'S'};
        err = mrw.Write( frs_mrk, 3 ).second;
        uint32toLE((uint32_t)rst_err.size(), ujpg_mrk);
        err = mrw.Write( ujpg_mrk, 4).second;
        // data: numbers of false set markers
        err = mrw.Write( rst_err.data(), rst_err.size() ).second;
    }
    if (early_eof_encountered) {
        unsigned char early_eof[] = {'E', 'E', 'E'};
        err = mrw.Write( early_eof, sizeof(early_eof) ).second;
        uint32toLE(max_cmp, ujpg_mrk);
        uint32toLE(max_bpos, ujpg_mrk + 4);
        uint32toLE(max_sah, ujpg_mrk + 8);
        uint32toLE(max_dpos[0], ujpg_mrk + 12);
        uint32toLE(max_dpos[1], ujpg_mrk + 16);
        uint32toLE(max_dpos[2], ujpg_mrk + 20);
        uint32toLE(max_dpos[3], ujpg_mrk + 24);
        err = mrw.Write(ujpg_mrk, 28).second;
    }
    // write garbage (data including and after EOI) (if any) to file
    if ( prefix_grbs > 0 || prefix_grbgdata != NULL) {
        // marker: "GRB" + [size of garbage]
        unsigned char grb_mrk[] = {'P', 'G', embedded_jpeg ? (unsigned char)'E': (unsigned char)'R'};
        err = mrw.Write( grb_mrk, sizeof(grb_mrk) ).second;
        uint32toLE(prefix_grbs, ujpg_mrk);
        err = mrw.Write( ujpg_mrk, 4 ).second;
        // data: garbage data
        err = mrw.Write( prefix_grbgdata, prefix_grbs ).second;
    }
    // write garbage (data including and after EOI) (if any) to file
    if ( grbs > 0 ) {
        // marker: "GRB" + [size of garbage]
        unsigned char grb_mrk[] = {'G', 'R', 'B'};
        err = mrw.Write( grb_mrk, sizeof(grb_mrk) ).second;
        uint32toLE(grbs, ujpg_mrk);
        err = mrw.Write( ujpg_mrk, 4 ).second;
        // data: garbage data
        err = mrw.Write( grbgdata, grbs ).second;
    }
    if (mrw.buffer().size() > 1024 * 1024) {
        //custom_exit(ExitCode::HEADER_TOO_LARGE);
    }
    std::vector<uint8_t, Sirikata::JpegAllocator<uint8_t> > compressed_header;
    if (ujgversion == 1) {
        compressed_header =
            Sirikata::ZlibDecoderCompressionWriter::Compress(mrw.buffer().data(),
                                                             mrw.buffer().size(),
                                                             Sirikata::JpegAllocator<uint8_t>());
    } else {
        compressed_header = Sirikata::BrotliCodec::Compress(mrw.buffer().data(),
                                                            mrw.buffer().size(),
                                                            Sirikata::JpegAllocator<uint8_t>());
    }
    write_byte_bill(Billing::HEADER, false, 2 + hdrs + prefix_grbs + grbs);
    static_assert(MAX_NUM_THREADS <= 255, "We only have a single byte for num threads");
    always_assert(NUM_THREADS <= 255);
    unsigned char zed[] = {'\0'};
    if (start_byte != 0) {
        zed[0] = (unsigned char)'Y';
    } else if (g_allow_progressive) {
        zed[0] = (unsigned char)'X';
    } else {
        zed[0] = (unsigned char)'Z';
    }
    err =  ujg_out->Write(zed, sizeof(zed)).second;
    unsigned char num_threads[] = {(unsigned char)NUM_THREADS};
    err =  ujg_out->Write(num_threads, sizeof(num_threads)).second;
    unsigned char zero3[3] = {};
    err =  ujg_out->Write(zero3, sizeof(zero3)).second;
    unsigned char git_revision[12] = {0}; // we only have 12 chars in the header for this
    hex_to_bin(git_revision, GIT_REVISION, sizeof(git_revision));
    err = ujg_out->Write(git_revision, sizeof(git_revision) ).second;
    uint32toLE(jpgfilesize - start_byte, ujpg_mrk);
    err = ujg_out->Write( ujpg_mrk, 4).second;
    write_byte_bill(Billing::HEADER, true, 24);
    uint32toLE((uint32_t)compressed_header.size(), ujpg_mrk);
    err = ujg_out->Write( ujpg_mrk, 4).second;
    write_byte_bill(Billing::HEADER, true, 4);
    auto err2 = ujg_out->Write(compressed_header.data(),
                               compressed_header.size());
    write_byte_bill(Billing::HEADER, true, compressed_header.size());
    zlib_hdrs = compressed_header.size();
    if (err != Sirikata::JpegError::nil() || err2.second != Sirikata::JpegError::nil()) {
        fprintf( stderr, "write error, possibly drive is full" );
        errorlevel.store(2);
        return false;
    }
    unsigned char cmp_mrk[] = {'C', 'M', 'P'};
    err = ujg_out->Write( cmp_mrk, sizeof(cmp_mrk) ).second;
    write_byte_bill(Billing::HEADER, true, 3);
    while (g_encoder->encode_chunk(&colldata, ujg_out,
                                   &selected_splits[0], selected_splits.size()) == CODING_PARTIAL) {
    }
    
    // errormessage if write error
    if ( err != Sirikata::JpegError::nil() ) {
        fprintf( stderr, "write error, possibly drive is full" );
        errorlevel.store(2);
        return false;
    }

    // get filesize, if avail
    if (ujg_out) {
        ujgfilesize = ujg_out->getsize();
    }


    return true;
}