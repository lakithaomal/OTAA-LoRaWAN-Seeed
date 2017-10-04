
#include "LoRaWan.h"


unsigned char data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA,};
char buffer[256];

int loopcount = 0;

void setup(void)
{
    int i;
    
    SerialUSB.begin(115200);
    while(!SerialUSB);
    
    lora.init();
    lora.setDeviceReset();
    
    memset(buffer, 0, 256);
    lora.getVersion(buffer, 256, 1);
    SerialUSB.print(buffer); 
    
    memset(buffer, 0, 256);
    lora.getId(buffer, 256, 1);
    SerialUSB.print(buffer);

    lora.setId(NULL, NULL, "70b3d57ed0006fcc");      // set App key, from TTN device overview

    // ???
    lora.setKey("039CBB775BFF1AC59FFFBCA9BC5DEC52",     // NetSKEY
                "9D3F0252CA11CC6920F36DE215748093",     // AppSKEY
                "ce3889feb09fa4b536ee9c41ba863364");    // AppKey
    
    if (lora.setDeviceMode(LWOTAA) == false)               // Over The Air Activation
      SerialUSB.print("Set Mode to OTAA failed.\n");
    else
      SerialUSB.print("OTAA mode set.\n");
      
//    lora.setDeciveMode(LWABP);              // pre-shared keys
    lora.setDataRate(DR0, US915);
    lora.setAdaptiveDataRate(true); 
    lora.setReceiveWindowFirst(true);
    

#ifdef DEAD1
    lora.setChannel(0, 903.9);
    lora.setChannel(1, 904.1);
    lora.setChannel(2, 904.3);
    lora.setChannel(3, 904.5);
    lora.setChannel(4, 904.7);
    lora.setChannel(5, 904.9);
    lora.setChannel(6, 905.3);
  
    lora.setReceiceWindowFirst(1);                  // enable downstream channels
    lora.setReceiceWindowFirst(0, 923.3);      // set frquecies. From https://www.thethingsnetwork.org/wiki/LoRaWAN/Frequencies/Frequency-Plans
    lora.setReceiceWindowFirst(1, 923.9);      
    lora.setReceiceWindowFirst(2, 924.5);
    lora.setReceiceWindowFirst(3, 925.1);
    lora.setReceiceWindowFirst(4, 925.7);
    lora.setReceiceWindowFirst(5, 926.3);
    lora.setReceiceWindowFirst(6, 927.5);
    
    lora.setReceiceWindowSecond(923.3, DR0);

#elif DEAD2

    for (i=0; i < 16; i++)          // should be 72
      lora.setChannel(i, 902.3 + ((float)i) * 0.2);
  
    lora.setReceiceWindowFirst(1);                  // enable downstream channels
    for (i=0; i<16; i+=8) {
      lora.setReceiceWindowFirst(i+0, 923.3);      // set frquecies. 2.2.2
      lora.setReceiceWindowFirst(i+1, 923.9);      
      lora.setReceiceWindowFirst(i+2, 924.5);
      lora.setReceiceWindowFirst(i+3, 925.1);
      lora.setReceiceWindowFirst(i+4, 925.7);
      lora.setReceiceWindowFirst(i+5, 926.3);
      lora.setReceiceWindowFirst(i+6, 926.9);
      lora.setReceiceWindowFirst(i+7, 927.5);
    }
    
    lora.setReceiceWindowSecond(923.3, DR8);      // 2.2.7

#endif 

    lora.setReceiveWindowSecond(923.3, DR8);      // 2.2.7
    lora.setPower(14);

//    lora.loraDebug();
    SerialUSB.print("Starting OTTA Join.\n");
    loopcount = 0;
    while(true) {
      loopcount++;
      if (lora.setOTAAJoin(JOIN))
        break;
    }

    SerialUSB.print("Took ");
    SerialUSB.print(loopcount);
    SerialUSB.println(" tries to join.");
    
    if (lora.transferPacket("Start!", 1) == false) {
      SerialUSB.print("packet transmit failed.\n");
    }

    if(SerialUSB.available()) {
      SerialUSB.print("--Entering Debug--\n");
      lora.loraDebug();
      SerialUSB.print("--Exit Debug--\n");
    }
  
  loopcount = 0;
}

void loop(void)
{   
    bool result = false;

    if(SerialUSB.available()) {
      SerialUSB.print("--Entering Debug--\n");
      lora.loraDebug();
      SerialUSB.print("--Exit Debug--\n");
    }

    //result = lora.transferPacket("Hello W!", 10);
    data[5]++;
    result = lora.transferPacket(data, 6, 10);
    
    if(result)
    {
        short length;
        short rssi;
        
        memset(buffer, 0, 256);
        length = lora.receivePacket(buffer, 256, &rssi);
        
        if(length)
        {
            SerialUSB.print("Length is: ");
            SerialUSB.println(length);
            SerialUSB.print("RSSI is: ");
            SerialUSB.println(rssi);
            SerialUSB.print("Data is: ");
            for(unsigned char i = 0; i < length; i ++)
            {
                SerialUSB.print("0x");
                SerialUSB.print(buffer[i], HEX);
                SerialUSB.print(" ");
            }
            SerialUSB.println();
        }
    }
}


