/*

  MIT License

  Copyright (c) 2022 Adrien Grellard

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/

/**
 * @file      littlefs_helper.cpp
 *
 * @brief     main file
 * @see       https://github.com/AdriLighting
 * 
 * @author    Adrien Grellard   
 * @date      sam. 15 janv. 2022 13:39:16
 *
 */

#include "littlefs_helper.h"

/*
config
  SIZE
  nb lines
line
  line SIZE string
replace
  line pos *
    line SIZE
  tot +
    line pos
  -> print line SIZE string
read
  line pos *
    line SIZE
  tot +
    line pos  
  -> last char  || line SIZE
new
  append at eof
*/



StaticJsonDocument<2048> DOC_LHCONF;


// LH_fileManager * _LH_fileManagerPtr; 

#ifdef DEBUG_LH
  void LH_debugPrint(const String & msg, const String & file, const String & line, const String & func) {
    Serial.printf_P(PSTR("[%s:%S] %s\n"), file.c_str() , line.c_str() , func.c_str());
    if (msg!="") Serial.printf_P(PSTR("%s\n"), msg.c_str() ); 
  }
#endif

unsigned int _SPIFFS_printFiles_size;

void _SPIFFS_printFiles(const String & path){
    #if defined(ESP8266)
        Dir sub_dir = LittleFS.openDir(path);

        Serial.printf_P(PSTR("\t[%s]\n"), path.c_str());
        int totalsize = 0;
        while (sub_dir.next()) {
            if (sub_dir.isDirectory()) _SPIFFS_printFiles(path + "/" + sub_dir.fileName());
            else {
                String sub_fileInfo = sub_dir.fileName() + (sub_dir.isDirectory() ? " [sub_dir]" : String(" (") + sub_dir.fileSize() + " b)");
                Serial.printf_P(PSTR("\t\t%s\n"), sub_fileInfo.c_str());   
                totalsize += sub_dir.fileSize();
            }
        }
        if (totalsize > 0) Serial.printf_P(PSTR("\t\t\t[totalsize: %d b]\n"), totalsize); 
        _SPIFFS_printFiles_size += totalsize;
        // return totalsize; 
    #elif defined(ESP32)
        // return 0;
    #else
    #endif

    // return 0;
}
void SPIFFS_printFiles(const String & path){
    #if defined(ESP8266)
        // ADRI_LOG(-1, 0, 2,"","");
        Dir dir = LittleFS.openDir(path);
        Serial.println("");
        Serial.println( F("[Print file and folder]"));
        int totalsize = 0;
        _SPIFFS_printFiles_size = 0;
        while (dir.next()) {
            String fileInfo = dir.fileName() + (dir.isDirectory() ? " [DIR]" : String(" (") + dir.fileSize() + " b)");
            if (dir.isDirectory()) {
                _SPIFFS_printFiles(dir.fileName());
            } else  {
                Serial.println(fileInfo);
                totalsize += dir.fileSize();
            }
        }
        Serial.printf_P(PSTR("\n[totalsize: %d b]\n"), totalsize+_SPIFFS_printFiles_size);
        Serial.println();
        // ADRI_LOG(-1, 1, 2,"","");
    #elif defined(ESP32)
    #else
    #endif    
}


void SPIFFS_fileRead(const String &ret){
    Serial.printf("\n[SPIFFS_fileRead][path: %s]\n", ret.c_str());
    int nbr = 0;
    File file = LittleFS.open(ret, "r");
    if (file) {
        while (file.position()<file.size()) {
            String xml = file.readStringUntil('\n');
            if (xml != "") {
              Serial.printf("[%5d] %s\n", nbr, xml.c_str());
              yield();
              nbr++;
            }
        }
        file.close();
        Serial.printf("[SPIFFS_fileRead][SUCCES]\n"); 
    } else Serial.printf("[SPIFFS_fileRead][FAILE OPEN FILE r]\n"); 
}

void SPIFFS_filesRead(const String & path){
    #if defined(ESP8266)
        // ADRI_LOG(-1, 0, 2,"","");
        Dir dir = LittleFS.openDir(path);
        Serial.println("");
        Serial.println( F("[Print file and folder]"));
        int totalsize = 0;
        while (dir.next()) {
            String fileInfo = dir.fileName() + (dir.isDirectory() ? " [DIR]" : String(" (") + dir.fileSize() + " b)");
            if (dir.isDirectory()) {
                // _SPIFFS_printFiles(dir.fileName());
            } else  {
                Serial.println(fileInfo);
                SPIFFS_fileRead(dir.fileName());
                totalsize += dir.fileSize();
            }
        }
        Serial.printf_P(PSTR("\n[totalsize: %d b]\n"), totalsize);
        Serial.println();
        // ADRI_LOG(-1, 1, 2,"","");
    #elif defined(ESP32)
    #else
    #endif    
}




/**
 * @brief   constructor
 * 
 * @see     https://github.com/AdriLighting/
 * @author  Adrien Grellard 
 * @date    sam. 08 janv. 2022 18:40:11
 */
LH_file::LH_file(){
  _STATU = LHS_OK;
}
LH_file::LH_file(File & f, const char * path){
  _STATU = LHS_OK;
  start(f, path);
}

LH_file::~LH_file(){
  if (LittleFS.exists(_path)) {close();savConfig();close();}
  if(_LineArray != nullptr) delete[] _LineArray;
}

/**
 * @brief     mak normal string
 * 
 * @see     https://github.com/AdriLighting/
 * @author  Adrien Grellard 
 * @date    sam. 08 janv. 2022 18:40:11
 *  
 * @param     s   reference 
 *
 * @return    size of new string
 */
uint32_t LH_file::line_denormalize(String * s) {
  String source = *s;
  source = String(source[0])+String(source[1])+String(source[2]);
  uint8_t len = source.toInt();
  *s=s->substring(4,len+4);
  return len;
}
uint32_t LH_file::config_denormalize(String * s) {
  uint32_t i=s->length()-1;
  while (    ((s->charAt(i)==STUFFING_CHAR)||(s->charAt(i)=='\n'))   &&   (i>0) ) i--;
  *s=s->substring(0,i+1);
  return i+1;
}

/**
 * @brief      mak format string
 *
 * @see        https://github.com/AdriLighting/
 * @author     Adrien Grellard
 * @date       sam. 08 janv. 2022 18:40:11
 *
 * @param      s       reference
 * @param[in]  cr      closing string with "\n"
 * @param[in]  prefix  prefix with size of line int 3ch (03d)
 * @param[in]  sr      { parameter_description }
 */
void LH_file::line_normalize(String * s, boolean cr, boolean prefix, char sr) {
  if (prefix){
    char len[4];
    sprintf(len, "%03d ", s->length());
    *s = String(len) + *s;
  }
  *s = s->substring(0, _line_maxSize);         
  while (s->indexOf('\n')!=-1) s->replace('\n',sr);  // we remove all '\n'
  while (s->length()<_line_maxSize) *s+=String(sr);          // we stuff
  if (cr) *s+="\n";                                             // we close with single '\n'
}
void LH_file::line_normalize(String * s, uint32_t size, boolean cr, char sr) {
  *s = s->substring(0, size);                                     
  while (s->indexOf('\n')!=-1) s->replace('\n',sr);    
  while (s->length()<size) *s+=String(sr);       
  if (cr) *s+="\n";                                               
}

void LH_file::get_lineTotal(uint32_t & result)    {result = _line_total;}
void LH_file::get_lineMaxSize(uint32_t & result)  {result = _line_maxSize;}

void LH_file::open(File & f, const char * mod){
  f = LittleFS.open(_path, mod);
}
void LH_file::open(const char * mod){
  #ifdef DEBUG_LH
    Serial.printf_P(PSTR("[LH_file::open >START<][_fileClose: %d][mod: %s]\n"), _fileClose, mod );
  #endif  
  if (!_fileClose) {
    if (_fileMod != mod) {
      close();
    }
  }
  _fileMod = mod;
  _file = LittleFS.open(_path, mod);
  if (_file)  _fileClose = false;
  else        _fileClose = true;
  #ifdef DEBUG_LH
    Serial.printf_P(PSTR("[LH_file::open >END<][_fileClose: %d]\n"), _fileClose );
  #endif   
}
void LH_file::close(){
  if (_fileClose) return;
  #ifdef DEBUG_LH
    uint32_t s = millis();
    _file.close();  
    Serial.printf_P(PSTR("[LH_file::close][_fileMod: %s][duration: %d ms]\n"), _fileMod, millis() - s);
  #else
    _file.close();  
  #endif
  _fileClose = true;
}

boolean LH_file::start(const char * path){
  _path = path;
  return openConfig();
}
boolean LH_file::start(File & f, const char * path){
  _file = f;
  _path = path;
  return openConfig();
}


boolean LH_file::openConfig() {

  if (!LittleFS.exists(_path)) {
    String out;

    _line_total = 1;

    _root = DOC_LHCONF.to<JsonObject>();

    _root[F("lt")]    = _line_total;
    _root[F("lsm")]   = _line_maxSize;

    open("w");
    if (_fileClose) {
      return false;
    }

    serializeJson(DOC_LHCONF, out);
    line_normalize(&out, _line_configMaxSize);
    _file.print(out);

    savConfig(&_file);
    close();
  }

  if (!readConfig()) return false;

  return true;      
}

boolean LH_file::readConfig() {
  open("r");
  if (_fileClose) {
    return false;
  } 

  _file.seek(0,SeekSet);
  String line = _file.readStringUntil('\n');

  config_denormalize(&line);

  DeserializationError error = deserializeJson(DOC_LHCONF, line);
  if (error) {
    Serial.printf("[DeserializationError]\n");
    return false;
  }
  _line_maxSize   = DOC_LHCONF[F("lsm")].as<uint32_t>(); 
  _line_total     = DOC_LHCONF[F("lt")].as<uint32_t>(); 
  #ifdef DEBUG_LH
    Serial.printf("[readConfig][_fileMod: %s][_line_maxSize: %d][_line_total: %d]\n", _fileMod, _line_maxSize, _line_total);
  #endif
  return true;
}

void LH_file::savConfig(File * f){
  DOC_LHCONF[F("lsm")]  = _line_maxSize;
  DOC_LHCONF[F("lt")]     = _line_total;

  String out;
  serializeJson(DOC_LHCONF, out);
  line_normalize(&out, _line_configMaxSize);
  int old_pos = f->position();
  f->seek(0,SeekSet);
  f->print(out);
  f->seek(old_pos,SeekSet);

  // JsonObject documentRoot = DOC_LHCONF.as<JsonObject>();
  // Serial.printf(">keyValue\n");
 //  for (JsonPair keyValue : documentRoot) {
 //    Serial.printf("%s\n",keyValue.key().c_str() );
 //  }  
}
boolean LH_file::savConfig(){
  open("r+");
  if (_fileClose) {
    return false;
  }
  savConfig(&_file); 
  close();
  return true;
}



uint32_t LH_file::readLine(uint32_t lineNbr, String & result){
  open("r");
  if (_fileClose) {
    return 0;
  }

  uint32_t S= (lineNbr)*(_line_maxSize);
  _file.seek(S+lineNbr,SeekSet);
  result = _file.readStringUntil('\n');
  return line_denormalize(&result);
}

boolean LH_file::readLineByte(uint32_t lineNbr, String & result){
  open("r");
  if (_fileClose) {
    return false;
  }

  uint32_t S= (lineNbr)*(_line_maxSize);
  _file.seek(S+lineNbr,SeekSet);
  char buf[_line_maxSize];
  _file.readBytes(buf, _line_maxSize);
  String line = String(buf);
  line_normalize(&line, true, true);
  result += line;
  return true;
}


boolean LH_file::deleteLine(uint32_t lineNbr){
  open("r+");
  if (_fileClose) {
    return false;
  }
  Serial.println("test");
  uint32_t S= (lineNbr)*(_line_maxSize);
  _file.seek(S+lineNbr, SeekSet);
  String result = "";
  line_normalize(&result, false, true);
  _file.print(result); 
 
  // S= (lineNbr+1)*(_line_maxSize);
  // _file.seek(S+(lineNbr+1));
  // _file.print(result); 


  close();
  return true;
}

boolean LH_file::replaceLineValue(const String & value, uint32_t lineNbr, uint8_t charPos){
  open("r+");
  if (_fileClose) {
    return false;
  }
  uint32_t S= (lineNbr)*(_line_maxSize);
  _file.seek(S+lineNbr+charPos,SeekSet);
  _file.print(value);
  close();

  return true;
}

boolean LH_file::replaceLine(const String & value, uint32_t lineNbr){
  open("r+");
  if (_fileClose) {
    return false;
  }
  if ((value.length()-1) > _line_maxSize) {
    return false;   
  }

  String result = value;
  line_normalize(&result, false, true);

  uint32_t S= (lineNbr)*(_line_maxSize);
  _file.seek(S+lineNbr,SeekSet);
  _file.print(result);
  close();

  return true;
}
boolean LH_file::replaceLine(File * f, const String & value, uint32_t lineNbr){
  if ((value.length()-1) > _line_maxSize) {
    return false;   
  }

  String result = value;
  line_normalize(&result, false, true);

  uint32_t S= (lineNbr)*(_line_maxSize);
  f->seek(S+lineNbr);
  f->print(result);
  return true;
}


boolean LH_file::newLine(const String & value, boolean sav){
  if (_line_total == 0) return false;

  if ((value.length()-1) > _line_maxSize) {
    return false;   
  }

  String result = value;
  line_normalize(&result, true, true);

  open("a");
  if (_fileClose) {
    return false;
  }     

  _line_total++;
  _file.print(result);
  close();

  if (sav) {
    open("r+");
    if (_fileClose) {
      return false;
    }       
    savConfig(&_file);
  }

  return true;
}
boolean LH_file::newLine(File * f, const String & value){
  if (_STATU != LHS_OK) {
    Serial.printf("_STATU: %d\n", _STATU);
    return false;
  }
  if (_line_total == 0) {Serial.printf("_line_total: %d\n", _line_total); return false;}

  if ((value.length()-1) > _line_maxSize) {
    _STATU = LHS_ERR_SIZEMAX;
    Serial.printf("_STATU: %d\n", _STATU);
    return false;   
  }

  String result = value;
  line_normalize(&result);
  
  _line_total++;
  f->print(result);

  return true;
}







/*
LH_fileManager::LH_fileManager(){
  _LH_fileManagerPtr = this;
  deserializeConfigFile();
}
bool LH_fileManager::deserializeConfigFile(){
    File jfile = LittleFS.open(_path, "r");
    DeserializationError error;
    if (jfile){
        error = deserializeJson(DOC_LHCONF, jfile);
        jfile.close();
    } else {
        return false;
    }

    if (error) {
        return false;
    }
    return true;
}
void LH_fileManager::update(const char * path, uint32_t line_sizeMax, uint32_t line_total){
  if (!LittleFS.exists(_path)) {
    JsonObject root = DOC_LHCONF.to<JsonObject>();
    JsonObject obj  = root.createNestedObject(path);
    obj[F("line_sizeMax")]  = line_sizeMax;
    obj[F("line_total")]    = line_total;
  } else {
    DOC_LHCONF[path][F("line_sizeMax")]   = line_sizeMax;
    DOC_LHCONF[path][F("line_total")]     = line_total;
  }
}
String ch_toString(const char * c){
  return String((const __FlashStringHelper*) c);
}
void LH_fileManager::deleteObj(const char * path){
  LittleFS.remove(path);
  JsonObject documentRoot = DOC_LHCONF.as<JsonObject>();
  documentRoot.remove(path);
}

void LH_fileManager::sav(){
  File f = LittleFS.open(_path, "w");
  serializeJson(DOC_LHCONF, f);
  f.close();    
}

*/