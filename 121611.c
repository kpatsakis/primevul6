bool build_huffcodes( unsigned char *clen, uint32_t clenlen, unsigned char *cval, uint32_t cvallen, huffCodes *hc, huffTree *ht )
{
    int nextfree;
    int code;
    int node;
    int i, j, k;


    // fill with zeroes
    memset( hc->clen, 0, 256 * sizeof( short ) );
    memset( hc->cval, 0, 256 * sizeof( short ) );
    memset( ht->l, 0, 256 * sizeof( short ) );
    memset( ht->r, 0, 256 * sizeof( short ) );

    // 1st part -> build huffman codes

    // creating huffman-codes
    k = 0;
    code = 0;

    // symbol-value of code is its position in the table
    for( i = 0; i < 16; i++ ) {
        uint32_t clen_index = i & 0xff;
        for( j = 0; j < (int) (clen_index < clenlen ? clen[clen_index] : 0); j++ ) {
            uint32_t cval_index = k&0xff;
            uint8_t cval_val= cval_index < cvallen ? cval[cval_index] : 0;
            hc->clen[ (int) cval_val&0xff] = 1 + i;
            hc->cval[ (int) cval_val&0xff] = code;

            k++;
            code++;
        }
        code = code << 1;
    }

    // find out eobrun max value
    hc->max_eobrun = 0;
    for ( i = 14; i >= 0; i-- ) {
        if ( hc->clen[(i << 4) & 255] > 0 ) {
            hc->max_eobrun = ( 2 << i ) - 1;
            break;
        }
    }

    // 2nd -> part use codes to build the coding tree

    // initial value for next free place
    nextfree = 1;
    const char * huffman_no_space = "Huffman table out of space\n";
    // work through every code creating links between the nodes (represented through ints)
    for ( i = 0; i < 256; i++ )    {
        // (re)set current node
        node = 0;
        // go through each code & store path
        for ( j = hc->clen[i] - 1; j > 0; j-- ) {
            if (node <= 0xff) {
                if ( BITN( hc->cval[i], j ) == 1 ) {
                    if ( ht->r[node] == 0 ) {
                         ht->r[node] = nextfree++;
                    }
                    node = ht->r[node];
                }
                else {
                    if ( ht->l[node] == 0 ) {
                        ht->l[node] = nextfree++;
                    }
                    node = ht->l[node];
                }
            } else {
                while(write(2, huffman_no_space, strlen(huffman_no_space)) == -1 && errno == EINTR) {}
                if (filetype == JPEG) {
                    return false;
                }
            }
        }
        if (node <= 0xff) {
            // last link is number of targetvalue + 256
            if ( hc->clen[i] > 0 ) {
                if ( BITN( hc->cval[i], 0 ) == 1 ) {
                    ht->r[node] = i + 256;
                } else {
                    ht->l[node] = i + 256;
                }
            }
        } else {
            while(write(2, huffman_no_space, strlen(huffman_no_space)) == -1 && errno == EINTR) {}
            if (filetype == JPEG) {
                return false; // we accept any .lep file that was encoded this way
            }
        }
    }
    return true;
}