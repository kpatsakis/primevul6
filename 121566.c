bool decode_jpeg(const std::vector<std::pair<uint32_t, uint32_t> > & huff_input_offsets,
                 std::vector<ThreadHandoff>*luma_row_offset_return)
{
    abitreader* huffr; // bitwise reader for image data

    unsigned char  type = 0x00; // type of current marker segment
    unsigned int   len  = 0; // length of current marker segment
    unsigned int   hpos = 0; // current position in header

    int lastdc[ 4 ] = {0, 0, 0, 0}; // last dc for each component
    Sirikata::Aligned256Array1d<int16_t,64> block; // store block for coeffs
    int peobrun; // previous eobrun
    unsigned int eobrun; // run of eobs
    int rstw; // restart wait counter

    int cmp, bpos, dpos;
    int mcu = 0, sub, csc;
    int eob, sta;
    bool is_baseline = true;
    max_cmp = 0; // the maximum component in a truncated image
    max_bpos = 0; // the maximum band in a truncated image
    memset(max_dpos, 0, sizeof(max_dpos)); // the maximum dpos in a truncated image
    max_sah = 0; // the maximum bit in a truncated image

    // open huffman coded image data for input in abitreader
    huffr = new abitreader( huffdata, hufs );
    // preset count of scans
    scnc = 0;

    // JPEG decompression loop
    while ( true )
    {
        // seek till start-of-scan, parse only DHT, DRI and SOS
        for ( type = 0x00; type != 0xDA; ) {
            if ( 3 + ( uint64_t ) hpos >= hdrs ) break;
            type = hdrdata[ hpos + 1 ];
            len = 2 + B_SHORT( hdrdata[ hpos + 2 ], hdrdata[ hpos + 3 ] );
            if ( ( type == 0xC4 ) || ( type == 0xDA ) || ( type == 0xDD ) ) {
                std::vector<unsigned char> over_data;
                unsigned char * hdr_seg_data = NULL;
                if ((uint64_t)hpos + (uint64_t)len > (uint64_t)hdrs) {
                    over_data.insert(over_data.end(), &hdrdata[hpos], &hdrdata[hpos] + (hdrs - hpos));
                    over_data.resize(len);
                    hdr_seg_data = &over_data[0];
                } else {
                    hdr_seg_data = &( hdrdata[ hpos ] );
                }
                if ( !parse_jfif_jpg( type, len, len, hdr_seg_data ) ) {
                    delete huffr;
                    return false;
                }
            }
            hpos += len;
        }

        // get out if last marker segment type was not SOS
        if ( type != 0xDA ) break;

        // check if huffman tables are available
        for ( csc = 0; csc < cs_cmpc; csc++ ) {
            cmp = cs_cmp[ csc ];
            if ( (( jpegtype == 1 || (( cs_cmpc > 1 || cs_to == 0 ) && cs_sah == 0 )) && htset[ 0 ][ cmpnfo[cmp].huffdc ] == 0 ) || 
                 ( jpegtype == 1 && htset[ 1 ][ cmpnfo[cmp].huffdc ] == 0 ) ||
                 ( cs_cmpc == 1 && cs_to > 0 && cs_sah == 0 && htset[ 1 ][ cmpnfo[cmp].huffac ] == 0 ) ) {
                fprintf( stderr, "huffman table missing in scan%i", scnc );
                delete huffr;
                errorlevel.store(2);
                return false;
            }
        }


        // intial variables set for decoding
        cmp  = cs_cmp[ 0 ];
        csc  = 0;
        mcu  = 0;
        sub  = 0;
        dpos = 0;
        if (!huffr->eof) {
            max_bpos = std::max(max_bpos, cs_to);
            // FIXME: not sure why only first bit of cs_sah is examined but 4 bits of it are stored
            max_sah = std::max(max_sah, std::max(cs_sal,cs_sah));
            for (int i = 0; i < cs_cmpc; ++i) {
                max_cmp = std::max(max_cmp, cs_cmp[i]);
            }
        }
/*
        // startup
        luma_row_offset_return->push_back(crystallize_thread_handoff(huffr,
                                                                     huff_input_offsets,
                                                                     mcu / mcuh,
                                                                     lastdc,
                                                                     cmpnfo[0].bcv / mcuv));
*/
        bool do_handoff_print = true;
        // JPEG imagedata decoding routines
        while ( true )
        {
            // (re)set last DCs for diff coding
            lastdc[ 0 ] = 0;
            lastdc[ 1 ] = 0;
            lastdc[ 2 ] = 0;
            lastdc[ 3 ] = 0;

            // (re)set status
            sta = 0;

            // (re)set eobrun
            eobrun  = 0;
            peobrun = 0;

            // (re)set rst wait counter
            rstw = rsti;
            if (cs_cmpc != colldata.get_num_components()) {
                if (!g_allow_progressive) {
                    custom_exit(ExitCode::PROGRESSIVE_UNSUPPORTED);
                } else {
                    is_baseline = false;
                }
            }

            if (jpegtype != 1) {
                if (!g_allow_progressive) {
                    custom_exit(ExitCode::PROGRESSIVE_UNSUPPORTED);
                } else {
                    is_baseline = false;
                }
            }
            // decoding for interleaved data
            if ( cs_cmpc > 1 )
            {
                if ( jpegtype == 1 ) {
                    // ---> sequential interleaved decoding <---
                    while ( sta == 0 ) {
                        if (do_handoff_print) {
                            luma_row_offset_return->push_back(crystallize_thread_handoff(huffr,
                                                                                         huff_input_offsets,
                                                                                         mcu / mcuh,
                                                                                         lastdc,
                                                                                         cmpnfo[0].bcv / mcuv));
                            do_handoff_print = false;
                        }

                        if(!huffr->eof) {
                            max_dpos[cmp] = std::max(dpos, max_dpos[cmp]); // record the max block read
                        }
                        // decode block
                        eob = decode_block_seq( huffr,
                            &(htrees[ 0 ][ cmpnfo[cmp].huffdc ]),
                            &(htrees[ 1 ][ cmpnfo[cmp].huffac ]),
                            block.begin() );
                        if ( eob > 1 && !block[ eob - 1 ] ) {
                            fprintf( stderr, "cannot encode image with eob after last 0" );
                            errorlevel.store(1);
                        }

                        // fix dc
                        block[ 0 ] += lastdc[ cmp ];
                        lastdc[ cmp ] = block[ 0 ];

                        AlignedBlock&aligned_block = colldata.mutable_block((BlockType)cmp, dpos);

                        // copy to colldata
                        for ( bpos = 0; bpos < eob; bpos++ ) {
                            aligned_block.mutable_coefficients_zigzag(bpos) = block[ bpos ];
                        }
                        // check for errors, proceed if no error encountered
                        int old_mcu = mcu;
                        if ( eob < 0 ) sta = -1;
                        else sta = next_mcupos( &mcu, &cmp, &csc, &sub, &dpos, &rstw, cs_cmpc);
                        if (mcu % mcuh == 0 && old_mcu !=  mcu) {
                            do_handoff_print = true;
                            //fprintf(stderr, "ROW %d\n", (int)row_handoff.size());
                            
                        }
                        if(huffr->eof) {
                            sta = 2;
                            break;
                        }

                    }
                }
                else if ( cs_sah == 0 ) {
                    // ---> progressive interleaved DC decoding <---
                    // ---> succesive approximation first stage <---
                    while ( sta == 0 ) {
                        if (do_handoff_print) {
                            luma_row_offset_return->push_back(crystallize_thread_handoff(huffr,
                                                                                         huff_input_offsets,
                                                                                         mcu / mcuh,
                                                                                         lastdc,
                                                                                         cmpnfo[0].bcv / mcuv));
                            do_handoff_print = false;
                        }
                        if(!huffr->eof) max_dpos[cmp] = std::max(dpos, max_dpos[cmp]); // record the max block serialized
                        sta = decode_dc_prg_fs( huffr,
                            &(htrees[ 0 ][ cmpnfo[cmp].huffdc ]),
                            block.begin() );

                        // fix dc for diff coding
                        colldata.set((BlockType)cmp,0,dpos) = block[0] + lastdc[ cmp ];
                        
                        uint16_t u_last_dc = lastdc[ cmp ] = colldata.set((BlockType)cmp,0,dpos);
                        u_last_dc <<= cs_sal; // lastdc might be negative--this avoids UB
                        // bitshift for succesive approximation
                        colldata.set((BlockType)cmp,0,dpos) = u_last_dc;

                        // next mcupos if no error happened
                        int old_mcu = mcu;
                        if ( sta != -1 ) {
                            sta = next_mcupos( &mcu, &cmp, &csc, &sub, &dpos, &rstw, cs_cmpc);
                        }
                        if (mcu % mcuh == 0 && old_mcu !=  mcu) {
                            do_handoff_print = true;
                            //fprintf(stderr, "ROW %d\n", (int)row_handoff.size());
                            
                        }
                        if(huffr->eof) {
                            sta = 2;
                            break;
                        }

                    }
                }
                else {
                    // ---> progressive interleaved DC decoding <---
                    // ---> succesive approximation later stage <---
                    while ( sta == 0 ) {
                        if(!huffr->eof) max_dpos[cmp] = std::max(dpos, max_dpos[cmp]); // record the max block serialized
                        // decode next bit
                        sta = decode_dc_prg_sa( huffr,
                            block.begin() );

                        // shift in next bit
                        colldata.set((BlockType)cmp,0,dpos) += block[0] << cs_sal;

                        // next mcupos if no error happened
                        if ( sta != -1 )
                            sta = next_mcupos( &mcu, &cmp, &csc, &sub, &dpos, &rstw, cs_cmpc);
                        if(huffr->eof) {
                            sta = 2;
                            break;
                        }

                    }
                }
            }
            else // decoding for non interleaved data
            {
                if ( jpegtype == 1 ) {
                    int vmul = cmpnfo[0].bcv / mcuv;
                    int hmul = cmpnfo[0].bch / mcuh;
                    // ---> sequential non interleaved decoding <---
                    while ( sta == 0 ) {
                        if (do_handoff_print) {
                            luma_row_offset_return->push_back(crystallize_thread_handoff(huffr,
                                                                                         huff_input_offsets,
                                                                                         (dpos/(hmul * vmul)) / mcuh,
                                                                                         lastdc,
                                                                                         cmpnfo[0].bcv / mcuv));
                            do_handoff_print = false;
                        }
                        if(!huffr->eof) max_dpos[cmp] = std::max(dpos, max_dpos[cmp]); // record the max block serialized
                        // decode block
                        eob = decode_block_seq( huffr,
                            &(htrees[ 0 ][ cmpnfo[cmp].huffdc ]),
                            &(htrees[ 1 ][ cmpnfo[cmp].huffac ]),
                            block.begin() );
                        if ( eob > 1 && !block[ eob - 1 ] ) {
                            fprintf( stderr, "cannot encode image with eob after last 0" );
                            errorlevel.store(1);
                        }
                        // fix dc
                        block[ 0 ] += lastdc[ cmp ];
                        lastdc[ cmp ] = block[ 0 ];

                        // copy to colldata
                        AlignedBlock& aligned_block = colldata.mutable_block((BlockType)cmp, dpos);
                        for ( bpos = 0; bpos < eob; bpos++ ) {
                            aligned_block.mutable_coefficients_zigzag(bpos) = block[ bpos ];
                        }
                        
                        // check for errors, proceed if no error encountered
                        if ( eob < 0 ) sta = -1;
                        else sta = next_mcuposn( &cmp, &dpos, &rstw);
                        mcu = dpos / (hmul * vmul);
                        if (cmp == 0 && (mcu % mcuh == 0) && (dpos %(hmul *vmul) == 0)) {
                            do_handoff_print = true;

                        }
                        if(huffr->eof) {
                            sta = 2;
                            break;
                        }

                    }
                }
                else if ( cs_to == 0 ) {
                    if ( cs_sah == 0 ) {
                        // ---> progressive non interleaved DC decoding <---
                        // ---> succesive approximation first stage <---
                        while ( sta == 0 ) {
                            if (do_handoff_print) {
                                luma_row_offset_return->push_back(crystallize_thread_handoff(huffr,
                                                                                             huff_input_offsets,
                                                                                             dpos / cmpnfo[cmp].bch,
                                                                                             lastdc,
                                                                                             cmpnfo[0].bcv / mcuv));
                                do_handoff_print = false;
                            }

                            if(!huffr->eof) max_dpos[cmp] = std::max(dpos, max_dpos[cmp]); // record the max block serialized
                            sta = decode_dc_prg_fs( huffr,
                                &(htrees[ 0 ][ cmpnfo[cmp].huffdc ]),
                                block.begin() );

                            // fix dc for diff coding
                            colldata.set((BlockType)cmp,0,dpos) = block[0] + lastdc[ cmp ];
                            lastdc[ cmp ] = colldata.set((BlockType)cmp,0,dpos);

                            // bitshift for succesive approximation
                            colldata.set((BlockType)cmp,0,dpos) <<= cs_sal;

                            // check for errors, increment dpos otherwise
                            if ( sta != -1 )
                                sta = next_mcuposn( &cmp, &dpos, &rstw );
                            if (cmp == 0 && dpos % cmpnfo[cmp].bch == 0) {
                                do_handoff_print = true;
                            }
                            if(huffr->eof) {
                                sta = 2;
                                break;
                            }

                        }
                    }
                    else {
                        // ---> progressive non interleaved DC decoding <---
                        // ---> succesive approximation later stage <---
                        while( sta == 0 ) {
                            if(!huffr->eof) max_dpos[cmp] = std::max(dpos, max_dpos[cmp]); // record the max block serialized
                            // decode next bit
                            sta = decode_dc_prg_sa( huffr,
                                block.begin() );

                            // shift in next bit
                            colldata.set((BlockType)cmp,0,dpos) += block[0] << cs_sal;

                            // check for errors, increment dpos otherwise
                            if ( sta != -1 )
                                sta = next_mcuposn( &cmp, &dpos, &rstw );
                            if(huffr->eof) {
                                sta = 2;
                                break;
                            }

                        }
                    }
                }
                else {
                    if ( cs_sah == 0 ) {
                        // ---> progressive non interleaved AC decoding <---
                        // ---> succesive approximation first stage <---
                        while ( sta == 0 ) {
                            if(!huffr->eof) max_dpos[cmp] = std::max(dpos, max_dpos[cmp]); // record the max block serialized
                            // decode block
                            eob = decode_ac_prg_fs( huffr,
                                                    &(htrees[ 1 ][ cmpnfo[cmp].huffac ]),
                                                    block.begin(), &eobrun, cs_from, cs_to );

                            // check for non optimal coding
                            if ( ( eob == cs_from ) && ( eobrun > 0 ) &&
                                ( peobrun > 0 ) && ( peobrun <
                                hcodes[ 1 ][ cmpnfo[cmp].huffac ].max_eobrun - 1 ) ) {
                                fprintf( stderr,
                                    "reconstruction of non optimal coding not supported" );
                                errorlevel.store(1);
                            }
                            AlignedBlock &aligned_block = colldata.mutable_block((BlockType)cmp, dpos);
                            // copy to colldata
                            for ( bpos = cs_from; bpos < eob; bpos++ ) {
                                uint16_t block_bpos = block[ bpos ];
                                block_bpos <<= cs_sal; // prevents UB since block_bpos could be negative
                                aligned_block.mutable_coefficients_zigzag(bpos) = block_bpos;
                            }
                            // check for errors
                            if ( eob < 0 ) sta = -1;
                            else sta = skip_eobrun( &cmp, &dpos, &rstw, &eobrun );

                            // proceed only if no error encountered
                            if ( sta == 0 )
                                sta = next_mcuposn( &cmp, &dpos, &rstw );
                            if(huffr->eof) {
                                sta = 2;
                                break;
                            }

                        }
                    }
                    else {
                        // ---> progressive non interleaved AC decoding <---
                        // ---> succesive approximation later stage <---
                        while ( sta == 0 ) {
                            // copy from colldata
                            AlignedBlock &aligned_block = colldata.mutable_block((BlockType)cmp, dpos);
                            for ( bpos = cs_from; bpos <= cs_to; bpos++ ) {
                                block[ bpos ] = aligned_block.coefficients_zigzag(bpos);
                            }
                            if ( eobrun == 0 ) {
                                if(!huffr->eof) max_dpos[cmp] = std::max(dpos, max_dpos[cmp]); // record the max block serialized
                                // decode block (long routine)
                                eob = decode_ac_prg_sa( huffr,
                                                        &(htrees[ 1 ][ cmpnfo[cmp].huffac ]),
                                                        block.begin(), &eobrun, cs_from, cs_to );

                                // check for non optimal coding
                                if ( ( eob == cs_from ) && ( eobrun > 0 ) &&
                                    ( peobrun > 0 ) && ( peobrun <
                                    hcodes[ 1 ][ cmpnfo[cmp].huffac ].max_eobrun - 1 ) ) {
                                    fprintf( stderr,
                                        "reconstruction of non optimal coding not supported" );
                                    errorlevel.store(1);
                                }

                            }
                            else {
                                if(!huffr->eof) max_dpos[cmp] = std::max(dpos, max_dpos[cmp]); // record the max block serialized
                                // decode block (short routine)
                                eob = decode_eobrun_sa( huffr,
                                                        block.begin(), &eobrun, cs_from, cs_to );
                                if ( eob > 1 && !block[ eob - 1 ] ) {
                                    fprintf( stderr, "cannot encode image with eob after last 0" );
                                    errorlevel.store(1);
                                }
                            }
                            // store eobrun
                            peobrun = eobrun;
                            // copy back to colldata
                            for ( bpos = cs_from; bpos <= cs_to; bpos++ ) {
                                uint16_t block_bpos = block[ bpos ];
                                block_bpos <<= cs_sal;
                                aligned_block.mutable_coefficients_zigzag(bpos) += block_bpos;
                            }
                            // proceed only if no error encountered
                            if ( eob < 0 ) sta = -1;
                            else sta = next_mcuposn( &cmp, &dpos, &rstw );
                            if(huffr->eof) {
                                sta = 2;
                                break;
                            }
                        }
                    }
                }
            }
            // unpad huffman reader / check padbit
            if ( padbit != -1 ) {
                if ( padbit != huffr->unpad( padbit ) ) {
                    fprintf( stderr, "inconsistent use of padbits" );
                    padbit = 1;
                    errorlevel.store(1);
                }
            }
            else {
                padbit = huffr->unpad( padbit );
            }
            // evaluate status
            if ( sta == -1 ) { // status -1 means error
                fprintf( stderr, "decode error in scan%i / mcu%i",
                    scnc, ( cs_cmpc > 1 ) ? mcu : dpos );
                delete huffr;
                errorlevel.store(2);
                return false;
            }
            else if ( sta == 2 ) { // status 2/3 means done
                scnc++; // increment scan counter
                break; // leave decoding loop, everything is done here
            }
            // else if ( sta == 1 ); // status 1 means restart - so stay in the loop
        }
    }
    if (early_eof_encountered) {
        colldata.set_truncation_bounds(max_cmp, max_bpos, max_dpos, max_sah);
    }
    luma_row_offset_return->push_back(crystallize_thread_handoff(huffr, huff_input_offsets, (uint16_t)(mcu / mcuh), lastdc, cmpnfo[0].bcv / mcuv));
    for (size_t i = 1; i < luma_row_offset_return->size(); ++i) {
        if ((*luma_row_offset_return)[i].luma_y_start < 
            (*luma_row_offset_return)[i-1].luma_y_end) {
            (*luma_row_offset_return)[i].luma_y_start = (*luma_row_offset_return)[i-1].luma_y_end;
        }
    }
    // check for unneeded data
    if ( !huffr->eof ) {
        fprintf( stderr, "unneeded data found after coded image data" );
        errorlevel.store(1);
    }

    // clean up
    delete( huffr );

    if (is_baseline) {
        g_allow_progressive = false;
    }
    return true;
}