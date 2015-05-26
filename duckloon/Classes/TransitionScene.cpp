#include "TransitionScene.h"
#include "LevelDemoScene.h"

#include "global.h"

#include <iostream>
#include <fstream>
#include <vector>

USING_NS_CC;

ui::EditBox* _editName;
//4D974-9QX42-9Y43G-YJ7JG-JDYBP

/*
Name: createScene
Params: None
Returns: A pointer to a Scene object
*/
Scene* Transition::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	Transition* layer = Transition::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool Transition::init()
{

	//////////////////////////////
	// 1. Initialize
	if (!Layer::init())
	{
		//glClearColor(0.0, 0.0, 0.0, 1.0);
		return false;
	}
	_startGame = false;

	//grabs size of the screen which is used for positioning the background, menu, player, etc.
	Size visibleSize = Director::getInstance()->getVisibleSize(); //width=960 height=640
	Vec2 origin = Director::getInstance()->getVisibleOrigin();  //origin is x=0 y=0

	std::string pNormalSprite = "extensions/green_edit.png";
	_editName = ui::EditBox::create(cocos2d::Size(visibleSize.width / 2, visibleSize.height * 3 / 4), ui::Scale9Sprite::create(pNormalSprite));
	_editName->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height * 3 / 4));
	_editName->setFontName("Paint Boy");
	_editName->setFontSize(25);
	_editName->setFontColor(Color3B::RED);
	_editName->setPlaceHolder("Name:");
	_editName->setPlaceholderFontColor(Color3B::WHITE);
	_editName->setMaxLength(8);
	_editName->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	addChild(_editName);
	
	// add a menu item with "X" image, which is clicked to quit the program.  it's an autorelease object
	auto closeItem = MenuItemImage::create("pics/menu/CloseNormal.png",
		"pics/menu/CloseSelected.png",
		CC_CALLBACK_1(Transition::BeginGame, this));
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width / 2,
		origin.y + closeItem->getContentSize().height / 2));

	auto menu = Menu::create(closeItem, NULL); // create menu
	menu->setPosition(Vec2::ZERO); //bottom right of the screen
	this->addChild(menu, 2);

	std::wstring error;
	FMOD::Channel *channel;
	g_FMODResult = g_FMODInitparams.system->playSound(FMOD_CHANNEL_REUSE, g_Sounds["Sound/Duckloon_Level_1_Forest_MUSIC_MIX.mp3"], false, &g_channels["BGM"]);

	std::wstringstream ssErrors;
	if (!FMODERRORCHECK(g_FMODResult, error))
	{
		ssErrors << error << std::endl;
		error = ssErrors.str();
		CCLOG("Playing sound: %s\n", error);
		return false;
	}

	this->scheduleUpdate();

	return true;
}

void Transition::BeginGame(Ref* pSender) {
	_startGame = true;
	Constant::userID = _editName->getText();
}

void Transition::update(float delta) {
	Node::update(delta);
	if (_startGame)
	{
		CCLOG("Level %d", Constant::level);

		auto scene = LevelDemo::createScene();
		Director::getInstance()->pushScene(scene);
	}
	
}
