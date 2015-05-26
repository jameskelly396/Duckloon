#ifndef __JSONReader_H__
#define __JSONReader_H__

#include "cocos2d.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

USING_NS_CC;

class JSONReader
{
public:
	JSONReader();
	~JSONReader();

	bool LoadJSONFileLevel(std::string fileName);

	rapidjson::Document document;
};
#endif