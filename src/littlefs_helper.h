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

void l_1();
void SPIFFS_fileRead(const String &ret);

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
 * @brief   class
 * @see     https://github.com/AdriLighting/
 * 
 * @author  Adrien Grellard 
 * @date    sam. 15 janv. 2022 13:40:41
 */
class LH_file
{
	LH_STATU 			_STATU 							= LHS_OK; 					/**< \brief  */
	uint32_t			_line_maxSize 			= 100; 							/**< \brief  */
	uint32_t			_line_configMaxSize = 100; 							/**< \brief  */
	uint32_t			_line_total 				= 0; 								/**< \brief  */
	const char * 	_path 							= "/myFile_2.txt"; 	/**< \brief  */
	JsonObject		_root; 																	/**< \brief  */
public:


	LH_file();
	~LH_file(){};

	void line_normalize(String * s, boolean cr = true);
	void line_normalize(String * s, uint32_t size);
	int line_denormalize(String * s);

	boolean newLine(const String & value, boolean sav = true);
	boolean newLine(File * f, const String & value);

	boolean replaceLine(const String & value, uint32_t lineNbr, boolean sav = true);
	boolean readLine(uint32_t lineNbr, boolean sav = true);

	void open(const char * path);
	boolean openConfig();
	void updateConfig();
	void savConfig(File * f);
	boolean readConfig();

};


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

#endif // LITTLEFS_HELPER_H