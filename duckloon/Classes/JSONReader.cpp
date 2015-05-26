#include <iostream>
#include <fstream>
#include "JSONReader.h"

JSONReader::JSONReader()
{
	
}

JSONReader::~JSONReader()
{

}

/*
Name: LoadJSONFileLevel
Params: A String representing the name of the JSON file
A referenced rapidjson::Document to load the json information into.
Returns: A bool representing whether or not the json file successfully loaded.
*/
//https://github.com/miloyip/rapidjson/blob/master/example/tutorial/tutorial.cpp tutorial
bool JSONReader::LoadJSONFileLevel(std::string fileName)
{
	std::ifstream file;
	file.open(fileName, std::ifstream::binary);	//open the input file

	if (!file.is_open()){						//file not found
		cocos2d::log("Error: JSON File not found");
		return false;
	}
	std::stringstream strStream;
	strStream << file.rdbuf();						//read the file
	std::string jsonStr = strStream.str();			//str holds the content of the file

	if (this->document.Parse(jsonStr.c_str()).HasParseError()){	// Parse JSON string into DOM.
		cocos2d::log("Error: JSON File could not be parsed into document");
		return false;
	}

	if (!this->document.IsObject()){	// Parse JSON string into DOM.
		cocos2d::log("Error: JSON document is not object");
		return false;
	}
	return true;
}