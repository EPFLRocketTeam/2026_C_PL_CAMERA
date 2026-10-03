
#include <stdint.h>
#include <stddef.h>

namespace platform {

    void initPlatform ();
    bool isManualMode ();
    float measureCurrent (uint8_t rep);

    uint32_t currentTime ();
    void setLED (bool enabled);

    namespace camera {
        bool isPowerOn ();
        void powerOn (bool isFirst);
        void powerOff ();
        void setupPowerOnBase ();

        bool isRecording ();
        void startRecording (bool isFirst);
        void stopRecording (bool isFirst);

        void initUart ();
        void sendUart (const uint8_t *data, size_t len);
    };

    namespace can {
        void init_can ();

        void try_receive ();
        void send_health ();
    };

};
