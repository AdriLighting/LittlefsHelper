#include <Arduino.h>
#include <littlefs_helper.h>

/**
 * @brief      convert hex to uint8_t
 *
 * @param[in]  source  maximum 1octet coded in 3charactere (%03x)
 * @param      start   postion to start extraction of 3chars
 *
 * @return     value to uint8_t
 */
uint8_t getHex(const String & source, uint8_t & start){ 
  String s_1 = String(source[start])+String(source[start+1])+String(source[start+2]);
  start += 3;
  return (uint8_t)strtol(s_1.c_str(), NULL, 16);  
}

uint32_t strToHex(const char* str) {
  return (uint32_t) strtoull(str, 0, 16);
}

/**
 * @brief      split string with sep
 *
 * @param[in]  s      source String
 * @param[in]  sep    separator to split 
 * @param      rSize  size of array by ref
 *
 * @return     array of split values
 */
String * explode(String s, char sep, int & rSize) {
  String  t           = s + sep;
  int     str_index   = 0;
  int     list_index  = 0;
  int     j;
  String  sub;
  int     size        = t.length();

  while (str_index < size ) {
    j=t.indexOf(sep,str_index);
    if (j!=-1) {
        list_index++;
        str_index=j+1;
    }
  }

  String * list = new String[list_index+1];

  str_index   =0;
  list_index  =0;
  while (str_index < size ) {
    j=t.indexOf(sep,str_index);
    if (j!=-1) {
      sub=t.substring(str_index, j);
      list[list_index]=sub;
      list_index++;
      str_index=j+1;
    }
  }

  list[list_index]="";
  rSize = list_index;

  return list;
}

void l_1(){
  // LH_fileManager * _LH_fileManager = new LH_fileManager();


  uint32_t  tNow, tDif, lrSize;
  String    result;
  File      f;
  File      f_2;
  char      buf[10];
  String    line      = "";
  String    lineRead  = "";

  if(LittleFS.exists(F("/myFile_1.txt")))  LittleFS.remove(F("/myFile_1.txt"));
  if(LittleFS.exists(F("/myFile_2.txt")))  LittleFS.remove(F("/myFile_2.txt"));
  LH_file * _LH_file    = new LH_file(f, "/myFile_1.txt");
  LH_file * _LH_file_2  = new LH_file(f_2, "/myFile_2.txt");

  // DynamicJsonDocument doc(2048);
  // JsonObject root = doc.to<JsonObject>();
  // JsonObject oOjbect = root.createNestedObject("outputs");
  // output_toJson(0, oOjbect, true, true);
  // serializeJson(doc, line_2);

  sprintf(buf, "%03x%03x%03x", 255, 5, 10);     // c1
  line += String(buf) + " "; 
  sprintf(buf, "%03x%03x%03x", 220, 33, 001);   // coff
  line += String(buf) + " ";
  sprintf(buf, "%03x%03x%03x", 80, 200, 222);   // hsv
  line += String(buf) + " ";
  sprintf(buf, "%03x%03x%03x", 252, 253, 4);    // rgb
  line += String(buf) + " ";
  sprintf(buf, "%03x%03x%03x", 200, true, 0);   //tw,istw,tw_v
  line += String(buf) + " ";
  sprintf(buf, "%03x%03x", true, true);         //select,onoff,
  line += String(buf) + " ";
  sprintf(buf, "%03x%03x%03x", 0, 5, 80);       //type,pin,size
  line += String(buf) + " ";
  sprintf(buf, "%03x%03x", 12, 1);              //eff_nb,eff_colorMod
  line += String(buf) + " ";
  sprintf(buf, "%03x%03x%03x", 255, 255, 255);  //eff_c1
  line += String(buf) + " ";
  sprintf(buf, "%03x%03x%03x", 80, 0, 1);       //eff_c2
  line += String(buf) + " ";
  Serial.printf_P(PSTR("[length: %d]\n"), line.length());

  tNow = millis();
  _LH_file->newLine(line);
  _LH_file->newLine(line);
  _LH_file->newLine(line);
  _LH_file->newLine(line);
  tDif = millis() - tNow;
  Serial.printf_P(PSTR("[newLine duration: %d]\n"), tDif);

  lrSize = _LH_file->readLine(1, lineRead);
  Serial.printf_P(PSTR("[_LH_file readLineByte: %d-%d]\n%s\n"), 1, lrSize, lineRead.c_str());

  uint8_t start = 0;
  int LarraySize;
  String * Larray = explode(lineRead, ' ', LarraySize);
  for(int i = 0; i < LarraySize-1; ++i) {
    Serial.println(Larray[i]);

    String line_2 = Larray[i];
    uint8_t len = line_2.length();

    if (len == 9) {
      Serial.printf_P(PSTR("[%d: %d]\n"), start, getHex(lineRead, start) );
      Serial.printf_P(PSTR("[%d: %d]\n"), start, getHex(lineRead, start) );
      Serial.printf_P(PSTR("[%d: %d]\n"), start, getHex(lineRead, start) );
      start+= 1;
    }
    else if (len == 6) {
      Serial.printf_P(PSTR("[%d: %d]\n"), start, getHex(lineRead, start) );
      Serial.printf_P(PSTR("[%d: %d]\n"), start, getHex(lineRead, start) );
      start+= 1;
    }   
    else if (len == 3) {
      Serial.printf_P(PSTR("[%d: %d]\n"), start, getHex(lineRead, start) );
      start+= 1;
    }   

    Serial.printf("\n");  

  }

  lrSize = _LH_file->readLine(1, lineRead);
  Serial.printf_P(PSTR("[_LH_file readLine: %d-%d]\n%s\n"), 1, lrSize, lineRead.c_str());  

  uint8_t charPos = 5 * 10;
  _LH_file->replaceLineValue("123456 ", 2, charPos+4);
  lrSize = _LH_file->readLine(2, lineRead);
  Serial.printf_P(PSTR("[_LH_file readLine: %d-%d]\n%s\n"), 2, lrSize, lineRead.c_str());  


  tNow = millis();
  _LH_file_2->newLine("Nam quis nulla. Integer malesuada. In in enim a arcu imperdiet malesuada. Sed vel lectus. Donec");
  _LH_file_2->newLine("Nam quis nulla. Integer malesuada. In in enim a arcu imperdiet malesuada. Sed vel lectus. Donec ");
  _LH_file_2->newLine("Nam quis---nulla.- In - teger malesuada. -In in enim a -");
  _LH_file_2->newLine("Nam quis---nulla.- In - teger malesuada. -In in enim a -");
  tDif = millis() - tNow;
  Serial.printf_P(PSTR("[newLine duration: %d]\n"), tDif);

  lrSize = _LH_file_2->readLine(3, lineRead);
  Serial.printf_P(PSTR("[_LH_file_2 readLine: %d-%d]\n%s\n"), 3, lrSize, lineRead.c_str());  

  _LH_file_2->replaceLine("lineRead",1);
  _LH_file_2->replaceLine("lineRead",2);
  _LH_file_2->replaceLineValue("test",2,4);


  delete _LH_file;
  delete _LH_file_2;

  tNow = millis();
  SPIFFS_fileRead("/myFile_1.txt");
  tDif = millis() - tNow;
  Serial.printf("[duration: %d]\n", tDif);
  SPIFFS_printFiles("/");

  tNow = millis();
  SPIFFS_fileRead("/myFile_2.txt");
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



void setup() {
  Serial.begin(115200);

  for(unsigned long const serialBeginTime = millis(); !Serial && (millis() - serialBeginTime > 5000); ) { }
  delay(300);

  Serial.println(F("\n##################################################\n"));  

  Serial.setDebugOutput(false);

  LittleFS.begin();

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



void output_toJson(uint8_t pos, JsonObject & value, boolean shortKey, boolean fs){
    // len = 758
    JsonObject col = value.createNestedObject("c1");
    col[F("r")] = 255;
    col[F("g")] = 0;
    col[F("b")] = 0;

    col = value.createNestedObject("coff");
    col[F("r")] = 255;
    col[F("g")] = 0;
    col[F("b")] = 0;

    if (!shortKey){
      value[F("hsv_h")]       = 80;
      value[F("hsv_s")]       = 255;
      value[F("hsv_v")]       = 255;
      value[F("rgb_r")]       = 255;
      value[F("rgb_g")]       = 0;
      value[F("rgb_b")]       = 0;
      value[F("tw")]          = 255;
      value[F("tw_v")]        = 255;
      value[F("bri")]         = 255;
      value[F("select")]      = true;
      value[F("onoff")]       = true;
      value[F("type")]        = 0;
      value[F("pin")]         = 5;
      value[F("size")]        = 60;
      value[F("name")]        = "device";
      value[F("eff_activ")]   = false;
      if (fs) {
      // if (_effects[pos]) {
      value[F("eff_nb")]      = 1;      
      // }
      value[F("eff_colorMod")] = 0;
      col = value.createNestedObject("eff_c1");
      col[F("r")] = 255;
      col[F("g")] = 0;
      col[F("b")] = 0;
      col = value.createNestedObject("eff_c2");
      col[F("r")] = 255;
      col[F("g")] = 0;
      col[F("b")] = 0;      
      }

    } else {
      value[F("0")]           = 80;
      value[F("1")]           = 255;
      value[F("2")]           = 255;
      value[F("3")]           = 255;
      value[F("4")]           = 0;
      value[F("5")]           = 0;
      value[F("6")]           = 255;
      value[F("7")]           = false;
      value[F("8")]           = 255;
      value[F("9")]           = true;
      value[F("10")]          = true;
      value[F("11")]          = 0;
      value[F("12")]          = 5;
      value[F("13")]          = 60;
      value[F("14")]          = "device";   
      value[F("15")]          = 255;      
      value[F("16")]          = false;  
      if (fs) {
      // if (_effects[pos]) {
      value[F("17")]          = 1;                       
      // }
      value[F("18")]          = 0;
      col = value.createNestedObject("eff_c1");
      col[F("r")] = 255;
      col[F("g")] = 0;
      col[F("b")] = 0;
      col = value.createNestedObject("eff_c2");
      col[F("r")] = 255;
      col[F("g")] = 0;
      col[F("b")] = 0;        
      }
    
    }
    // fsprintf("[eff][pos: %d][name: %s]\n", _effects[pos]->getEn()  , _effects[pos]->getEffectName().c_str() );    
}
