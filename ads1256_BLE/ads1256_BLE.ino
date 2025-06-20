/*
  Bibliotecas
    - Nimble by h2zero
    - BleGamepad by lemmingDev
    - ADS1256 by curiousscientist
*/
#include <ADS1256.h>
#include <BleGamepad.h>

/*
  Load Cell - Extension
  Green     - Green
  White     - Yellow
  Black     - Brown
  Red       - Red
*/
long  mapidimap(long long x, long long in_min, long long in_max, long long out_min, long long out_max) {
  unsigned long long calc1 = (x - in_min) * (out_max - out_min);
  unsigned long long calc2 = calc1 / (in_max - in_min);
  return calc2;
}

ADS1256 A(16, 25, 0, 5, 2.5); //DRDY, RESET, SYNC(PDWN), CS, VREF(float).   //ESP32 WROOM 32
BleGamepad bleGamepad ("Pedals BLE Gamepad", "GFY", 100);

void setup() {
  Serial.begin(115200); //The value does not matter if you use an MCU with native USB
  A.InitializeADC(); //See the documentation for every details

  A.setPGA(PGA_64);
  //--------------------------------------------

  //Set input channels
  A.setMUX(DIFF_6_7);
  //--------------------------------------------

  //Set DRATE
  A.setDRATE(DRATE_500SPS);
  delay(100);
  bleGamepad.begin();
}

void loop() {
  long VrxValue = 0;
  long VryValue = 0;
  long VlxValue = 0;
  long test = 0;
  double dChannels[4];

  if (bleGamepad.isConnected()) {
    // put your main code here, to run repeatedly:
    for (int j = 0; j < 4; j++)
    {
      dChannels[j] = A.cycleDifferential();
    }
    for (int i = 0; i < 4; i++)
    {
      Serial.print(dChannels[i], 4);
      if(i < 3)
	  	{
	    	Serial.print("\t");
	  	}
    }
    VlxValue = mapidimap(dChannels[0], 125000, 650000, 0, 32737);   //Acelerador
    VrxValue = mapidimap(dChannels[1], 5000, 220000, 0, 32737);     //Freio
    VryValue = mapidimap(dChannels[2], -12000, 1600000, 0, 32737);  //Embreagem
    

    bleGamepad.setLeftThumb(VlxValue, 0);
    bleGamepad.setRightThumb(VrxValue, VryValue);
    Serial.println();
  }
}
