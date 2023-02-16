#ifndef _ARTEMIS_CHANNELS_H
#define _ARTEMIS_CHANNELS_H

#include "config/artemis_defs.h"

namespace Artemis
{
    namespace Teensy
    {
        namespace Channels
        {
            // Channel IDs
            enum Channel_ID : uint8_t
            {
                RFM23_CHANNEL = 1,
                PDU_CHANNEL,
                RPI_CHANNEL,
            };

            void rfm23_channel();
            void pdu_channel();
            void rpi_channel();
        }
    }
}

#endif // _ARTEMIS_CHANNELS_H