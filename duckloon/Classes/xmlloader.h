#include <Windows.h>
#include <vector>
#include <map>
#include "pugixml.hpp"

class XMLLoader
{
public:
	~XMLLoader(){}

	/*
	Name: getInstance
	Params: None
	Returns: An instance of the XMLLoader; if an instance already exists, it returns that instance
	*/
	static XMLLoader& getInstance()
	{
		static XMLLoader instance;
		return instance;
	}

	std::vector<std::string> xmls;

	enum eLang
	{
		LANGUAGE_NONE = 0,
		LANGUAGE_ENGLISH,
		LANGUAGE_SPANISH,
		LANGUAGE_FRENCH,
		LANGUAGE_ITALIAN
	};

	class Language
	{
	public:
		Language():
			langStrings(0)
		{}

		std::vector<std::string> langStrings; //The language strings associated with the language
		pugi::xml_document* doc = new pugi::xml_document(); //The document containing the language data
	};

	std::map<eLang, Language> langs;
	eLang CURRENT_LANG = LANGUAGE_ENGLISH;
private:
	XMLLoader() :
		xmls(0)
	{}
};

/*
Name: LoadXML
Params: A reference to the XMLLoader instance.
Returns: void
Purpose: Opens the XML files and loads the content into the XML loader for use throughout the game.
*/
static void LoadXML(XMLLoader& xmlLoader)
{
	//Load XML files
	xmlLoader = XMLLoader::getInstance();

	xmlLoader.xmls.push_back("english.xml");
	xmlLoader.xmls.push_back("spanish.xml");
	xmlLoader.xmls.push_back("french.xml");
	xmlLoader.xmls.push_back("italian.xml");

	pugi::xml_parse_result englishResult = xmlLoader.langs[XMLLoader::LANGUAGE_ENGLISH].doc->load_file(xmlLoader.xmls[0].c_str());
	pugi::xml_parse_result spanishResult = xmlLoader.langs[XMLLoader::LANGUAGE_SPANISH].doc->load_file(xmlLoader.xmls[1].c_str());
	pugi::xml_parse_result frenchResult = xmlLoader.langs[XMLLoader::LANGUAGE_FRENCH].doc->load_file(xmlLoader.xmls[2].c_str());
	pugi::xml_parse_result italianResult = xmlLoader.langs[XMLLoader::LANGUAGE_ITALIAN].doc->load_file(xmlLoader.xmls[3].c_str());

	
	if (!englishResult)
	{
		OutputDebugStringW(L"Unable to open english xml file");
		exit(EXIT_FAILURE);
	}
	if (!spanishResult)
	{
		OutputDebugStringW(L"Unable to open spanish xml file");
		exit(EXIT_FAILURE);
	}
	if (!frenchResult)
	{
		OutputDebugStringW(L"Unable to open french xml file");
		exit(EXIT_FAILURE);
	}
	if (!italianResult)
	{
		OutputDebugStringW(L"Unable to open italian xml file");
		exit(EXIT_FAILURE);
	}

	std::map<XMLLoader::eLang, XMLLoader::Language>::iterator gmIt;

	for (gmIt = xmlLoader.langs.begin(); gmIt != xmlLoader.langs.end(); gmIt++)
	{
		pugi::xml_node language = gmIt->second.doc->child("Language");

		std::string temp;

		for (pugi::xml_node_iterator it = language.begin(); it != language.end(); it++)
		{
			//temp = it->name();
			
				temp = it->child_value();
				gmIt->second.langStrings.push_back(temp);
			
		}
	}
}