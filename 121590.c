bool recode_jpeg( void )
{
    if (!g_use_seccomp) {
        pre_byte = clock();
    }
    abitwriter*  huffw; // bitwise writer for image data
    abytewriter* storw; // bytewise writer for storage of correction bits

    unsigned char  type = 0x00; // type of current marker segment
    unsigned int   len  = 0; // length of current marker segment
    unsigned int   hpos = 0; // current position in header

    int lastdc[ 4 ]; // last dc for each component
    Sirikata::Aligned256Array1d<int16_t, 64> block; // store block for coeffs
    unsigned int eobrun; // run of eobs
    int rstw; // restart wait counter

    int cmp, bpos, dpos;
    int mcu, sub, csc;
    int eob, sta;
    int tmp;

    // open huffman coded image data in abitwriter
    huffw = new abitwriter( ABIT_WRITER_PRELOAD, max_file_size);
    huffw->fillbit = padbit;

    // init storage writer
    storw = new abytewriter( ABIT_WRITER_PRELOAD);

    // preset count of scans and restarts
    scnc = 0;
    rstc = 0;
    MergeJpegProgress streaming_progress;

    // JPEG decompression loop
    while ( true )
    {
        // seek till start-of-scan, parse only DHT, DRI and SOS
        for ( type = 0x00; type != 0xDA; ) {
            if ( hpos >= hdrs ) break;
            type = hpos + 1 < hdrs ? hdrdata[ hpos + 1 ] : 0;
            len = 2 + B_SHORT( hpos + 2 < hdrs ? hdrdata[ hpos + 2 ]:0, hpos + 3 < hdrs ? hdrdata[ hpos + 3 ] :0);
            if ( ( type == 0xC4 ) || ( type == 0xDA ) || ( type == 0xDD ) ) {
                if ( !parse_jfif_jpg( type, len, len > hdrs - hpos ? hdrs - hpos : len, &( hdrdata[ hpos ] ) ) ) {
                    delete huffw;
                    delete storw;
                    return false;
                }
                int max_scan = 0;
                for (int i = 0; i < cmpc; ++i) {
                    max_scan = std::max(max_scan, cmpnfo[i].bcv);
                }
                rstp.reserve(max_scan);
                scnp.reserve(max_scan);
                hpos += len;
            }
            else {
                hpos += len;
                continue;
            }
        }

        // get out if last marker segment type was not SOS
        if ( type != 0xDA ) break;


        // (re)alloc scan positons array
        while ((int)scnp.size() < scnc + 2) {
            scnp.push_back(0);
        }

        // (re)alloc restart marker positons array if needed
        if ( rsti > 0 ) {
            tmp = rstc + ( ( cs_cmpc > 1 ) ?
                ( mcuc / rsti ) : ( cmpnfo[ cs_cmp[ 0 ] ].bc / rsti ) );
            while ((int)rstp.size() <= tmp ) {
                rstp.push_back((unsigned int) -1 );
            }
        }

        // intial variables set for encoding
        cmp  = cs_cmp[ 0 ];
        csc  = 0;
        mcu  = 0;
        sub  = 0;
        dpos = 0;

        // store scan position
        scnp.at(scnc) = huffw->getpos();
        scnp.at(scnc + 1) = 0; // danielrh@ avoid uninitialized memory when doing progressive writeout
        bool first_pass = true;
        // JPEG imagedata encoding routines
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
            eobrun = 0;

            // (re)set rst wait counter
            rstw = rsti;
            if (cs_cmpc != colldata.get_num_components() && !g_allow_progressive) {
                custom_exit(ExitCode::PROGRESSIVE_UNSUPPORTED);
            }
            if (jpegtype != 1 && !g_allow_progressive) {
                custom_exit(ExitCode::PROGRESSIVE_UNSUPPORTED);
            }
            if ((jpegtype != 1 || cs_cmpc != colldata.get_num_components())
                && colldata.is_memory_optimized(0)
                && first_pass) {
                colldata.init(cmpnfo, cmpc, mcuh, mcuv, false);
            }
            first_pass = false;
            // encoding for interleaved data
            if ( cs_cmpc > 1 )
            {
                if ( jpegtype == 1 ) {
                    // ---> sequential interleaved encoding <---
                    while ( sta == 0 ) {
                        // copy from colldata
                        const AlignedBlock &aligned_block = colldata.block((BlockType)cmp, dpos);
                        //fprintf(stderr, "Reading from cmp(%d) dpos %d\n", cmp, dpos);
                        for ( bpos = 0; bpos < 64; bpos++ ) {
                            block[bpos] = aligned_block.coefficients_zigzag(bpos);
                        }
                        int16_t dc = block[0];
                        // diff coding for dc
                        block[ 0 ] -= lastdc[ cmp ];
                        lastdc[ cmp ] = dc;

                        // encode block
                        eob = encode_block_seq( huffw,
                                                &(hcodes[ 0 ][ cmpnfo[cmp].huffdc ]),
                                                &(hcodes[ 1 ][ cmpnfo[cmp].huffac ]),
                                                block.begin() );

                        // check for errors, proceed if no error encountered
                        if ( eob < 0 ) sta = -1;
                        else sta = next_mcupos( &mcu, &cmp, &csc, &sub, &dpos, &rstw, cs_cmpc);
                        if (sta == 0 && huffw->no_remainder()) {
                            merge_jpeg_streaming(&streaming_progress, huffw->peekptr(), huffw->getpos(), false);
                        }
                        if (str_out->has_exceeded_bound()) {
                            sta = 2;
                        }
                    }
                }
                else if ( cs_sah == 0 ) {
                    // ---> progressive interleaved DC encoding <---
                    // ---> succesive approximation first stage <---
                    while ( sta == 0 ) {
                        // diff coding & bitshifting for dc
                        tmp = colldata.at((BlockType)cmp , 0 , dpos ) >> cs_sal;
                        block[ 0 ] = tmp - lastdc[ cmp ];
                        lastdc[ cmp ] = tmp;

                        // encode dc
                        sta = encode_dc_prg_fs( huffw,
                                                &(hcodes[ 0 ][ cmpnfo[cmp].huffdc ]),
                                                block.begin() );

                        // next mcupos if no error happened
                        if ( sta != -1 )
                            sta = next_mcupos( &mcu, &cmp, &csc, &sub, &dpos, &rstw, cs_cmpc);
                        if (sta == 0 && huffw->no_remainder()) {
                            merge_jpeg_streaming(&streaming_progress, huffw->peekptr(), huffw->getpos(), false);
                        }
                        if (str_out->has_exceeded_bound()) {
                            sta = 2;
                        }
                    }
                }
                else {
                    // ---> progressive interleaved DC encoding <---
                    // ---> succesive approximation later stage <---
                    while ( sta == 0 ) {
                        // fetch bit from current bitplane
                        block[ 0 ] = BITN( colldata.at((BlockType)cmp , 0 , dpos ), cs_sal );

                        // encode dc correction bit
                        sta = encode_dc_prg_sa( huffw, block.begin() );

                        // next mcupos if no error happened
                        if ( sta != -1 )
                            sta = next_mcupos( &mcu, &cmp, &csc, &sub, &dpos, &rstw, cs_cmpc);
                        if (sta == 0 && huffw->no_remainder()) {
                            merge_jpeg_streaming(&streaming_progress, huffw->peekptr(), huffw->getpos(), false);
                        }
                        if (str_out->has_exceeded_bound()) {
                            sta = 2;
                        }

                    }
                }
            }
            else // encoding for non interleaved data
            {
                if ( jpegtype == 1 ) {
                    // ---> sequential non interleaved encoding <---
                    while ( sta == 0 ) {
                        const AlignedBlock& aligned_block = colldata.block((BlockType)cmp, dpos);
                        // copy from colldata
                        int16_t dc = block[ 0 ] = aligned_block.dc();
                        for ( bpos = 1; bpos < 64; bpos++ )
                            block[ bpos ] = aligned_block.coefficients_zigzag(bpos);

                        // diff coding for dc
                        block[ 0 ] -= lastdc[ cmp ];
                        lastdc[ cmp ] = dc;

                        // encode block
                        eob = encode_block_seq( huffw,
                            &(hcodes[ 0 ][ cmpnfo[cmp].huffdc ]),
                            &(hcodes[ 1 ][ cmpnfo[cmp].huffac ]),
                                                block.begin() );

                        // check for errors, proceed if no error encountered
                        if ( eob < 0 ) sta = -1;
                        else sta = next_mcuposn( &cmp, &dpos, &rstw);
                        if (sta == 0 && huffw->no_remainder()) {
                            merge_jpeg_streaming(&streaming_progress, huffw->peekptr(), huffw->getpos(), false);
                        }
                        if (str_out->has_exceeded_bound()) {
                            sta = 2;
                        }

                    }
                }
                else if ( cs_to == 0 ) {
                    if ( cs_sah == 0 ) {
                        // ---> progressive non interleaved DC encoding <---
                        // ---> succesive approximation first stage <---
                        while ( sta == 0 ) {
                            // diff coding & bitshifting for dc
                            tmp = colldata.at((BlockType)cmp , 0 , dpos ) >> cs_sal;
                            block[ 0 ] = tmp - lastdc[ cmp ];
                            lastdc[ cmp ] = tmp;

                            // encode dc
                            sta = encode_dc_prg_fs( huffw,
                                &(hcodes[ 0 ][ cmpnfo[cmp].huffdc ]),
                                                    block.begin() );

                            // check for errors, increment dpos otherwise
                            if ( sta != -1 )
                                sta = next_mcuposn( &cmp, &dpos, &rstw );
                            if (sta == 0 && huffw->no_remainder()) {
                                merge_jpeg_streaming(&streaming_progress, huffw->peekptr(), huffw->getpos(), false);
                            }
                            if (str_out->has_exceeded_bound()) {
                                sta = 2;
                            }

                        }
                    }
                    else {
                        // ---> progressive non interleaved DC encoding <---
                        // ---> succesive approximation later stage <---
                        while ( sta == 0 ) {
                            // fetch bit from current bitplane
                            block[ 0 ] = BITN( colldata.at((BlockType)cmp , 0 , dpos ), cs_sal );

                            // encode dc correction bit
                            sta = encode_dc_prg_sa( huffw, block.begin() );

                            // next mcupos if no error happened
                            if ( sta != -1 )
                                sta = next_mcuposn( &cmp, &dpos, &rstw );
                        }
                        if (str_out->has_exceeded_bound()) {
                            sta = 2;
                        }
                    }
                }
                else {
                    if ( cs_sah == 0 ) {
                        // ---> progressive non interleaved AC encoding <---
                        // ---> succesive approximation first stage <---
                        while ( sta == 0 ) {
                            const AlignedBlock& aligned_block = colldata.block((BlockType)cmp, dpos);
                            // copy from colldata
                            for ( bpos = cs_from; bpos <= cs_to; bpos++ ) {
                                block[ bpos ] =
                                    FDIV2( aligned_block.coefficients_zigzag(bpos), cs_sal );
                            }
                            // encode block
                            eob = encode_ac_prg_fs( huffw,
                                &(hcodes[ 1 ][ cmpnfo[cmp].huffac ]),
                                                    block.begin(), &eobrun, cs_from, cs_to );

                            // check for errors, proceed if no error encountered
                            if ( eob < 0 ) sta = -1;
                            else sta = next_mcuposn( &cmp, &dpos, &rstw );
                            if (sta == 0 && huffw->no_remainder()) {
                                merge_jpeg_streaming(&streaming_progress, huffw->peekptr(), huffw->getpos(), false);
                            }
                            if (str_out->has_exceeded_bound()) {
                                sta = 2;
                            }

                        }

                        // encode remaining eobrun
                        encode_eobrun( huffw,
                            &(hcodes[ 1 ][ cmpnfo[cmp].huffac ]),
                            &eobrun );

                    }
                    else {
                        // ---> progressive non interleaved AC encoding <---
                        // ---> succesive approximation later stage <---
                        while ( sta == 0 ) {
                            const AlignedBlock& aligned_block= colldata.block((BlockType)cmp, dpos);
                            // copy from colldata
                            for ( bpos = cs_from; bpos <= cs_to; bpos++ ) {
                                block[ bpos ] =
                                    FDIV2( aligned_block.coefficients_zigzag(bpos), cs_sal );
                            }
                            // encode block
                            eob = encode_ac_prg_sa( huffw, storw,
                                &(hcodes[ 1 ][ cmpnfo[cmp].huffac ]),
                                block.begin(), &eobrun, cs_from, cs_to );

                            // check for errors, proceed if no error encountered
                            if ( eob < 0 ) sta = -1;
                            else sta = next_mcuposn( &cmp, &dpos, &rstw );
                            if (sta == 0 && huffw->no_remainder()) {
                                merge_jpeg_streaming(&streaming_progress, huffw->peekptr(), huffw->getpos(), false);
                            }
                            if (str_out->has_exceeded_bound()) {
                                sta = 2;
                            }

                        }

                        // encode remaining eobrun
                        encode_eobrun( huffw,
                            &(hcodes[ 1 ][ cmpnfo[cmp].huffac ]),
                            &eobrun );

                        // encode remaining correction bits
                        encode_crbits( huffw, storw );
                    }
                }
            }

            // pad huffman writer
            huffw->pad( padbit );

            // evaluate status
            if ( sta == -1 ) { // status -1 means error
                fprintf( stderr, "encode error in scan%i / mcu%i",
                    scnc, ( cs_cmpc > 1 ) ? mcu : dpos );
                delete huffw;
                errorlevel.store(2);
                return false;
            }
            else if ( sta == 2 ) { // status 2 means done
                scnc++; // increment scan counter
                break; // leave decoding loop, everything is done here
            }
            else if ( sta == 1 ) { // status 1 means restart
                if ( rsti > 0 ) // store rstp & stay in the loop
                    rstp.at(rstc++) = huffw->getpos() - 1;
            }
            huffw->flush_no_pad();
            dev_assert(huffw->no_remainder() && "this should have been padded");
            if (huffw->no_remainder()) {
                merge_jpeg_streaming(&streaming_progress, huffw->peekptr(), huffw->getpos(), false);
            }
        }
    }

    // safety check for error in huffwriter
    if ( huffw->error ) {
        delete huffw;
        fprintf( stderr, MEM_ERRMSG );
        errorlevel.store(2);
        return false;
    }

    // get data into huffdata
    huffdata = huffw->getptr();
    hufs = huffw->getpos();
    always_assert(huffw->no_remainder() && "this should have been padded");
    merge_jpeg_streaming(&streaming_progress, huffdata, hufs, true);
    if (!fast_exit) {
        delete huffw;

        // remove storage writer
        delete storw;
    }
    // store last scan & restart positions
    if (scnc >= scnp.size()) {
        delete huffw;
        fprintf( stderr, MEM_ERRMSG );
        errorlevel.store(2);
        return false;
    }
    scnp.at(scnc) = hufs;
    if ( !rstp.empty() )
        rstp.at(rstc) = hufs;


    return true;
}