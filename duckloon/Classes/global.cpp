#include "global.h"

CGameControllerManager*		g_p_GameControllerManager = 0;
IGameController*			g_p_GameController_0 = 0;

XMLLoader xmlLoader = XMLLoader::getInstance();

std::vector<DBManager::HighScore> highScores(0);

int Constant::level = 0;
std::string Constant::userID = "";

//FMODInitParams g_FMODInitparams;
//std::vector<FMOD::Sound*> g_bgMusic;
//std::vector<FMOD::Sound*> g_gameEffect;
//FMOD::Channel* g_channels[2] = { 0 };

FMODInitParams g_FMODInitparams;
std::map<std::string, FMOD::Sound*> g_Sounds;
std::map<std::string, FMOD::Channel*> g_channels;
FMOD_RESULT g_FMODResult;