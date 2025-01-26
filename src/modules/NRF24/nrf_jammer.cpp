#include "nrf_common.h"
#include "nrf_jammer.h"
#include <globals.h>
#include "core/mykeyboard.h"
#include "core/display.h"

/* **************************************************************************************
** name : nrf_jammer
** details : Starts 2.4Gz jammer usinf NRF24
************************************************************************************** */
void nrf_jammer() {
  #if defined(NRF24_CE_PIN) && defined(NRF24_SS_PIN) && defined(USE_NRF24_VIA_SPI)
    RF24 radio(NRF24_CE_PIN, NRF24_SS_PIN);                                                               ///ce-csn
    byte hopping_channel[] = {32,34, 46,48, 50, 52, 0, 1, 2, 4, 6, 8, 22, 24, 26, 28, 30, 74, 76, 78, 80, 82, 84,86 };  // channel to hop
    byte ptr_hop = 0;  // Pointer to the hopping array
    if(nrf_start())
    {
        Serial.println("NRF24 turned On");
        
        NRFradio.setAutoAck(false);
        NRFradio.stopListening();
        NRFradio.setRetries(0, 0);
        NRFradio.setPayloadSize(5);
        NRFradio.setAddressWidth(3);
        NRFradio.setPALevel(RF24_PA_MAX, true);
        NRFradio.setDataRate(RF24_2MBPS);
        NRFradio.setCRCLength(RF24_CRC_DISABLED);
        NRFradio.printPrettyDetails();
        NRFradio.startConstCarrier(RF24_PA_MAX, hopping_channel[ptr_hop]);


        drawMainBorder();
        tft.setCursor(10,28);
        tft.setTextSize(FM);
        tft.println("BLE Jammer:");
        tft.setCursor(10,tft.getCursorY()+8);
        tft.println("Select to stop!");
        delay(200);

        while(!check(SelPress)) {
            ptr_hop++;                                            /// perform next channel change
            if (ptr_hop >= sizeof(hopping_channel)) ptr_hop = 0;  // To avoid array indexing overflow
            NRFradio.setChannel(hopping_channel[ptr_hop]);           // Change channel        
            delayMicroseconds(10);
        }
      NRFradio.stopConstCarrier();//this will stop jamming without powering down nrf
        //NRFradio.powerDown(); power down without powering up on scanner
    } else { 
        Serial.println("Fail Starting radio");
        displayError("NRF24 not found");
        delay(500);
    }
  #endif
}

void nrf_jammer_all_ch() {
  #if defined(NRF24_CE_PIN) && defined(NRF24_SS_PIN) && defined(USE_NRF24_VIA_SPI)
    RF24 radio(NRF24_CE_PIN, NRF24_SS_PIN);                                                               ///ce-csn
    byte hopping_channel[126];  // NRF24 has channels 0-125
    for (int i = 0; i <= 125; i++) {
      hopping_channel[i] = i;
    }

    byte ptr_hop = 0;  // Pointer to the hopping array
    if(nrf_start())
    {
        Serial.println("NRF24 turned On");
        
        NRFradio.setAutoAck(false);
        NRFradio.stopListening();
        NRFradio.setRetries(0, 0);
        NRFradio.setPayloadSize(5);
        NRFradio.setAddressWidth(3);
        NRFradio.setPALevel(RF24_PA_MAX, true);
        NRFradio.setDataRate(RF24_2MBPS);
        NRFradio.setCRCLength(RF24_CRC_DISABLED);
        NRFradio.printPrettyDetails();
        NRFradio.startConstCarrier(RF24_PA_MAX, hopping_channel[ptr_hop]);


        drawMainBorder();
        tft.setCursor(10,28);
        tft.setTextSize(FM);
        tft.println("All CH (1-125) Jammer:");
        tft.setCursor(10,tft.getCursorY()+8);
        tft.println("Select to stop!");
        delay(200);

        while(!check(SelPress)) {
            // Hop through channels more efficiently with a longer dwell time
            ptr_hop++;                                           
            if (ptr_hop >= sizeof(hopping_channel)/sizeof(byte)) ptr_hop = 0;  // Correct array size calculation
            NRFradio.setChannel(hopping_channel[ptr_hop]);
            
            // Dwell longer on each channel for better interference
            delayMicroseconds(100);
            
            // Optional: Add random dwell time for unpredictable jamming pattern
            // delayMicroseconds(random(50, 150));
        }
      NRFradio.stopConstCarrier();//this will stop jamming without powering down nrf
        //NRFradio.powerDown(); power down without powering up on scanner
    } else { 
        Serial.println("Fail Starting radio");
        displayError("NRF24 not found");
        delay(500);
    }
  #endif
}