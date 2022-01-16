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
	-> last char	|| line SIZE
new
	append at eof
*/



StaticJsonDocument<2048> DOC_LHCONF;


// LH_fileManager * _LH_fileManagerPtr; 


void debugPrint(const String & msg, const String & file, const String & line, const String & func) {
	Serial.printf_P(PSTR("[%s:%S] %s\n"), file.c_str() , line.c_str() , func.c_str());
	if (msg!="") Serial.printf_P(PSTR("%s\n"), msg.c_str() ); 
}
int _SPIFFS_printFiles(const String & path){
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
        return totalsize; 
    #elif defined(ESP32)
        return 0;
    #else
    #endif

    return 0;
}
void SPIFFS_printFiles(const String & path){
    #if defined(ESP8266)
        // ADRI_LOG(-1, 0, 2,"","");
        Dir dir = LittleFS.openDir(path);
        Serial.println("");
        Serial.println( F("[Print file and folder]"));
        int totalsize = 0;
        while (dir.next()) {
            String fileInfo = dir.fileName() + (dir.isDirectory() ? " [DIR]" : String(" (") + dir.fileSize() + " b)");
            if (dir.isDirectory()) {
                int dSize = _SPIFFS_printFiles(dir.fileName());
                totalsize += dSize;
            } else  {
                Serial.println(fileInfo);
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
  int 		size 				= t.length();

  while (str_index < size ) {
	  j=t.indexOf(sep,str_index);
	  if (j!=-1) {
	      list_index++;
	      str_index=j+1;
	  }
  }

	String * list = new String[list_index+1];

	str_index		=0;
	list_index	=0;
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



/**
 * @brief		constructor
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
	if (LittleFS.exists(_path)) {savConfig();close();}
	if(_LineArray != nullptr) delete[] _LineArray;
}

/**
 * @brief			mak normal string
 * 
 * @see     https://github.com/AdriLighting/
 * @author  Adrien Grellard 
 * @date    sam. 08 janv. 2022 18:40:11
 *  
 * @param			s 	reference 
 *
 * @return		size of new string
 */
int LH_file::line_denormalize(String * s) {
	int i=s->length()-1;
	while (    ((s->charAt(i)==STUFFING_CHAR)||(s->charAt(i)=='\n'))   &&   (i>0) ) i--;
	*s=s->substring(0,i+1);
	return i+1;
}

/**
 * @brief		mak format string
 * 
 * @see     https://github.com/AdriLighting/
 * @author  Adrien Grellard 
 * @date    sam. 08 janv. 2022 18:40:11
 *  
 * @param      s     reference
 * @param[in]  cr    closing string with "\n"
 */
void LH_file::line_normalize(String * s, boolean cr) {
	*s = s->substring(0, _line_maxSize);													// we adjust the length
	while (s->indexOf('\n')!=-1) s->replace('\n',STUFFING_CHAR);	// we remove all '\n'
	while (s->length()<_line_maxSize) *s+=STUFFING_CHAR;					// we stuff
	if (cr) *s+="\n";																							// we close with single '\n'
}
void LH_file::line_normalize(String * s, uint32_t size, boolean cr) {
	*s = s->substring(0, size);																			
	while (s->indexOf('\n')!=-1) s->replace('\n',STUFFING_CHAR);		
	while (s->length()<size) *s+=STUFFING_CHAR;				
	if (cr) *s+="\n";																								
}

void LH_file::get_lineTotal(uint32_t & result) 		{result = _line_total;}
void LH_file::get_lineMaxSize(uint32_t & result)	{result = _line_maxSize;}

void LH_file::open(File & f, const char * mod){
	f = LittleFS.open(_path, mod);
}
void LH_file::open(const char * mod){
	if (!_fileClose) {
		if (_fileMod != mod) {
			#ifdef DEBUG_LH
				Serial.printf_P(PSTR("[_fileMod: %s][mod: %s]\n"), _fileMod, mod );
			#endif
			close();
		}
	}
	_fileMod = mod;
	_file = LittleFS.open(_path, mod);
	if (_file) 	_fileClose = false;
	else 				_fileClose = true;
}
void LH_file::close(){
	if (_fileClose) return;
	#ifdef DEBUG_LH
		uint32_t s = millis();
		_file.close();	
		Serial.printf_P(PSTR("[LH_file::close][took %d ms]\n"), millis() - s);
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

		_root[F("lt")] 		= _line_total;
		_root[F("lsm")] 	= _line_maxSize;

		open("w");
		if (_fileClose) {
			return false;
		}

		serializeJson(DOC_LHCONF, out);
		line_normalize(&out, _line_configMaxSize);
		_file.print(out);

		savConfig(&_file);

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
	line_denormalize(&line);

	DeserializationError error = deserializeJson(DOC_LHCONF, line);
	if (error) {
		return false;
	}

	_line_maxSize 	= DOC_LHCONF[F("lsm")].as<uint32_t>(); 
	_line_total 		= DOC_LHCONF[F("lt")].as<uint32_t>(); 

	return true;
}

void LH_file::savConfig(File * f){
	DOC_LHCONF[F("lsm")] 	= _line_maxSize;
	DOC_LHCONF[F("lt")] 		= _line_total;

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
}

boolean LH_file::readLines(uint32_t start, uint32_t end, String & result, const String & sep){
	open("r");
	if (_fileClose) {
		return false;
	}
	for (uint32_t i = start; i <= end; ++i) {
		readLine(&_file, i, result);
		if (i < end) result+= sep;
	}
	return true;
}
boolean LH_file::readLine(uint32_t lineNbr, String & result){
	open("r");
	if (_fileClose) {
		return false;
	}

	uint32_t S= (lineNbr)*(_line_maxSize);
	_file.seek(S+lineNbr);
	String line = _file.readStringUntil('\n');
	line_denormalize(&line);
	result += line;
	return true;
}
/*void flf::gotoline(File * f, int line) {
	int pos=(fd.char_nbr)*(line+1);
	f->seek(pos,SeekSet);
}*/
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
	line_normalize(&line);
	result += line;
	return true;
}
boolean LH_file::readLine(File * f, uint32_t lineNbr, String & result){
	uint32_t S= (lineNbr)*(_line_maxSize);
	f->seek(S+lineNbr);
	String line = f->readStringUntil('\n');
	line_denormalize(&line);
	result += line;
	return true;
}


boolean LH_file::replaceLineValue(const String & value, uint32_t lineNbr, uint8_t charPos){
	open("r+");
	if (_fileClose) {
		return false;
	}

	uint32_t S= (lineNbr)*(_line_maxSize);
	_file.seek(S+lineNbr+charPos);
	_file.print(value);

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
	line_normalize(&result, false);

	uint32_t S= (lineNbr)*(_line_maxSize);
	_file.seek(S+lineNbr);
	_file.print(result);

	return true;
}
boolean LH_file::replaceLine(File * f, const String & value, uint32_t lineNbr){
	if ((value.length()-1) > _line_maxSize) {
		return false;		
	}

	String result = value;
	line_normalize(&result, false);

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
	line_normalize(&result);

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

*/