#include "LevelDemoScene.h"
#include "PEShapeCache_X3_0.h"
#include "JSONReader.h"

#include "global.h"

#include <iostream>
#include <fstream>

#include "sqlite3.h"

FMODInitParams _FMODInitparams;
FMOD_RESULT _FMODResult;

const char* _soundName;

USING_NS_CC;

/*
Name: createScene
Params: None
Returns: A pointer to a Scene object
*/
Scene* LevelDemo::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::createWithPhysics();
	
	// 'layer' is an autorelease object
	LevelDemo* layer = LevelDemo::create();

	// add layer as a child to scene
	scene->addChild(layer);
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	scene->getPhysicsWorld()->setSubsteps(2);
	

	// return the scene
	return scene;
}


int playSound(lua_State *L) { 
	const char* soundName = lua_tostring(L, 1);
	_soundName = soundName;
	CCLOG("Playing sound: %s\n", soundName);

	std::wstring error;
	FMOD::Channel *channel;
	g_FMODResult = g_FMODInitparams.system->playSound(FMOD_CHANNEL_REUSE, g_Sounds[soundName], false, &channel);

	std::wstringstream ssErrors;
	if (!FMODERRORCHECK(g_FMODResult, error))
	{
		ssErrors << error << std::endl;
		error = ssErrors.str();
		CCLOG("Error Audio: %s \n", error);
		return false;
	}

	return 0;
}


// on "init" you need to initialize your instance
bool LevelDemo::init()
{
	//////////////////////////////
	// 1. Initialize
	if (!Layer::init())
	{
		//glClearColor(0.0, 0.0, 0.0, 1.0);
		return false;
	}

	g_p_GameControllerManager = CGameControllerManager::getGameControllerManager();
	g_p_GameController_0 = g_p_GameControllerManager->getController(0);

	////http://www.lua.org/pil/2.4.html
	lua_State *lua_state;
	lua_state = luaL_newstate();
	luaL_openlibs(lua_state);

	lua_pushcfunction(lua_state, playSound);
	lua_setglobal(lua_state, "playSound");

	if (luaL_loadfile(lua_state, "fmod.lua") != LUA_ERRFILE){
		lua_newtable(lua_state);
		lua_pushstring(lua_state, "area");
		lua_pushstring(lua_state, "player");
		lua_rawset(lua_state, -3);
		lua_setglobal(lua_state, "args");
		lua_pcall(lua_state, 0, LUA_MULTRET, 0);
	}
	lua_close(lua_state);
	
	//g_FMODInitparams.system->update();

	//*****DATABASE*****//
	DBManager::getInstance()->LoadDatabase("duckloonDB.db");

	DBManager::getInstance()->GetHighScores();

	DBManager::getInstance()->CloseDatabase();

	//grabs size of the screen which is used for positioning the background, menu, player, etc.
	Size visibleSize = Director::getInstance()->getVisibleSize(); //width=960 height=640
	Vec2 origin = Director::getInstance()->getVisibleOrigin();  //origin is x=0 y=0

	// Read and Parse JSON file into DOM.
	JSONReader* jsonReader = new JSONReader();
	if (!jsonReader->LoadJSONFileLevel("json/test.json"))
		return false;

	/////////////////////////////
	// 2. Load in the level
	//http://gamedevelopment.tutsplus.com/tutorials/parsing-tiled-tmx-format-maps-in-your-own-game-engine--gamedev-3104
	//http://www.raywenderlich.com/39113/cocos2d-x-tile-map-tutorial-part-1
	//http://discuss.cocos2d-x.org/t/tutorial-series-use-the-cocos2d-x-3-0-game-engine-write-a-tile-map-game-part05/13126
	assert(jsonReader->document.HasMember("tileMap"));
	assert(jsonReader->document["tileMap"].IsString());
	//std::string file = jsonReader->document["tileMap"].GetString();
	std::string file;
	if (Constant::level == 1)
		file = "TiledTestRes/Level1.tmx";
	else if (Constant::level == 2)
		file = "TiledTestRes/Level2.tmx";
	else
		file = "TiledTestRes/test2.tmx";
	_tileMap = TMXTiledMap::create(file);
	this->addChild(_tileMap);

	//Create boundary around level
	Size MapSize = _tileMap->getMapSize();
	Size TileSize = _tileMap->getTileSize();
	Size OutsideBoundary;
	OutsideBoundary.height = MapSize.height * TileSize.height;
	OutsideBoundary.width = MapSize.width * TileSize.width;
	auto edgeBox = PhysicsBody::createEdgeBox(OutsideBoundary, PHYSICSBODY_MATERIAL_DEFAULT, 3);
	auto edgeNode = Node::create();
	edgeNode->setPosition(Point(OutsideBoundary.width / 2, OutsideBoundary.height / 2));
	edgeNode->setPhysicsBody(edgeBox);
	this->addChild(edgeNode);

	//Spawn Point
	_spawnPoint = _tileMap->objectGroupNamed("SpawnPoint");
	float playerStartingPosX, playerStartingPosY;
	if (_spawnPoint != NULL){
		ValueMap spawnPoint = _spawnPoint->getObject("SpawnPoint");
		playerStartingPosX = spawnPoint["x"].asFloat();
		playerStartingPosY = spawnPoint["y"].asFloat();

		ValueMap exitPoint = _spawnPoint->getObject("Exit");
		std::string exit_name = exitPoint["name"].asString();
		float playerExitPosX = exitPoint["x"].asFloat();
		float playerExitPosY = exitPoint["y"].asFloat();
		float exit_width = exitPoint["width"].asFloat();
		float exit_height = exitPoint["height"].asFloat();

		CCParticleFlower* exitsprite = CCParticleFlower::create();
		exitsprite->setPosition(Vec2(playerExitPosX + (exit_width * 0.5), playerExitPosY));
		exitsprite->setTag(9);  
		exitsprite->setName(exit_name);
		auto exitbody = PhysicsBody::createBox(Size(exit_width, exit_height + (exit_height * 0.5)), PhysicsMaterial(0, 0, 1));
		exitbody->setDynamic(false);
		exitbody->setGravityEnable(false);
		exitbody->setCategoryBitmask(0x01);
		exitbody->setCollisionBitmask(0x01);
		exitbody->setContactTestBitmask(0xFFFFFFFF);
		exitsprite->setPhysicsBody(exitbody);
		this->addChild(exitsprite);
	}
	else{
		const rapidjson::Value& playerInfo = jsonReader->document["Player"]; // Using a reference for consecutive access is handy and faster.
		assert(playerInfo.IsObject());
		assert(playerInfo.HasMember("StartingPosX"));
		assert(playerInfo["StartingPosX"].IsDouble());
		double playerPosX = playerInfo["StartingPosX"].GetDouble();
		double playerPosY = playerInfo["StartingPosY"].GetDouble();
		playerStartingPosX = visibleSize.width / playerPosX + origin.x;
		playerStartingPosY = visibleSize.height / playerPosY + origin.y;
	}

	//Platforms
	_collision = _tileMap->objectGroupNamed("Collision");
	ValueVector vec_obj = _collision->getObjects();
	for (int i = 0; i < vec_obj.size(); i++){
		ValueMap map_obj = vec_obj[i].asValueMap();
		float xpos = map_obj["x"].asFloat();
		float ypos = map_obj["y"].asFloat();
		float map_width = map_obj["width"].asFloat();
		float map_height = map_obj["height"].asFloat();

		auto mapbody = PhysicsBody::createBox(Size(map_width, map_height), PhysicsMaterial(0, 0, 1));
		mapbody->setDynamic(false);
		mapbody->setGravityEnable(false);
		mapbody->setCategoryBitmask(0x01);
		mapbody->setCollisionBitmask(0x01);
		mapbody->setContactTestBitmask(0xFFFFFFFF);
		

		auto mapSprite = Sprite::create("pics/water.png");
		mapSprite->setVisible(false);
		mapSprite->setPosition(Vec2(xpos + (map_width * 0.5), ypos + (map_height * 0.5)));
		mapSprite->setPhysicsBody(mapbody);
		this->addChild(mapSprite);
	}

	//Removable Hazards
	_hazards = _tileMap->objectGroupNamed("Hazards");
	ValueVector vec_haz = _hazards->getObjects();
	for (int i = 0; i < vec_haz.size(); i++){
		ValueMap map_obj = vec_haz[i].asValueMap();
		std::string haz_name = map_obj["name"].asString();
		if (haz_name == "haz_Fire"){
			float xpos = map_obj["x"].asFloat();
			float ypos = map_obj["y"].asFloat();
			float haz_width = map_obj["width"].asFloat();
			float haz_height = map_obj["height"].asFloat();

			
			//load fire barrier that can be put out when the player shoots water at it
			//CCParticleRain* firesprite = CCParticleRain::create();
			CCParticleFire* firesprite = CCParticleFire::create();
			firesprite->setPosition(Vec2(xpos + (haz_width * 0.5), ypos ));
			firesprite->setTag(100 + i);  //leaves 20 IDs from 0 to 19 for non hazards to use
			firesprite->setName(haz_name);
			auto firebody = PhysicsBody::createBox(Size(haz_width, haz_height + (haz_height * 0.5)), PhysicsMaterial(0, 0, 1));
			firebody->setDynamic(false);
			firebody->setGravityEnable(false);
			firebody->setCategoryBitmask(0x01);
			firebody->setCollisionBitmask(0x01);
			firebody->setContactTestBitmask(0xFFFFFFFF);
			firesprite->setPhysicsBody(firebody);
			this->addChild(firesprite);
		}
	}

	//Substance locations that can be absorbed
	_substances = _tileMap->objectGroupNamed("Substances");
	ValueVector vec_sub = _substances->getObjects();
	for (int i = 0; i < vec_sub.size(); i++){
		ValueMap map_obj = vec_sub[i].asValueMap();
		std::string sub_name = map_obj["name"].asString();
		if (sub_name == "Water"){
			float xpos = map_obj["x"].asFloat();
			float ypos = map_obj["y"].asFloat();

			//create a water particle. If the player comes into contact with it they get one shot of water
			PEShapeCache::getInstance()->addBodysWithFile("Physics/water.plist");
			auto waterSprite = Sprite::create("pics/water.png");
			waterSprite->setName("sub_Water");
			waterSprite->setTag(200 + i);
			waterSprite->setPosition(Vec2(xpos, ypos));

			auto waterbody = PEShapeCache::getInstance()->getPhysicsBodyByName("water");
			waterSprite->setPhysicsBody(waterbody);
			waterbody->setCategoryBitmask(0x01);
			waterbody->setCollisionBitmask(0x01);
			waterbody->setContactTestBitmask(0xFFFFFFFF);
			this->addChild(waterSprite, 0);
		}
	}

	// renders background images
	assert(jsonReader->document.HasMember("backgroundPic"));
	assert(jsonReader->document["backgroundPic"].IsString());
	std::string backgroundPic;
	backgroundPic = jsonReader->document["backgroundPic"].GetString();
	_backgroundSprite = Sprite::create(backgroundPic);
	_backgroundSprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y)); //center of screen
	this->addChild(_backgroundSprite, -1);


	/////////////////////////////
	// 3. Create player and interactive sprites
	const rapidjson::Value& playerInfo = jsonReader->document["Player"]; // Using a reference for consecutive access is handy and faster.
	assert(playerInfo.IsObject());
	assert(playerInfo.HasMember("playerPhysics"));
	assert(playerInfo["playerPhysics"].IsString());
	std::string playerPhysics = playerInfo["playerPhysics"].GetString();

	PEShapeCache::getInstance()->addBodysWithFile(playerPhysics);
	_player = Sprite::create();
	_player->setName("Player");//used for future references to this sprite
	_player->setTag(0);
	_player->setPosition(Vec2(playerStartingPosX, playerStartingPosY)); //center of screen

	//creates physicsbody for the player
	//auto playerbody = PEShapeCache::getInstance()->getPhysicsBodyByName("Duck01");
	auto playerbody = PhysicsBody::createBox(Size(visibleSize.width * 0.25 + origin.x, visibleSize.height * 0.5 + origin.x), PhysicsMaterial(1, 0, 1));
	_player->setPhysicsBody(playerbody);
	playerbody->setCategoryBitmask(0x01);
	playerbody->setCollisionBitmask(1);
	playerbody->setContactTestBitmask(true);
	playerbody->setGravityEnable(true);
	playerbody->setRotationEnable(false);

	//load animation files
	Vector<SpriteFrame*> animFrames_walking;
	//load player pics
	std::vector<std::string> vec_playerWalking;
	{
		const rapidjson::Value& a = jsonReader->document["pic_playerWalking"]; // Using a reference for consecutive access is handy and faster.
		assert(a.IsArray());
		for (rapidjson::SizeType i = 0; i < a.Size(); i++) // rapidjson uses SizeType instead of size_t.
		{
			assert(a[i].HasMember("playerWalking"));
			std::string playerWalkingPic;
			playerWalkingPic = a[i]["playerWalking"].GetString();
			auto frame = SpriteFrame::create(playerWalkingPic, Rect(0, 0, 295, 358)); //we assume that the sprites' dimentions are 40*40 rectangles.
			animFrames_walking.pushBack(frame);
			vec_playerWalking.push_back(playerWalkingPic);
		}
	}
	auto animation = Animation::createWithSpriteFrames(animFrames_walking, 0.2f);
	auto animate = RepeatForever::create(Animate::create(animation));
	_player->runAction(animate);


	_player->setScale(0.2f);
	this->addChild(_player); //sprite added to layer 0


	//Sets the current state of the player
	_playerState = new CPlayerState();
	_playerState->currentPlayerMode = CPlayerState::FALLING;


	//load enemy pics
	std::vector<std::string> vec_enemyPics;
	std::vector<double> vec_enemyPosX;
	{
		const rapidjson::Value& a = jsonReader->document["enemies"]; // Using a reference for consecutive access is handy and faster.
		assert(a.IsArray());
		for (rapidjson::SizeType i = 0; i < a.Size(); i++) // rapidjson uses SizeType instead of size_t.
		{
			assert(a[i].HasMember("enemyPic"));
			std::string enemyPic;
			enemyPic = a[i]["enemyPic"].GetString();
			vec_enemyPics.push_back(enemyPic);

			double enemyPosX;
			assert(a[i].HasMember("PositionX")); //loads the enemies positions into the file
			assert(a[i]["PositionX"].IsDouble());
			enemyPosX = a[i]["PositionX"].GetDouble();
			vec_enemyPosX.push_back(enemyPosX);
		}
	}

	for (int i = 0; i < vec_enemyPics.size(); i++)
	{
		auto enemySprite = Sprite::create(vec_enemyPics[i]);
		enemySprite->setPosition(Vec2(visibleSize.width * vec_enemyPosX[i] + origin.x, visibleSize.height / 2 + origin.y)); //center of screen
		this->addChild(enemySprite); //sprite added to layer 0
	}

	/////////////////////////////
	// 4. Additional Labels
	// add a label shows "Level Demo"
	assert(jsonReader->document.HasMember("levelName"));
	assert(jsonReader->document["levelName"].IsString());
	std::string levelName;
	levelName = jsonReader->document["levelName"].GetString();
	auto headerLabel = Label::createWithTTF(levelName, "fonts/Marker Felt.ttf", 24);
	headerLabel->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - headerLabel->getContentSize().height)); //top center of the screen
	headerLabel->setName("LevelDemo");
	_scrollNames.push_back(headerLabel->getName());
	this->addChild(headerLabel, 1);

	std::string substanceName = "EMPTY";
	_substanceLabel = Label::createWithTTF(substanceName, "fonts/Marker Felt.ttf", 24);
	_substanceLabel->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - _substanceLabel->getContentSize().height)); //top center of the screen
	_substanceLabel->setName("SubstanceHUD");
	//_scrollNames.push_back(_substanceLabel->getName());
	this->addChild(_substanceLabel, 1);

	// add a menu item with "X" image, which is clicked to quit the program.  it's an autorelease object
	auto closeItem = MenuItemImage::create("pics/menu/CloseNormal.png",
		"pics/menu/CloseSelected.png",
		CC_CALLBACK_1(LevelDemo::menuCloseCallback, this));
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width / 2,
		origin.y + closeItem->getContentSize().height / 2));

	auto menu = Menu::create(closeItem, NULL); // create menu
	menu->setPosition(Vec2::ZERO); //bottom right of the screen
	//menu->setName("Menu");
	//_scrollNames.push_back(menu->getName());
	this->addChild(menu, 2);

	/////////////////////////////
	// 5. set up listeners
	auto physicsListener = EventListenerPhysicsContact::create();
	physicsListener->onContactBegin = CC_CALLBACK_1(LevelDemo::onContactBegin, this);
	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(physicsListener, this);

	_newKeyboard = Keyboard::getInstance();

	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(_newKeyboard->keyboardListener, this);
	
	this->_start = std::clock();
	this->scheduleUpdate();


	return true;
}


void LevelDemo::update(float delta) {
	// Register an update function that checks to see if specific keys are pressed
	// and if it is displays how long, otherwise tell the user to press it
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Node::update(delta);
	g_FMODInitparams.system->update();
	::g_p_GameController_0->Update(delta);

	Point playerPos = _player->getPosition();
	this->setViewPointCenter(playerPos);

	lua_State *lua_state;
	lua_state = luaL_newstate();
	luaL_openlibs(lua_state);

	lua_pushcfunction(lua_state, playSound);
	lua_setglobal(lua_state, "playSound");

	float leftJoyX = ::g_p_GameController_0->getLeftJoystickXAxis();
	if (_newKeyboard->isKeyPressed(EventKeyboard::KeyCode::KEY_LEFT_ARROW) || leftJoyX < 0.0) { //move left
		float posX = _player->getPositionX();
		_player->setPositionX(posX - (visibleSize.width / 7 * delta));
		_player->resumeSchedulerAndActions(); //running animation
		if (!_player->isFlippedX()) //turns picture around to face left
			_player->setFlippedX(true);
	}
	else if (_newKeyboard->isKeyPressed(EventKeyboard::KeyCode::KEY_RIGHT_ARROW) || leftJoyX > 0.0) { //move right
		float posX = _player->getPositionX();
		_player->setPositionX(posX + (visibleSize.width / 7 * delta));
		_player->resumeSchedulerAndActions(); //running animation
		if (_player->isFlippedX()) //turns picture around to face right
			_player->setFlippedX(false);
	}
	else {
		_player->pauseSchedulerAndActions();
	}


	if ((_newKeyboard->isKeyPressed(EventKeyboard::KeyCode::KEY_SPACE) || ::g_p_GameController_0->bIsAButtonDown())
		&& _playerState->currentPlayerMode != CPlayerState::FALLING) { //jump
		_playerState->currentPlayerMode = CPlayerState::JUMPING;
		if (luaL_loadfile(lua_state, "fmod.lua") != LUA_ERRFILE){
			lua_newtable(lua_state);
			lua_pushstring(lua_state, "area");
			if (_playerState->currentHeldSubstance == CPlayerState::WATER)
				lua_pushstring(lua_state, "water_jump");
			else
				lua_pushstring(lua_state, "jump");
			lua_rawset(lua_state, -3);
			lua_setglobal(lua_state, "args");
			lua_pcall(lua_state, 0, LUA_MULTRET, 0);
		}
	}
	else if ((_newKeyboard->isKeyPressed(EventKeyboard::KeyCode::KEY_SPACE) || ::g_p_GameController_0->bIsAButtonDown()) 
		&& _playerState->bHasDoubleJump) {
		if (_playerState->currentHeldSubstance == CPlayerState::AIR || _playerState->currentHeldSubstance == CPlayerState::EMPTY){
			PhysicsBody* physicsBody = _player->getPhysicsBody();
			Vec2 vel = physicsBody->getVelocity();
			if (vel.y < 0.0){
				physicsBody->setVelocity(Vec2(0, visibleSize.height / 5));
				_playerState->bHasDoubleJump = false;
				_playerState->currentHeldSubstance = CPlayerState::AIR;
				_substanceLabel->setString("AIR");
				_substanceLabel->setColor(ccc3(0, 130, 105));
				if (luaL_loadfile(lua_state, "fmod.lua") != LUA_ERRFILE){
					lua_newtable(lua_state);
					lua_pushstring(lua_state, "area");
					lua_pushstring(lua_state, "jump");
					lua_rawset(lua_state, -3);
					lua_setglobal(lua_state, "args");
					lua_pcall(lua_state, 0, LUA_MULTRET, 0);
				}
			}
		}
	}

	if (_newKeyboard->isKeyPressed(EventKeyboard::KeyCode::KEY_R) || ::g_p_GameController_0->bIsBButtonDown()) { //refill duck with air
		_playerState->currentHeldSubstance = CPlayerState::AIR;
		_substanceLabel->setString("AIR");
		_substanceLabel->setColor(ccc3(0, 130, 105));
		if (luaL_loadfile(lua_state, "fmod.lua") != LUA_ERRFILE){
			lua_newtable(lua_state);
			lua_pushstring(lua_state, "area");
			lua_pushstring(lua_state, "refill");
			lua_rawset(lua_state, -3);
			lua_setglobal(lua_state, "args");
			lua_pcall(lua_state, 0, LUA_MULTRET, 0);
		}
	}

	if ((_newKeyboard->isKeyPressed(EventKeyboard::KeyCode::KEY_F) || ::g_p_GameController_0->bIsXButtonDown()) 
		&& _playerState->currentHeldSubstance != CPlayerState::EMPTY) { //shoot substance
		Sprite* projectileSprite;  
		PhysicsBody* projectileBody; 

		//If player is holding water
		if (_playerState->currentHeldSubstance == CPlayerState::WATER){

			if (luaL_loadfile(lua_state, "fmod.lua") != LUA_ERRFILE){
				lua_newtable(lua_state);
				lua_pushstring(lua_state, "area");
				lua_pushstring(lua_state, "water");
				lua_rawset(lua_state, -3);
				lua_setglobal(lua_state, "args");
				lua_pcall(lua_state, 0, LUA_MULTRET, 0);
			}

			projectileSprite = Sprite::create("pics/water.png");
			projectileSprite->setName("proj_Water");
			projectileSprite->setTag(2);
			if (_player->isFlippedX()) //player is facing left
				projectileSprite->setPosition(playerPos + Vec2(-30, 0));
			else
				projectileSprite->setPosition(playerPos + Vec2(30, 0));

			PEShapeCache::getInstance()->addBodysWithFile("Physics/water.plist");
			projectileBody = PEShapeCache::getInstance()->getPhysicsBodyByName("water");
			if (_player->isFlippedX()) //player is facing left
				projectileBody->setVelocity(Vec2(-visibleSize.width / 5, 0));
			else
				projectileBody->setVelocity(Vec2(visibleSize.width / 5, 0));
			this->removeChildByTag(2);
		}

		//If player is holding air, Air currently does nothing but spit out an ugly dust picture. to use air press 'R' to inhale then 'F' to fire
		if (_playerState->currentHeldSubstance == CPlayerState::AIR){

			if (luaL_loadfile(lua_state, "fmod.lua") != LUA_ERRFILE){
				lua_newtable(lua_state);
				lua_pushstring(lua_state, "area");
				lua_pushstring(lua_state, "air");
				lua_rawset(lua_state, -3);
				lua_setglobal(lua_state, "args");
				lua_pcall(lua_state, 0, LUA_MULTRET, 0);
			}
			projectileSprite = Sprite::create("pics/fog.png");
			projectileSprite->setName("proj_Air");
			projectileSprite->setTag(3);
			if (_player->isFlippedX()) //player is facing left
				projectileSprite->setPosition(playerPos + Vec2(-30, 0));
			else
				projectileSprite->setPosition(playerPos + Vec2(30, 0));

			projectileBody = PhysicsBody::createBox(projectileSprite->getContentSize(), PhysicsMaterial(0, 1, 0));
			if (_player->isFlippedX()) //player is facing left
				projectileBody->setVelocity(Vec2(-visibleSize.width / 12, 0));
			else
				projectileBody->setVelocity(Vec2(visibleSize.width / 12, 0));
			this->removeChildByTag(3);

		}

		projectileSprite->setPhysicsBody(projectileBody);
		projectileBody->setCategoryBitmask(0x02); //Note: will not interact with other physics bodies directly because a different number is used here
		projectileBody->setCollisionBitmask(0x02);
		projectileBody->setContactTestBitmask(0xFFFFFFFF);
		this->addChild(projectileSprite, 0);

		_playerState->currentHeldSubstance = CPlayerState::EMPTY;
		_substanceLabel->setString("EMPTY");
		_substanceLabel->setColor(ccc3(255, 255, 255));
	}

	//determines the jump physics
	if (_playerState->currentPlayerMode == CPlayerState::JUMPING)
	{
		PhysicsBody* physicsBody = _player->getPhysicsBody();
		physicsBody->setVelocity(Vec2(0, visibleSize.height / 5));
		_playerState->currentPlayerMode = CPlayerState::FALLING;
	}
	else if (_playerState->currentPlayerMode == CPlayerState::FALLING)
	{
		PhysicsBody* physicsBody = _player->getPhysicsBody();
		Vec2 vel = physicsBody->getVelocity();
		//if (vel.y >= -(visibleSize.height * 0.0005) && vel.y <= (visibleSize.height * 0.0005)){ //checks if the player has stopped falling
		if (vel.y ==  0.0){
			_playerState->currentPlayerMode = CPlayerState::STANDING;
			_playerState->bHasDoubleJump = true;
		}
	}


	lua_close(lua_state);
}


/*
Name: onContactBegin
Params: A referenced PhysicsContact representing the contact information of two objects
*/
bool LevelDemo::onContactBegin(PhysicsContact& contact){
	
	auto nodeA = contact.getShapeA()->getBody()->getNode();
	auto nodeB = contact.getShapeB()->getBody()->getNode();
	if (nodeA != NULL && nodeB != NULL){

		lua_State *lua_state;
		lua_state = luaL_newstate();
		luaL_openlibs(lua_state);

		lua_pushcfunction(lua_state, playSound);
		lua_setglobal(lua_state, "playSound");

		if ("Player" == nodeA->getName() || "Player" == nodeB->getName()){
			if ("sub_Water" == nodeA->getName() || "sub_Water" == nodeB->getName()){
				_playerState->currentHeldSubstance = CPlayerState::WATER; //player now holds water
				_substanceLabel->setString("WATER");
				_substanceLabel->setColor(ccc3(0, 0, 255));
			}
		}

		if ("Player" == nodeA->getName() || "Player" == nodeB->getName()){
			if ("haz_Fire" == nodeA->getName() || "haz_Fire" == nodeB->getName()){
				if (luaL_loadfile(lua_state, "fmod.lua") != LUA_ERRFILE){
					lua_newtable(lua_state);
					lua_pushstring(lua_state, "area");
					lua_pushstring(lua_state, "player");
					lua_rawset(lua_state, -3);
					lua_setglobal(lua_state, "args");
					lua_pcall(lua_state, 0, LUA_MULTRET, 0);
				}
			}
		}

		if ("Player" == nodeA->getName() || "Player" == nodeB->getName()){
			if ("Exit" == nodeA->getName() || "Exit" == nodeB->getName()){
				this->_eventDispatcher->removeAllEventListeners();

				_duration = (std::clock() - _start) / (double)CLOCKS_PER_SEC;
				CCLOG("duration %f", _duration);

				//Add the high score to database
				DBManager::getInstance()->LoadDatabase("duckloonDB.db");
				DBManager::getInstance()->SetHighScore(Constant::userID, _duration);
				DBManager::getInstance()->GetHighScores();
				DBManager::getInstance()->CloseDatabase();

				Constant::level = Constant::level++;
				Director::getInstance()->popScene();
				//auto scene = LevelDemo::createScene();
				//Director::getInstance()->replaceScene(TransitionFade::create(2, scene));

				if (luaL_loadfile(lua_state, "fmod.lua") != LUA_ERRFILE){
					lua_newtable(lua_state);
					lua_pushstring(lua_state, "area");
					lua_pushstring(lua_state, "level_done");
					lua_rawset(lua_state, -3);
					lua_setglobal(lua_state, "args");
					lua_pcall(lua_state, 0, LUA_MULTRET, 0);
				}
			}
		}

		if ("haz_Fire" == nodeA->getName() || "haz_Fire" == nodeB->getName()){
			if ("proj_Water" == nodeA->getName() || "proj_Water" == nodeB->getName()){
				this->removeChildByTag(nodeA->getTag()); //destroys the fire
				this->removeChildByTag(nodeB->getTag()); //destroys the water


				if (luaL_loadfile(lua_state, "fmod.lua") != LUA_ERRFILE){
					lua_newtable(lua_state);
					lua_pushstring(lua_state, "area");
					lua_pushstring(lua_state, "fire");
					lua_rawset(lua_state, -3);
					lua_setglobal(lua_state, "args");
					lua_pcall(lua_state, 0, LUA_MULTRET, 0);
				}
			}
		}

		lua_close(lua_state);
	}
	//CCLOG("tag %i colliding with %i", nodeA->getTag(), nodeB->getTag());
	return true;
}

void LevelDemo::setViewPointCenter(CCPoint position) {

	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	int x = MAX(position.x, winSize.width / 2);
	int y = MAX(position.y, winSize.height / 2);
	x = MIN(x, (_tileMap->getMapSize().width * this->_tileMap->getTileSize().width) - winSize.width / 2);
	y = MIN(y, (_tileMap->getMapSize().height * _tileMap->getTileSize().height) - winSize.height / 2);
	CCPoint actualPosition = ccp(x, y);

	CCPoint centerOfView = ccp(winSize.width / 2, winSize.height / 2);
	CCPoint viewPoint = ccpSub(centerOfView, actualPosition);
	this->setPosition(viewPoint);
	_backgroundSprite->setPosition(actualPosition);
	_substanceLabel->setPosition(Vec2(actualPosition.x - winSize.width / 2 + _substanceLabel->getContentSize().width,
		actualPosition.y + winSize.height / 2 - _substanceLabel->getContentSize().height));
	for (int i = 0; i < _scrollNames.size(); i++)
	{
		std::string tempStr = _scrollNames[i];
		auto tempNode = this->getChildByName(tempStr);
		tempNode->setPosition(Vec2(actualPosition.x,
			actualPosition.y + winSize.height / 2 - tempNode->getContentSize().height));
	}
}

//handles closing the program
void LevelDemo::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
	return;
#endif

	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}
