#include "XMLLoader.h"

#include "CGameController/CGameControllerManager.h"

#include "FMOD_Stuff.h"

#include "DBManager.h"

extern CGameControllerManager*	g_p_GameControllerManager;
extern IGameController*			g_p_GameController_0;

extern XMLLoader xmlLoader;

extern std::vector<DBManager::HighScore> highScores;

class Constant {
public:
	static int level;
	static std::string userID;
};

//extern FMODInitParams g_FMODInitparams;
//extern std::vector<FMOD::Sound*> g_bgMusic;
//extern std::vector<FMOD::Sound*> g_gameEffect;
//extern FMOD::Channel* g_channels[2];

extern FMODInitParams g_FMODInitparams;
extern std::map<std::string, FMOD::Sound*> g_Sounds;
extern std::map<std::string, FMOD::Channel*> g_channels;
extern FMOD_RESULT g_FMODResult;