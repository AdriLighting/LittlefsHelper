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

line 
len
type

*/

StaticJsonDocument<2048> DOC_LHCONF;
LH_fileManager * _LH_fileManagerPtr;


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



LH_file::LH_file(){
	_STATU = LHS_OK;
}
int LH_file::line_denormalize(String * s) {
	int i=s->length()-1;
	while (    ((s->charAt(i)==STUFFING_CHAR)||(s->charAt(i)=='\n'))   &&   (i>0) ) i--;
	*s=s->substring(0,i+1);
	return i+1;
}
void LH_file::line_normalize(String * s, boolean cr) {
	*s = s->substring(0, _line_maxSize);													// we adjust the length
	while (s->indexOf('\n')!=-1) s->replace('\n',STUFFING_CHAR);	// we remove all '\n'
	while (s->length()<_line_maxSize) *s+=STUFFING_CHAR;					// we stuff
	if (cr) *s+="\n";																											// we close with single '\n'
}
void LH_file::line_normalize(String * s, uint32_t size) {
	*s = s->substring(0, size);																			
	while (s->indexOf('\n')!=-1) s->replace('\n',STUFFING_CHAR);		
	while (s->length()<size) *s+=STUFFING_CHAR;				
	*s+="\n";																								
}

void LH_file::savConfig(File * f){
	DOC_LHCONF[F("lsm")] 	= _line_maxSize;
	DOC_LHCONF[F("lt")] 		= _line_total;

	String out;
	JsonObject obj = _root.createNestedObject("_path");
	obj[F("TEST")] = _line_total;
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
void LH_file::updateConfig(){

}

boolean LH_file::readConfig() {
	File f = LittleFS.open(_path, "r");
	if (!f) {
		_STATU = LHS_ERR_R;
		return false;
	}	
	f.seek(0,SeekSet);
	String line = f.readStringUntil('\n');
	line_denormalize(&line);
	f.close();

	DeserializationError error = deserializeJson(DOC_LHCONF, line);
	if (error) {
		_STATU = LHS_ERR_DESERIALIZE;
		return false;
	}

	_line_maxSize 	= DOC_LHCONF[F("lsm")].as<uint32_t>(); 
	_line_total 		= DOC_LHCONF[F("lt")].as<uint32_t>(); 

	return true;
}
boolean LH_file::openConfig() {

	if (!LittleFS.exists(_path)) {
		String out;

		_line_total = 1;

		_root = DOC_LHCONF.to<JsonObject>();

		_root[F("lt")] 		= _line_total;
		_root[F("lsm")] 	= _line_maxSize;

		File f = LittleFS.open(_path, "w");
		if (!f) {
			_STATU = LHS_ERR_W;
			return false;
		}
		serializeJson(DOC_LHCONF, out);
		line_normalize(&out, _line_configMaxSize);
		f.print(out);

		savConfig(&f);

		f.close();		
	}

	if (!readConfig()) return false;



  return true;			
}

void LH_file::open(const char * path){

	_path = path;

	openConfig();

}

boolean LH_file::readLine(uint32_t lineNbr, boolean sav){
	if (_STATU != LHS_OK) {
		Serial.printf("_STATU: %d\n", _STATU);
		return false;
	}
	File f = LittleFS.open(_path, "r");
	if (!f) {
		_STATU = LHS_ERR_R;
		return false;
	}

	uint32_t S= (lineNbr)*(_line_maxSize);
	f.seek(S+lineNbr);
	String line = f.readStringUntil('\n');
	line_denormalize(&line);
	f.close();

	Serial.printf("[readLine][pos! %d]\n%s\n", lineNbr, line.c_str());

	return true;
}

boolean LH_file::replaceLine(const String & value, uint32_t lineNbr, boolean sav){
	if (_STATU != LHS_OK) {
		Serial.printf("_STATU: %d\n", _STATU);
		return false;
	}
	File f = LittleFS.open(_path, "r+");
	if (!f) {
		_STATU = LHS_ERR_RW;
		return false;
	}
	if ((value.length()-1) > _line_maxSize) {
		_STATU = LHS_ERR_SIZEMAX;
		return false;		
	}

	String result = value;
	line_normalize(&result, false);

	uint32_t S= (lineNbr)*(_line_maxSize);
	f.seek(S+lineNbr);
	f.print(result);
	f.close();

	return true;
}

boolean LH_file::newLine(const String & value, boolean sav){
	if (_STATU != LHS_OK) {
		Serial.printf("_STATU: %d\n", _STATU);
		return false;
	}
	if (_line_total == 0) return false;

	if ((value.length()-1) > _line_maxSize) {
		_STATU = LHS_ERR_SIZEMAX;
		return false;		
	}

	String result = value;
	line_normalize(&result);

	File f = LittleFS.open(_path, "a");
	if (!f) {
		_STATU = LHS_ERR_A;
		return false;
	}			
	_line_total++;
	f.print(result);
	f.close();
	if (sav) {
		f = LittleFS.open(_path, "r+");
		if (!f) {
			_STATU = LHS_ERR_W;
			return false;
		}				
		savConfig(&f);
		f.close();
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

void l_1(){
	// LH_fileManager * _LH_fileManager = new LH_fileManager();
	LH_file * _LH_file = new LH_file();
	_LH_file->open("/myFile_1.txt");

	File f = LittleFS.open("/myFile_1.txt", "a");
	_LH_file->newLine(&f, "line a");
	_LH_file->newLine(&f, "line 2");
	_LH_file->newLine(&f, "line 3ssssssssssssssss");
	_LH_file->newLine(&f, "line 4");
	f.close();

	// _LH_fileManager->update(_LH_file->_path, _LH_file->_line_maxSize, _LH_file->_line_total );
	// _LH_fileManager->deleteObj(_LH_file->_path);
	// _LH_fileManager->sav();

		// File f = LittleFS.open("/myFile_1.txt", "r+");
		// _LH_file->savConfig(&f);
		// f.close();		

	SPIFFS_fileRead("/myFile_1.txt");

	Serial.print(F("\n"));
	_LH_file->readLine( 0);
	_LH_file->readLine( 2);
	_LH_file->replaceLine("line replace", 2);
	_LH_file->readLine( 2);

	SPIFFS_fileRead("/myFile_1.txt");

	SPIFFS_fileRead("/myFile_2.txt");
	SPIFFS_fileRead("/fileManager.json");
}

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
		JsonObject obj 	= root.createNestedObject(path);
		obj[F("line_sizeMax")] 	= line_sizeMax;
		obj[F("line_total")] 		= line_total;
	}	else {
		DOC_LHCONF[path][F("line_sizeMax")] 	= line_sizeMax;
		DOC_LHCONF[path][F("line_total")] 		= line_total;
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

