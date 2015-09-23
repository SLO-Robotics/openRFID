/*
 * openRFID: This is a simple was to control a pin via RFID tags that are listed within the program/code.  
 *
 * RFID CARDS: No uses of SD card, No use of internet. It's all in the code.
 * PRO'S: Makes it a very easy and simple circuit/setup. Update card list by editing code and uploading to your Arduino
 * CON'S: This can make it harder to add new cards or remove as that you must edit the code and upload to the Arduino each time
 *
 * By SLO Robotics: http://www.slorobotics.com/projects/openRFID (See site for gitHub link and video/build details)
 * Version: 1 (09/23/2015)
 * Authors: Shane Weddle
 * Other RFID projects: 
 * - openRFID-CF: Gets UID's(card ID's) from a CF card. easier to update. 
 * - openRFID-NET: Mange access list of UID's(card ID's) via a web site, you can host or use our SAAS.
 * - openRFID-USB: Mange access list of UID's(card ID's) via a USB on a PC/MAC
 *
 *
 * ----------------------------------------------------------------------------
 * Library used: MFRC522
 * Library link: https://github.com/miguelbalboa/rfid
 * - lots of useful info in the MFRC522.h read it to understand more.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno           Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS(NSS) SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 */

//#### ADD/REMOVE RFID UID'S in the string below    #####
//#### Separate UID's with ,                        #####
//#### The UID fromats can have : or not            #####
//#### The first item in the string must be "START" #####
//#### The last item in the string must be "END"    #####
String CardList = "START,132D3000,03:6e:34:00,a3:bd:30:00,83:8e:96:00,33:be:31:00,03:35:29:00,a3:8c:2b:00,END";
//#### ADD/REMOVE RFID UID'S in the string ABOVE #####

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN		9		 
#define SS_PIN		10		

MFRC522 mfrc522(SS_PIN, RST_PIN);	// Create MFRC522 instance
int loopcnt = 0;
String strlenx;

String compx;
int successRead;		// Variable integer to keep if we have Successful Read from Reader
 String UIDx;
     String uidCHR;
void setup() {
  //#### Open door latch pin ####  
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);
  
	Serial.begin(9600);		// Initialize serial communications with PC
	while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	ShowReaderDetails();	        // Show details of PCD - MFRC522 Card Reader details
	Serial.println(F("Scan PICC to see UID, type, and data blocks..."));
}

void loop() {
	// Look for new cards
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
  
		return;
	}

	// Select one of the cards
	if ( ! mfrc522.PICC_ReadCardSerial()) {

		return;
	}

//#### Start checking Cards ####
for (byte i = 0; i < mfrc522.uid.size; i++) {
   String uidCHR(mfrc522.uid.uidByte[i], HEX); 
    if (uidCHR.length() < 2) {
    uidCHR = "00";
    } 
             
  UIDx = UIDx + uidCHR;
        } 
           Serial.println();
             UIDx.toUpperCase();
              
        Serial.println(UIDx);
        getID();
       //Serial.println();
        Serial.println("Done looking.");
       // Serial.println();
        UIDx = "";
        loopcnt=0;
    delay(5000);
        
        }

//#### Get PICC's UID - CHECK UID FOR MATCH #### 
int getID() {
  

  while (loopcnt < 501) {
   loopcnt++;
   String approvedUID = getValue(CardList, ',', loopcnt);
      if (approvedUID == "END"){    
      Serial.println("NO MATCH");
        }else{
         
         approvedUID.replace(":", "");
            if (approvedUID == UIDx){
            Serial.println("Match");
// #### Open/unlock door for 2 sec ####
            digitalWrite(3, HIGH);
            delay(2000);
            digitalWrite(3, LOW);
// #### Put loopcnt (loop Count) over its max so that it does not keep looking for a match (as that we have one now)
            loopcnt=502;
            }
        }
    }
 return 1;
}
//#### END - Get PICC's UID / CHECK UID FOR MATCH #### 



//#### Split on , SUB/Helper ####
 String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

//#### END - Split on , SUB/Helper ####


void ShowReaderDetails() {
	// Get the MFRC522 software version
	byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
	Serial.print(F("MFRC522 Software Version: 0x"));
	Serial.print(v, HEX);
	if (v == 0x91)
		Serial.print(F(" = v1.0"));
	else if (v == 0x92)
		Serial.print(F(" = v2.0"));
	else
		Serial.print(F(" (unknown)"));
	Serial.println("");
	// When 0x00 or 0xFF is returned, communication probably failed
	if ((v == 0x00) || (v == 0xFF)) {
		Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
	}
}
