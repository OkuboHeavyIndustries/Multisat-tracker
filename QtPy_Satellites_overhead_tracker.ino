
#include <Arduino.h>
#include <U8g2lib.h>
#include <TinyGPS++.h> 
#include <ArduinoP13.h>
#include <Time.h>
#include <SPI.h>
#include <SD.h>

Sd2Card card;
SdVolume volume;
SdFile root;
File myFile;

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel pixels(1, PIN_NEOPIXEL);

//char *tleName = "ISS";
//char *tlel1   = "1 25544U 98067A   23106.77243927  .00020111  00000+0  36079-3 0  9995";
//char *tlel2   = "2 25544  51.6392 272.8882 0005955 200.6729 293.6537 15.49882047392260";

char tleName [30];
char tlel1 [80];
char tlel2 [80];

const char  *pcMyName = "OHI";    // Observer name


double       dfreqRX  = 137.100;     // Nominal downlink frequency
double       dfreqTX  = 437.800;     // Nominal uplink frequency


double       dSatLAT  = 0;           // Satellite latitude
double       dSatLON  = 0;           // Satellite longitude
double       dSatAZ   = 0;           // Satellite azimuth
double       dSatEL   = 0;           // Satellite elevation

double       dSatLATb  = 0;           // Satellite latitude
double       dSatLONb  = 0;           // Satellite longitude
double       dSatAZb   = 0;           // Satellite azimuth
double       dSatELb   = 0;           // Satellite elevation


double       dSunLAT  = 0;           // Sun latitude
double       dSunLON  = 0;           // Sun longitude
double       dSunAZ   = 0;           // Sun azimuth
double       dSunEL   = 0;           // Sun elevation

char         acBuffer[20];            // Buffer for ASCII time

String tlenamestring;
String tle1string;
String tle2string;

int tlenamelength;

static const uint32_t GPSBaud = 9600;

#define U8LOG_WIDTH 28
#define U8LOG_HEIGHT 7
uint8_t u8log_buffer[U8LOG_WIDTH*U8LOG_HEIGHT*10];
U8G2LOG u8g2log;

TinyGPSPlus gps;

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

int gpschar;

int satsabove=0;
int sats=0;
int satsdisplay = 0;
int satsabovedisplay = 0;

void setup(void) {

  Serial1.begin(GPSBaud);
  Serial.begin(GPSBaud); //uncomment to debug
  u8g2.begin();  
  u8g2log.begin(U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);
  u8g2log.setLineHeightOffset(1); // set extra space between lines in pixel, this can be negative
  u8g2log.setRedrawMode(1);   // 0: Update screen with newline, 1: Update screen for every char   
  
  pixels.begin();  // initialize the neopixel  

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");

  if (!SD.begin(0)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
 

  // open the file for reading:
  myFile = SD.open("tle.txt");

     
    
    // close the file: //don't!
  //  myFile.close();

}

void loop(void) {

  u8g2.firstPage();
  do {

    u8g2.setFont(u8g2_font_u8glib_4_tr);    // u8g2 font 
    
  //check file is open
  if (myFile.available()){
   // Serial.println("file open");
  }
  // if not, open it
  else{
       satsdisplay = sats;
       satsabovedisplay = satsabove;
       myFile = SD.open("tle.txt");
       Serial.println("\n");
       Serial.println("Starting recheck");
       Serial.println("\n");
       u8g2log.print("\n");
       u8g2log.println("Starting recheck");
       u8g2log.print("\n");
       sats=0;
       satsabove=0;
       }
  
  
    tlenamestring = myFile.readStringUntil('\n');
    tlenamestring.trim();
      tlenamelength = tlenamestring.length();
    tle1string = myFile.readStringUntil('\n');  
    tle2string = myFile.readStringUntil('\n');
    tlenamestring.toCharArray(tleName, tlenamestring.length()+1); 
    tle1string.toCharArray(tlel1, tle1string.length()+1); 
    tle2string.toCharArray(tlel2, tle2string.length()+1); 

    sats = sats +1;
    
    Serial.println(sats);
    
    
    
     //Serial.println(tleName);
    // Serial.println(tlel1);
    // Serial.println(tlel2);
  
  
  while (Serial1.available()>0)
      {  gpschar = Serial1.read(); //read raw gps data to gpschar
       // Serial.write(gpschar);  // uncomment to send raw gps over Serial to debug
      
        gps.encode(gpschar);      // extract useful info from raw gps data
      }
  
     double lat_val, lng_val, alt_m_val;
     uint8_t hr_val, min_val, sec_val, hr_val_jp, sats_val;
     bool loc_valid, alt_valid, time_valid, sats_valid;
     lat_val = gps.location.lat();  /* Get latitude data */
     loc_valid = gps.location.isValid(); /* Check if valid location data is available */
     lng_val = gps.location.lng(); /* Get longtitude data */
     alt_m_val = gps.altitude.meters();  /* Get altitude data in meters */
     alt_valid = gps.altitude.isValid(); /* Check if valid altitude data is available */
     hr_val = gps.time.hour(); /* Get hour */
     min_val = gps.time.minute();  /* Get minutes */
     sec_val = gps.time.second();  /* Get seconds */
     time_valid = gps.time.isValid();  /* Check if valid time data is available */
      double year_val = (gps.date.year());
      double month_val = (gps.date.month());
      double day_val = (gps.date.day());
     sats_valid = gps.satellites.isValid();
     sats_val = gps.satellites.value();

     if (hr_val < 15) { hr_val_jp = hr_val + 9;  // convert UTC to Japan time zone
     }
     else {hr_val_jp = hr_val -15;
     }


      double dMyLAT = gps.location.lat();
      double dMyLON = gps.location.lng();
      double dMyALT = gps.altitude.meters();
        
        //P13Sun Sun;                                                       // Create object for the sun
        P13DateTime MyTime(year_val, month_val, day_val, hr_val, min_val, sec_val); // Set start time for the prediction
        P13Observer MyQTH(pcMyName, dMyLAT, dMyLON, dMyALT);              // Set observer coordinates

        P13Satellite MySAT(tleName, tlel1, tlel2);                        // Create Sattelite data from TLE

        


        MyTime.ascii(acBuffer);             // Get time for prediction as ASCII string
        MySAT.predict(MyTime);              // Predict Sattelite for specific time
        MySAT.latlon(dSatLAT, dSatLON);     // Get the rectangular coordinates
        MySAT.elaz(MyQTH, dSatEL, dSatAZ);  // Get azimut and elevation for MyQTH

      if (dSatEL > 15) {
      satsabove = satsabove+1;
      u8g2log.println(tleName);
      Serial.println(tleName);
      
    
      Serial.print("sats above = ");
      Serial.println(satsabove);
      Serial.println("\n");
        
      //u8g2log.print("\n");
      }

    u8g2.drawFrame(0, 0, 128,64);  //setup fixed screen info and borders
    u8g2.drawLine(0, 9, 128,9);
   // u8g2.drawLine(0, 27, 128,27);
    u8g2.drawStr(2, 7, "OVERHEAD SATS");
    u8g2.drawLine(0, 54, 128,54);
    u8g2.drawStr(2, 61, "OKUBO HEAVY INDUSTRIES");

    u8g2.drawLine(104, 9, 104,54);
    u8g2.drawLine(105, 25, 127,25);
    u8g2.drawLine(105, 40, 127,40);
    u8g2.drawStr(107, 16, ">15");
    u8g2.drawCircle(119,13,1);
    u8g2.drawStr(107, 31, "active");
    u8g2.drawStr(107, 46, "check");
    

      u8g2.setCursor(107, 23);
    if (satsabovedisplay < 1){
      u8g2.println(satsabove); 
    } else {
      u8g2.println(satsabovedisplay);
    }
   
      u8g2.setCursor(107, 38);
    if (satsdisplay < 1){
      u8g2.print(sats); 
    } else {
      u8g2.print(satsdisplay);
    }

    u8g2.setCursor(107, 52);
    u8g2.print(sats);
        if (!time_valid)
        {
          u8g2.drawStr(74, 7, "Time : ********");
         
        }
        else
        {
          char time_string[32];
          sprintf(time_string, "Time : %02d:%02d:%02d \n", hr_val_jp, min_val, sec_val);
          u8g2.setCursor(74, 7);
          u8g2.print(time_string);    
        }

      
    //u8g2.drawLog(3, 34, u8g2log);
    u8g2.drawLog(3, 16, u8g2log);
  } while ( u8g2.nextPage() );
  
}
