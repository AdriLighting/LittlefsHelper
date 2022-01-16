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
 * @file      littlefs_helper.h
 *
 * @brief     main file
 * @see       https://github.com/AdriLighting
 * 
 * @author    Adrien Grellard   
 * @date      sam. 15 janv. 2022 13:39:16
 *
 */

#ifndef LITTLEFS_HELPER_H
#define LITTLEFS_HELPER_H

#include <Arduino.h>
#include <Arduino.h>
#if defined(ESP8266)
	#include <LittleFS.h>
#elif defined(ESP32)
	#include <SPIFFS.h>
#else
#endif
#include <ArduinoJson.h>


// #define DEBUG_LH


#ifndef ARDUINOTRACE_FUNCTION_NAME_IN_FLASH
#if defined(ESP8266)
#define ARDUINOTRACE_FUNCTION_NAME_IN_FLASH 1
#else
#define ARDUINOTRACE_FUNCTION_NAME_IN_FLASH 0
#endif
#endif

#if ARDUINOTRACE_FUNCTION_NAME_IN_FLASH
#define ARDUINOTRACE_FUNCTION_NAME \
  reinterpret_cast<const __FlashStringHelper *>(__PRETTY_FUNCTION__)
#else
#define ARDUINOTRACE_FUNCTION_NAME __PRETTY_FUNCTION__
#endif

void debugPrint(const String & msg, const String & file, const String & line, const String & func);

#define TRACE(text)				\
		{											\
			debugPrint(String(text), String(__FILE__), String(__LINE__), String(ARDUINOTRACE_FUNCTION_NAME)); \
		}


void SPIFFS_fileRead(const String &ret);
void SPIFFS_printFiles(const String & path);
uint8_t getHex(const String & source, uint8_t & start);
String * explode(String s, char sep, int & rSize) ;

#define STUFFING_CHAR '-'

/** 
 * @enum    LH_STATU
 * @brief   error mod
 *          
 * @see     https://github.com/AdriLighting
 * 
 * @author  Adrien Grellard   
 * @date    sam. 15 janv. 2022 13:40:41
 */
enum LH_STATU
{
	LHS_OK, 							/**<  */
	LHS_ERROR, 						/**<  */
	LHS_ERR_READCONFIG, 	/**<  */
	LHS_ERR_DESERIALIZE, 	/**<  */
	LHS_ERR_W, 						/**<  */
	LHS_ERR_A, 						/**<  */
	LHS_ERR_SIZEMAX, 			/**<  */
	LHS_ERR_RW, 					/**<  */
	LHS_ERR_R 						/**<  */
};

/**
 * @class   LH_file
 * 
 * @brief   création/lecture/ecriture d'un fichier stocker sur la memoir spiff du device
 * @see     https://github.com/AdriLighting/
 * 
 * @author  Adrien Grellard 
 * @date    sam. 15 janv. 2022 13:40:41
 */
class LH_file
{
	String 				* _LineArray = nullptr;
	LH_STATU 			_STATU 							= LHS_OK; 					/**< \brief last error */
	uint32_t			_line_maxSize 			= 100; 							/**< \brief nbr of char per line */
	uint32_t			_line_configMaxSize = 100; 							/**< \brief  */
	uint32_t			_line_total 				= 0; 								/**< \brief nbr de ligne contenu dans le fichier */
	const char * 	_path 							= "/myFile_2.txt"; 	/**< \brief emplacement du fichier */
	JsonObject		_root; 																	/**< \brief json oject pour la configuration du fichier */

	boolean 			_fileClose 					= true;
	const char * 	_fileMod 						= ""; 
public:
	File _file;

	LH_file(File & f, const char * path);
	LH_file();
	~LH_file();

	void line_normalize(String * s, boolean cr = true);
	void line_normalize(String * s, uint32_t size, boolean cr = true);
	int line_denormalize(String * s);

	boolean newLine(const String & value, boolean sav = false);
	boolean newLine(File * f, const String & value);

	// boolean insertLine(const String & value, const String & lineBefor, const String & lineAfter, uint32_t lineNbr);

	boolean replaceLineValue(const String & value, uint32_t lineNbr, uint8_t charSize);
	boolean replaceLine(const String & value, uint32_t lineNbr);
	boolean replaceLine(File * f, const String & value, uint32_t lineNbr);

	boolean readLines(uint32_t start, uint32_t end, String & result, const String & sep = "\n");
	boolean readLineByte(uint32_t lineNbr, String & result);
	boolean readLine(uint32_t lineNbr, String & result);
	boolean readLine(File * f, uint32_t lineNbr, String & result);

	boolean start(File & f, const char * path);
	boolean start(const char * path);




	void savConfig(File * f);
	boolean savConfig();

	void get_lineTotal(uint32_t & result);
	void get_lineMaxSize(uint32_t & result);

private:
	boolean openConfig();
	boolean readConfig();
	void open(File & f, const char * mod);
	void open(const char * mod);
	void close();

};

/**
 * @class   LH_fileManager
 * 
 * @brief   under construction
 * @see     https://github.com/AdriLighting/
 * 
 * @author  Adrien Grellard 
 * @date    sam. 15 janv. 2022 13:40:41
 */
/*
class LH_fileManager
{
		const char * _path = "/fileManager.json";
public:
	LH_fileManager();
	~LH_fileManager(){};
	bool deserializeConfigFile();
	void update(const char * path, uint32_t line_sizeMax, uint32_t line_total);
	void sav();
	void deleteObj(const char * path);
};
*/

#endif // LITTLEFS_HELPER_H