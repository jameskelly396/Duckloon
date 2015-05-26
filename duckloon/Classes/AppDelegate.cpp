#include "AppDelegate.h"
#include "OpeningMenuScene.h"
#include "LevelDemoScene.h"
#include "TransitionScene.h"
#include "global.h"
#include "EntityModel.h"
#include <fstream>

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

bool CreateAllTheSounds(std::wstring &error);

bool AppDelegate::applicationDidFinishLaunching() {
	LoadXML(xmlLoader);

    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLViewImpl::create("My Game");
        director->setOpenGLView(glview);
    }

	sqlite3* rawConnec = nullptr;

	std::string path = /*FileUtils::getInstance()->getWritablePath() +*/ "C:/duckloon/duckloon/proj.win32/my.db";
	
	CCLOG("The db path %s", path.c_str());


	sqlite3_open(path.c_str(), &rawConnec);

	std::shared_ptr<sqlite3> db(rawConnec, sqlite3_close);

	std::shared_ptr<IEntityModel> entityModel = std::make_shared<EntityModel>(db);

	std::shared_ptr<Entity> foundModel = entityModel->findEntityById(1);

    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

	//FMOD setup
	std::wstring error;
	if (!InitFMOD(::g_FMODInitparams, error))
	{
		return -1;
	}
	if (!CreateAllTheSounds(error))
	{
		return -1;
	}

	FMOD::Channel *channel;
	g_FMODResult = g_FMODInitparams.system->playSound(FMOD_CHANNEL_REUSE, g_Sounds["Sound/Duckloon_Menu_MUSIC_MIX.mp3"], false, &g_channels["BGM"]);

	std::wstringstream ssErrors;
	if (!FMODERRORCHECK(g_FMODResult, error))
	{
		ssErrors << error << std::endl;
		error = ssErrors.str();
		CCLOG("Playing sound: %s\n", error);
		return false;
	}

    // create a scene. it's an autorelease object
    auto scene = OpeningMenu::createScene();
	//auto scene = LevelDemo::createScene();
	//auto scene = Transition::createScene();

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}

bool CreateAllTheSounds(std::wstring &error)
{
	FMOD_RESULT FMODResult;
	std::wstringstream ssErrors;

	std::string fileName = "Sound.txt";
	std::string soundsDir = "Sound/";

	std::ifstream myFile((soundsDir + fileName).c_str());

	//Check if file is open
	if (!myFile.is_open()) return false;

	std::string temp;
	FMOD::Sound* sound;
	bool bKeepReading = true;

	//Read in the file
	while (bKeepReading)
	{
		myFile >> temp;

		if (temp == "#SOUND") //Found a Streaming Sound
		{
			myFile >> temp;
			FMODResult = ::g_FMODInitparams.system->createStream((soundsDir + temp).c_str(), FMOD_DEFAULT, 0, &sound);
			g_Sounds[(soundsDir + temp).c_str()] = sound;
			//FMOD::Channel *channel;
			//g_channels[(soundsDir + temp).c_str()] = channel;

			if (!FMODERRORCHECK(FMODResult, error))
			{
				ssErrors << error << std::endl;
				error = ssErrors.str();
				return false;
			}
		}
		else if (temp == "#END") bKeepReading = false;
	}
	//FMODResult = ::g_FMODInitparams.system->playSound(FMOD_CHANNEL_FREE, g_bgMusic[0], false, &(g_channels[0]));
	return true;
}
