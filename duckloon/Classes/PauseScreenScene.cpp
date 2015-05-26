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
Scene* PauseScreen::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	PauseScreen* layer = PauseScreen::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

/*
Name: CreateMenu
Params: An int representing the language index assigned to taht menu
		An int representing the language index for the currently selected language for display
Returns: void
*/
void PauseScreen::CreateMenu(int tag, int curLanguage)
{

	//grabs size of the screen which is used for positioning the background, menu, player, etc.
	Size visibleSize = Director::getInstance()->getVisibleSize(); //width=960 height=640
	Vec2 origin = Director::getInstance()->getVisibleOrigin();  //origin is x=0 y=0


	/////////////////////////////
	// 5. add a menu item with "X" image, which is clicked to quit the program.  it's an autorelease object
	auto menu_item_0 = MenuItemFont::create(xmlLoader.langs[xmlLoader.CURRENT_LANG].langStrings[9], CC_CALLBACK_1(PauseScreen::PopScene, this));
	auto menu_item_1 = MenuItemFont::create(xmlLoader.langs[xmlLoader.CURRENT_LANG].langStrings[10], CC_CALLBACK_1(PauseScreen::PopScene, this));
	auto menu_item_2 = MenuItemFont::create(xmlLoader.langs[xmlLoader.CURRENT_LANG].langStrings[11], CC_CALLBACK_1(PauseScreen::menuCloseCallback, this));

	menu_item_0->setPosition(Point(visibleSize.width / 2, (visibleSize.height / 4) * 3.5));
	menu_item_1->setPosition(Point(visibleSize.width / 2, (visibleSize.height / 4) * 2.5));
	menu_item_2->setPosition(Point(visibleSize.width / 2, (visibleSize.height / 4) * 2));
	auto *menu = Menu::create(menu_item_0, menu_item_1, menu_item_2, NULL);
	menu->setPosition(Point(0, 0));
	this->addChild(menu);

	menu->setTag(tag);
	if (curLanguage != tag)
		menu->setVisible(false);
}

// on "init" you need to initialize your instance
bool PauseScreen::init()
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
Name: PopScene
Params: A pointer to a cocos2d::Ref
Returns: void
Purpose: Pops the current scene off the stack and reveals the previous menu
*/
void PauseScreen::PopScene(cocos2d::Ref *pSender) {
	Director::getInstance()->popScene();
}

/*
Name: isKeyPressed
Params: An EventKeyboard::KeyCode object representing the keycode being checked
Returns: A bool representing whether or not the key has been pressed
*/
bool PauseScreen::isKeyPressed(EventKeyboard::KeyCode code) {
	// Check if the key is currently pressed by seeing if it's in the std::map keys
	if (keys.find(code) != keys.end())
		return true;
	return false;
}

void PauseScreen::update(float delta) {
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
	else if (isKeyPressed(EventKeyboard::KeyCode::KEY_2)) {
		xmlLoader.CURRENT_LANG = XMLLoader::LANGUAGE_SPANISH;
		engMenu->setVisible(false);
		spaMenu->setVisible(true);
		freMenu->setVisible(false);
		itaMenu->setVisible(false);
	}
	else if (isKeyPressed(EventKeyboard::KeyCode::KEY_3)) {
		xmlLoader.CURRENT_LANG = XMLLoader::LANGUAGE_FRENCH;
		engMenu->setVisible(false);
		spaMenu->setVisible(false);
		freMenu->setVisible(true);
		itaMenu->setVisible(false);
	}
	else if (isKeyPressed(EventKeyboard::KeyCode::KEY_4)) {
		xmlLoader.CURRENT_LANG = XMLLoader::LANGUAGE_ITALIAN;
		engMenu->setVisible(false);
		spaMenu->setVisible(false);
		freMenu->setVisible(false);
		itaMenu->setVisible(true);
	}
	else if (isKeyPressed(EventKeyboard::KeyCode::KEY_P)) {
		Director::getInstance()->popScene();
	}
}

// Because cocos2d-x requres createScene to be static, we need to make other non-pointer members static
std::map<cocos2d::EventKeyboard::KeyCode,
	std::chrono::high_resolution_clock::time_point> PauseScreen::keys;

//handles closing the program
void PauseScreen::menuCloseCallback(Ref* pSender)
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
