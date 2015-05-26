#include "OpeningMenuScene.h"
#include "LevelDemoScene.h"
#include "OptionMenuScene.h"
#include "OpeningStoryScene.h"
#include "PauseScreenScene.h"

#include "global.h"

#include <iostream>
#include <fstream>
#include <vector>

USING_NS_CC;

//4D974-9QX42-9Y43G-YJ7JG-JDYBP

/*
Name: createScene
Params: None
Returns: A pointer to a Scene object
*/
Scene* OpeningMenu::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	OpeningMenu* layer = OpeningMenu::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

/*
Name: LoadJSONFileLevel
Params: A String representing the name of the JSON file
A referenced rapidjson::Document to load the json information into.
Returns: A bool representing whether or not the json file successfully loaded.
*/
//https://github.com/miloyip/rapidjson
bool OpeningMenu::LoadJSONFileLevel(std::string fileName, rapidjson::Document &document)
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

	if (document.Parse(jsonStr.c_str()).HasParseError()){	// Parse JSON string into DOM.
		cocos2d::log("Error: JSON File could not be parsed into document");
		return false;
	}
	return true;
}

/*
Name: CreateMenu
Params: An int representing the language index assigned to taht menu
		An int representing the language index for the currently selected language for display
Returns: void
*/
void OpeningMenu::CreateMenu(int tag, int curLanguage)
{
	//grabs size of the screen which is used for positioning the background, menu, player, etc.
	Size visibleSize = Director::getInstance()->getVisibleSize(); //width=960 height=640
	Vec2 origin = Director::getInstance()->getVisibleOrigin();  //origin is x=0 y=0

	/////////////////////////////
	// 5. add a menu item with "X" image, which is clicked to quit the program.  it's an autorelease object
	auto menu_item_0 = MenuItemFont::create(xmlLoader.langs[xmlLoader.CURRENT_LANG].langStrings[1], CC_CALLBACK_1(OpeningMenu::NewGame, this));
	auto menu_item_1 = MenuItemFont::create(xmlLoader.langs[xmlLoader.CURRENT_LANG].langStrings[2], CC_CALLBACK_1(OpeningMenu::NewGame, this));
	auto menu_item_2 = MenuItemFont::create(xmlLoader.langs[xmlLoader.CURRENT_LANG].langStrings[3], CC_CALLBACK_1(OpeningMenu::Options, this));
	auto menu_item_3 = MenuItemFont::create(xmlLoader.langs[xmlLoader.CURRENT_LANG].langStrings[4], CC_CALLBACK_1(OpeningMenu::menuCloseCallback, this));

	auto menu_item_4 = MenuItemFont::create(xmlLoader.langs[xmlLoader.CURRENT_LANG].langStrings[12], CC_CALLBACK_1(OpeningMenu::NewGame, this));
	auto menu_item_5 = MenuItemFont::create(xmlLoader.langs[xmlLoader.CURRENT_LANG].langStrings[13], CC_CALLBACK_1(OpeningMenu::NewGame, this));

	menu_item_0->setPosition(Point(visibleSize.width / 2, (visibleSize.height / 4) * 3.5));
	menu_item_1->setPosition(Point(visibleSize.width / 2, (visibleSize.height / 4) * 2.5));
	menu_item_2->setPosition(Point(visibleSize.width / 2, (visibleSize.height / 4) * 2));
	menu_item_3->setPosition(Point(visibleSize.width / 2, (visibleSize.height / 4) * 1.5));

	menu_item_4->setPosition(Point(visibleSize.width / 2, (visibleSize.height / 4) * 0.75));
	menu_item_5->setPosition(Point(visibleSize.width / 2, (visibleSize.height / 4) * 0.5));
	auto *menu = Menu::create(menu_item_0, menu_item_1, menu_item_2, menu_item_3, menu_item_4, menu_item_5, NULL);
	menu->setPosition(Point(0, 0));
	this->addChild(menu);

	menu->setTag(tag);
	if (curLanguage != tag)
		menu->setVisible(false);
}

// on "init" you need to initialize your instance
bool OpeningMenu::init()
{

	//////////////////////////////
	// 1. Initialize
	if (!Layer::init())
	{
		//glClearColor(0.0, 0.0, 0.0, 1.0);
		return false;
	}

	int curLanguage = 0;
	if (xmlLoader.CURRENT_LANG == XMLLoader::LANGUAGE_ENGLISH)
		curLanguage = 0;
	if (xmlLoader.CURRENT_LANG == XMLLoader::LANGUAGE_SPANISH)
		curLanguage = 1;
	if (xmlLoader.CURRENT_LANG == XMLLoader::LANGUAGE_FRENCH)
		curLanguage = 2;
	if (xmlLoader.CURRENT_LANG == XMLLoader::LANGUAGE_ITALIAN)
		curLanguage = 3;

	xmlLoader.CURRENT_LANG = XMLLoader::LANGUAGE_ENGLISH;
	CreateMenu(0, curLanguage);
	xmlLoader.CURRENT_LANG = XMLLoader::LANGUAGE_SPANISH;
	CreateMenu(1, curLanguage);
	xmlLoader.CURRENT_LANG = XMLLoader::LANGUAGE_FRENCH;
	CreateMenu(2, curLanguage);
	xmlLoader.CURRENT_LANG = XMLLoader::LANGUAGE_ITALIAN;
	CreateMenu(3, curLanguage);

	if (curLanguage == 0) xmlLoader.CURRENT_LANG = XMLLoader::LANGUAGE_ENGLISH;
	if (curLanguage == 1) xmlLoader.CURRENT_LANG = XMLLoader::LANGUAGE_SPANISH;
	if (curLanguage == 2) xmlLoader.CURRENT_LANG = XMLLoader::LANGUAGE_FRENCH;
	if (curLanguage == 3) xmlLoader.CURRENT_LANG = XMLLoader::LANGUAGE_ITALIAN;

	auto keyboardListener = EventListenerKeyboard::create();
	Director::getInstance()->getOpenGLView()->setIMEKeyboardState(true);
	keyboardListener->onKeyPressed = [](EventKeyboard::KeyCode keyCode, Event* event){
		// If a key already exists, do nothing as it will already have a time stamp
		// Otherwise, set's the timestamp to now
		if (keys.find(keyCode) == keys.end()){
			keys[keyCode] = std::chrono::high_resolution_clock::now();
		}
	};
	keyboardListener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event){
		//remove the key
		keys.erase(keyCode);
	};

	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	this->scheduleUpdate();

	//this->getChildByName("Player");

	return true;
}

/*
Name: NewGame
Params: A pointer to a cocos2d::Ref
Returns: void
Purpose: Starts a new game when the "New Game" menu option is clicked
*/
void OpeningMenu::NewGame(cocos2d::Ref *pSender) {
	CCLOG("New Game");
	auto scene = OpeningStory::createScene();
	Director::getInstance()->replaceScene(TransitionFade::create(2, scene));
}

/*
Name: Options
Params: A pointer to a cocos2d::Ref
Returns: void
Purpose: Opens the game options when the "Options" menu option is clicked
*/
void OpeningMenu::Options(cocos2d::Ref *pSender) {
	CCLOG("Options");
	auto scene = OptionMenu::createScene();
	Director::getInstance()->pushScene(scene);
}

/*
Name: isKeyPressed
Params: An EventKeyboard::KeyCode object representing the keycode being checked
Returns: A bool representing whether or not the key has been pressed
*/
bool OpeningMenu::isKeyPressed(EventKeyboard::KeyCode code) {
	// Check if the key is currently pressed by seeing if it's in the std::map keys
	if (keys.find(code) != keys.end())
		return true;
	return false;
}

void OpeningMenu::update(float delta) {
	Node::update(delta);
	Menu* engMenu = (Menu*)(this->getChildByTag(0));
	Menu* spaMenu = (Menu*)(this->getChildByTag(1));
	Menu* freMenu = (Menu*)(this->getChildByTag(2));
	Menu* itaMenu = (Menu*)(this->getChildByTag(3));

	if (isKeyPressed(EventKeyboard::KeyCode::KEY_1)) {
		xmlLoader.CURRENT_LANG = XMLLoader::LANGUAGE_ENGLISH;
		engMenu->setVisible(true);
		spaMenu->setVisible(false);
		freMenu->setVisible(false);
		itaMenu->setVisible(false);
	}
	if (isKeyPressed(EventKeyboard::KeyCode::KEY_2)) {
		xmlLoader.CURRENT_LANG = XMLLoader::LANGUAGE_SPANISH;
		engMenu->setVisible(false);
		spaMenu->setVisible(true);
		freMenu->setVisible(false);
		itaMenu->setVisible(false);
	}
	if (isKeyPressed(EventKeyboard::KeyCode::KEY_3)) {
		xmlLoader.CURRENT_LANG = XMLLoader::LANGUAGE_FRENCH;
		engMenu->setVisible(false);
		spaMenu->setVisible(false);
		freMenu->setVisible(true);
		itaMenu->setVisible(false);
	}
	if (isKeyPressed(EventKeyboard::KeyCode::KEY_4)) {
		xmlLoader.CURRENT_LANG = XMLLoader::LANGUAGE_ITALIAN;
		engMenu->setVisible(false);
		spaMenu->setVisible(false);
		freMenu->setVisible(false);
		itaMenu->setVisible(true);
	}
	if (isKeyPressed(EventKeyboard::KeyCode::KEY_P)) {
		auto scene = PauseScreen::createScene();
		Director::getInstance()->pushScene(scene);
	}
}

// Because cocos2d-x requres createScene to be static, we need to make other non-pointer members static
std::map<cocos2d::EventKeyboard::KeyCode,
	std::chrono::high_resolution_clock::time_point> OpeningMenu::keys;

//handles closing the program
void OpeningMenu::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.", "Alert");
	return;
#endif

	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}
