parseOptions(CotpConnection* self, uint8_t* buffer, int bufLen)
{
    int bufPos = 0;

    while (bufPos < bufLen) {
        uint8_t optionType = buffer[bufPos++];
        uint8_t optionLen = buffer[bufPos++];

        if (optionLen > (bufLen - bufPos)) {
            if (DEBUG_COTP)
                printf("COTP: option to long optionLen:%i bufPos:%i bufLen:%i\n", optionLen, bufPos, bufLen);
            goto cpo_error;
        }

        if (DEBUG_COTP)
            printf("COTP: option: %02x len: %02x\n", optionType, optionLen);

        switch (optionType) {
        case 0xc0:
			if (optionLen == 1) {
				int requestedTpduSize = (1 << buffer[bufPos++]);

                CotpConnection_setTpduSize(self, requestedTpduSize);

				if (DEBUG_COTP)
				    printf("COTP: requested TPDU size: %i\n", requestedTpduSize);
			}
			else
			    goto cpo_error;
            break;

        case 0xc1: /* remote T-selector */
            if (optionLen < 5) {
                self->options.tSelSrc.size = optionLen;

                int i;
                for (i = 0; i < optionLen; i++)
                    self->options.tSelSrc.value[i] = buffer[bufPos++];
            }
            else
                goto cpo_error;
            break;

        case 0xc2: /* local T-selector */
            if (optionLen < 5) {
                self->options.tSelDst.size = optionLen;

                int i;
                for (i = 0; i < optionLen; i++)
                    self->options.tSelDst.value[i] = buffer[bufPos++];
            }
            else
                goto cpo_error;
            break;

        case 0xc6: /* additional option selection */
            if (optionLen == 1)
                bufPos++; /* ignore value */
            else
                goto cpo_error;
            break;

        default:
            if (DEBUG_COTP)
                printf("COTP: Unknown option %02x\n", optionType);

            bufPos += optionLen; /* ignore value */

            break;
        }
    }

    return true;

cpo_error:
    if (DEBUG_COTP)
        printf("COTP: cotp_parse_options: error parsing options!\n");
    return false;
}