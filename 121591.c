bool parse_jfif_jpg( unsigned char type, unsigned int len, unsigned int alloc_len, unsigned char* segment )
{
    unsigned int hpos = 4; // current position in segment, start after segment header
    int lval, rval; // temporary variables
    int skip;
    int cmp;
    int i;


    switch ( type )
    {
        case 0xC4: // DHT segment
            // build huffman trees & codes
            while ( hpos < len ) {
                lval = LBITS( hpos < alloc_len ? segment[ hpos ] : 0, 4 );
                rval = RBITS( hpos < alloc_len ? segment[ hpos ]: 0, 4 );
                if ( ((lval < 0) || (lval >= 2)) || ((rval < 0) || (rval >= 4)) )
                    break;

                hpos++;
                // build huffman codes & trees
                if (!build_huffcodes( &(segment[ hpos + 0 ]), alloc_len > hpos ? alloc_len - hpos : 0,
                                      &(segment[ hpos + 16 ]),  alloc_len > hpos + 16 ? alloc_len - hpos - 16 : 0,
                                      &(hcodes[ lval ][ rval ]), &(htrees[ lval ][ rval ]) )) {
                    errorlevel.store(2);
                    return false;
                }
                htset[ lval ][ rval ] = 1;

                skip = 16;
                for ( i = 0; i < 16; i++ )
                    skip += ( int ) (hpos + i < alloc_len ? segment[ hpos + i ] : 0);
                hpos += skip;
            }

            if ( hpos != len ) {
                // if we get here, something went wrong
                fprintf( stderr, "size mismatch in dht marker" );
                errorlevel.store(2);
                return false;
            }
            return true;

        case 0xDB: // DQT segment
            // copy quantization tables to internal memory
            while ( hpos < len ) {
                lval = LBITS( hpos < alloc_len ? segment[ hpos ] :  0, 4 );
                rval = RBITS( hpos < alloc_len ? segment[ hpos ] : 0, 4 );
                if ( (lval < 0) || (lval >= 2) ) break;
                if ( (rval < 0) || (rval >= 4) ) break;
                hpos++;
                if ( lval == 0 ) { // 8 bit precision
                    for ( i = 0; i < 64; i++ ) {
                        qtables[ rval ][ i ] = ( unsigned short ) (hpos + i < alloc_len ? segment[ hpos + i ] : 0);
                        if ( qtables[ rval ][ i ] == 0 ) break;
                    }
                    hpos += 64;
                }
                else { // 16 bit precision
                    for ( i = 0; i < 64; i++ ) {
                        qtables[ rval ][ i ] =
                            B_SHORT( (hpos + (2*i)< alloc_len ? segment[ hpos + (2*i) ] : 0), (hpos + 2*i+1 < alloc_len?segment[ hpos + (2*i) + 1 ]:0) );
                        if ( qtables[ rval ][ i ] == 0 ) break;
                    }
                    hpos += 128;
                }
            }

            if ( hpos != len ) {
                // if we get here, something went wrong
                fprintf( stderr, "size mismatch in dqt marker" );
                errorlevel.store(2);
                return false;
            }
            return true;

        case 0xDD: // DRI segment
            // define restart interval
          rsti = B_SHORT( hpos < alloc_len ? segment[ hpos ]:0, hpos +1 < alloc_len ?segment[ hpos + 1 ]:0 );
            return true;

        case 0xDA: // SOS segment
            // prepare next scan
            cs_cmpc = hpos < alloc_len ? segment[ hpos ] : 0;
            if ( cs_cmpc > cmpc ) {
                fprintf( stderr, "%i components in scan, only %i are allowed",
                            cs_cmpc, cmpc );
                errorlevel.store(2);
                return false;
            }
            hpos++;
            for ( i = 0; i < cs_cmpc; i++ ) {
                for ( cmp = 0; ( (hpos < alloc_len ? segment[ hpos ]:0) != cmpnfo[ cmp ].jid ) && ( cmp < cmpc ); cmp++ );
                if ( cmp == cmpc ) {
                    fprintf( stderr, "component id mismatch in start-of-scan" );
                    errorlevel.store(2);
                    return false;
                }
                cs_cmp[ i ] = cmp;
                cmpnfo[ cmp ].huffdc = LBITS( hpos + 1< alloc_len ? segment[ hpos + 1 ]:0, 4 );
                cmpnfo[ cmp ].huffac = RBITS( hpos + 1 < alloc_len ? segment[ hpos + 1 ]:0, 4 );
                if ( ( cmpnfo[ cmp ].huffdc < 0 ) || ( cmpnfo[ cmp ].huffdc >= 4 ) ||
                     ( cmpnfo[ cmp ].huffac < 0 ) || ( cmpnfo[ cmp ].huffac >= 4 ) ) {
                    fprintf( stderr, "huffman table number mismatch" );
                    errorlevel.store(2);
                    return false;
                }
                hpos += 2;
            }
            cs_from = hpos < alloc_len ? segment[ hpos + 0 ]:0;
            cs_to   = hpos + 1 < alloc_len ? segment[ hpos + 1 ]:0;
            cs_sah  = LBITS( hpos + 2 < alloc_len ? segment[ hpos + 2 ]:0, 4 );
            cs_sal  = RBITS( hpos +2  <  alloc_len ? segment[ hpos + 2 ]:0, 4 );
            // check for errors
            if ( ( cs_from > cs_to ) || ( cs_from > 63 ) || ( cs_to > 63 ) ) {
                fprintf( stderr, "spectral selection parameter out of range" );
                errorlevel.store(2);
                return false;
            }
            if ( ( cs_sah >= 12 ) || ( cs_sal >= 12 ) ) {
                fprintf( stderr, "successive approximation parameter out of range" );
                errorlevel.store(2);
                return false;
            }
            return true;

        case 0xC0: // SOF0 segment
            // coding process: baseline DCT

        case 0xC1: // SOF1 segment
            // coding process: extended sequential DCT

        case 0xC2: // SOF2 segment
            // coding process: progressive DCT

            // set JPEG coding type
            if ( type == 0xC2 )
                jpegtype = 2;
            else
                jpegtype = 1;

            // check data precision, only 8 bit is allowed
            lval = hpos < alloc_len ? segment[ hpos ]:0;
            if ( lval != 8 ) {
                fprintf( stderr, "%i bit data precision is not supported", lval );
                errorlevel.store(2);
                return false;
            }

            // image size, height & component count
            imgheight = B_SHORT( hpos +  1  < alloc_len ? segment[ hpos + 1 ]:0, hpos +  2 < alloc_len ?segment[ hpos + 2 ] :0);
            imgwidth  = B_SHORT( hpos + 3 < alloc_len ?segment[ hpos + 3 ]:0, hpos + 4 < alloc_len ?segment[ hpos + 4 ]:0 );
            cmpc      = hpos + 5 < alloc_len ?  segment[ hpos + 5 ]:0;
            if ( cmpc > 4 ) {
                cmpc = 4;
                fprintf( stderr, "image has %i components, max 4 are supported", cmpc );
                errorlevel.store(2);
                return false;
            }
            hpos += 6;
            // components contained in image
            for ( cmp = 0; cmp < cmpc; cmp++ ) {
                cmpnfo[ cmp ].jid = hpos < alloc_len ? segment[ hpos ]:0;
                cmpnfo[ cmp ].sfv = LBITS( hpos + 1 < alloc_len ? segment[ hpos + 1 ]:0, 4 );
                cmpnfo[ cmp ].sfh = RBITS( hpos + 1 < alloc_len ? segment[ hpos + 1 ]:0, 4 );
                if (cmpnfo[ cmp ].sfv > 4
                    || cmpnfo[ cmp ].sfh > 4) {
                    custom_exit(ExitCode::SAMPLING_BEYOND_FOUR_UNSUPPORTED);
                }
#ifndef ALLOW_3_OR_4_SCALING_FACTOR
                if (cmpnfo[ cmp ].sfv > 2
                    || cmpnfo[ cmp ].sfh > 2) {
                    custom_exit(ExitCode::SAMPLING_BEYOND_TWO_UNSUPPORTED);
                }
#endif
                uint32_t quantization_table_value = hpos + 2 < alloc_len ? segment[ hpos + 2 ]:0;
                if (quantization_table_value >= qtables.size()) {
                    errorlevel.store(2);
                    return false;
                }
                cmpnfo[ cmp ].qtable = qtables[quantization_table_value].begin();
                hpos += 3;
            }
    
            return true;

        case 0xC3: // SOF3 segment
            // coding process: lossless sequential
            fprintf( stderr, "sof3 marker found, image is coded lossless" );
            errorlevel.store(2);
            return false;

        case 0xC5: // SOF5 segment
            // coding process: differential sequential DCT
            fprintf( stderr, "sof5 marker found, image is coded diff. sequential" );
            errorlevel.store(2);
            return false;

        case 0xC6: // SOF6 segment
            // coding process: differential progressive DCT
            fprintf( stderr, "sof6 marker found, image is coded diff. progressive" );
            errorlevel.store(2);
            return false;

        case 0xC7: // SOF7 segment
            // coding process: differential lossless
            fprintf( stderr, "sof7 marker found, image is coded diff. lossless" );
            errorlevel.store(2);
            return false;
    
        case 0xC9: // SOF9 segment
            // coding process: arithmetic extended sequential DCT
            fprintf( stderr, "sof9 marker found, image is coded arithm. sequential" );
            errorlevel.store(2);
            return false;
    
        case 0xCA: // SOF10 segment
            // coding process: arithmetic extended sequential DCT
            fprintf( stderr, "sof10 marker found, image is coded arithm. progressive" );
            errorlevel.store(2);
            return false;
    
        case 0xCB: // SOF11 segment
            // coding process: arithmetic extended sequential DCT
            fprintf( stderr, "sof11 marker found, image is coded arithm. lossless" );
            errorlevel.store(2);
            return false;
    
        case 0xCD: // SOF13 segment
            // coding process: arithmetic differntial sequential DCT
            fprintf( stderr, "sof13 marker found, image is coded arithm. diff. sequential" );
            errorlevel.store(2);
            return false;
    
        case 0xCE: // SOF14 segment
            // coding process: arithmetic differential progressive DCT
            fprintf( stderr, "sof14 marker found, image is coded arithm. diff. progressive" );
            errorlevel.store(2);
            return false;

        case 0xCF: // SOF15 segment
            // coding process: arithmetic differntial lossless
            fprintf( stderr, "sof15 marker found, image is coded arithm. diff. lossless" );
            errorlevel.store(2);
            return false;
    
        case 0xE0: // APP0 segment
        case 0xE1: // APP1 segment
        case 0xE2: // APP2 segment
        case 0xE3: // APP3 segment
        case 0xE4: // APP4 segment
        case 0xE5: // APP5 segment
        case 0xE6: // APP6 segment
        case 0xE7: // APP7 segment
        case 0xE8: // APP8 segment
        case 0xE9: // APP9 segment
        case 0xEA: // APP10 segment
        case 0xEB: // APP11 segment
        case 0xEC: // APP12segment
        case 0xED: // APP13 segment
        case 0xEE: // APP14 segment
        case 0xEF: // APP15 segment
        case 0xFE: // COM segment
            // do nothing - return true
            return true;
    
        case 0xD0: // RST0 segment
        case 0xD1: // RST1segment
        case 0xD2: // RST2 segment
        case 0xD3: // RST3 segment
        case 0xD4: // RST4 segment
        case 0xD5: // RST5 segment
        case 0xD6: // RST6 segment
        case 0xD7: // RST7 segment
            // return errormessage - RST is out of place here
            fprintf( stderr, "rst marker found out of place" );
            errorlevel.store(2);
            return false;

        case 0xD8: // SOI segment
            // return errormessage - start-of-image is out of place here
            fprintf( stderr, "soi marker found out of place" );
            errorlevel.store(2);
            return false;

        case 0xD9: // EOI segment
            // return errormessage - end-of-image is out of place here
            fprintf( stderr, "eoi marker found out of place" );
            errorlevel.store(2);
            return false;
    
        default: // unknown marker segment
            // return warning
            fprintf( stderr, "unknown marker found: FF %2X", type );
            errorlevel.store(1);
            return true;
    }
}