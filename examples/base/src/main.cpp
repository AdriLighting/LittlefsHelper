#include <Arduino.h>
#include <littlefs_helper.h>


void l_1();
void setup() {
  Serial.begin(115200);

  for(unsigned long const serialBeginTime = millis(); !Serial && (millis() - serialBeginTime > 5000); ) { }
  delay(300);

  Serial.println(F("\n##################################################\n"));  

  Serial.setDebugOutput(false);

  LittleFS.begin();
  // LittleFS.format();

  l_1();

/*
  StaticJsonDocument<100> testDocument;
 
  testDocument["sensorType"] = "temperature";
  testDocument["sensorValue"] = 10;
  
  JsonObject documentRoot = testDocument.as<JsonObject>();
 
  for (JsonPair keyValue : documentRoot) {
    Serial.println(keyValue.value().as<String>());
  }*/

}

void loop() {
	
}

void l_1(){
  // LH_fileManager * _LH_fileManager = new LH_fileManager();


  uint32_t  tNow, tDif;
  String    result;
  File      f;
  char      buf[10];
  String    line      = "";
  String    line_2    = "";
  String    lineRead  = "";
  uint8_t   start     = 0;
  LH_file * _LH_file    = new LH_file(f,    "/myFile_1.txt");

  // DynamicJsonDocument doc(2048);
  // JsonObject root = doc.to<JsonObject>();
  // JsonObject oOjbect = root.createNestedObject("outputs");
  // output_toJson(0, oOjbect, true, true);
  // serializeJson(doc, line_2);
/*
  sprintf(buf, "%03x%03x%03x", 255, 5, 10);     // c1
  line += String(buf) + " "; 
  sprintf(buf, "%03x%03x%03x", 220, 33, 00);    // coff
  line += String(buf) + " ";
  sprintf(buf, "%03x%03x%03x", 80, 200, 222);   // hsv
  line += String(buf) + " ";
  sprintf(buf, "%03x%03x%03x", 252, 253, 4);    // rgb
  line += String(buf) + " ";
  sprintf(buf, "%03x%03x%03x", 200, true, 0);   //tw,istw,tw_v
  line += String(buf) + " ";
  sprintf(buf, "%03x%03x", true, true);         //select,onoff,
  line += String(buf) + " ";
  // sprintf(buf, "%03x%03x%03x", 0, 5, 80);        //type,pin,size
  // line += String(buf) + " ";
  sprintf(buf, "%03x%03x", 12, 1);              //eff_nb,eff_colorMod
  line += String(buf) + " ";
  sprintf(buf, "%03x%03x%03x", 255, 255, 255);  //eff_c1
  line += String(buf) + " ";
  sprintf(buf, "%03x%03x%03x", 80, 0, 1);   //eff_c2
  line += String(buf) + " ";
  Serial.printf("[length: %d]\n", line.length());

  tNow = millis();
  _LH_file->newLine(line);
  _LH_file->newLine(line);
  _LH_file->newLine(line);
  tDif = millis() - tNow;
  Serial.printf("[newLine duration: %d]\n", tDif);
*/

  start     = 0;
  lineRead  = "";

  _LH_file->readLineByte(1, lineRead);
  Serial.printf("[_LH_file readLineByte: %d]\n%s\n", 1, lineRead.c_str());


  int LarraySize;
  String * Larray = explode(lineRead, ' ', LarraySize);
  for(int i = 0; i < LarraySize-1; ++i) {
    Serial.println(Larray[i]);

    line_2 = Larray[i];
    uint8_t len = line_2.length();

    if (len == 9) {
      Serial.printf("[%d: %d]\n", start, getHex(lineRead, start) );
      Serial.printf("[%d: %d]\n", start, getHex(lineRead, start) );
      Serial.printf("[%d: %d]\n", start, getHex(lineRead, start) );
      start+= 1;
    }
    if (len == 6) {
      Serial.printf("[%d: %d]\n", start, getHex(lineRead, start) );
      Serial.printf("[%d: %d]\n", start, getHex(lineRead, start) );
      start+= 1;
    }   
    if (len == 3) {
      Serial.printf("[%d: %d]\n", start, getHex(lineRead, start) );
      start+= 1;
    }   

    Serial.printf("\n");  

  }

  lineRead ="";
  _LH_file->readLine(1, lineRead);
  Serial.printf("[_LH_file readLine: %d]\n%s\n", 1, lineRead.c_str());  

  lineRead ="";
  uint8_t charPos = 5 * 10;
  _LH_file->replaceLineValue("123456 ", 2, charPos);
  _LH_file->readLine(2, lineRead);
  Serial.printf("[_LH_file readLine: %d]\n%s\n", 2, lineRead.c_str());  


  delete _LH_file;

  tNow = millis();
  SPIFFS_fileRead("/myFile_1.txt");
  tDif = millis() - tNow;
  Serial.printf("[duration: %d]\n", tDif);
  SPIFFS_printFiles("/");


  // _LH_file->open(f, "r+");
  // _LH_file->replaceLine(&f, "replace", 2);
  // f.close();   

  // SPIFFS_fileRead("/myFile_1.txt");

  // SPIFFS_fileRead("/myFile_2.txt");
  // SPIFFS_fileRead("/fileManager.json");
  // 
  
}