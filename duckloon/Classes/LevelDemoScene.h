#ifndef __LevelDemo_Scene_H__
#define __LevelDemo_Scene_H__

#include "cocos2d.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "Keyboard.h"
#include "CPlayerState.h"


#include <lua.hpp>
#include <ctime>
 
class LevelDemo : public cocos2d::Layer  /*public cocos2d::LayerColor*/
{
public:
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene* createScene();

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();
	
	// a selector callback
	void menuCloseCallback(cocos2d::Ref* pSender);

	//Keyboard callbacks
	bool isKeyPressed(cocos2d::EventKeyboard::KeyCode);

	bool onContactBegin(cocos2d::PhysicsContact& contact);

	void setViewPointCenter(CCPoint position);

	Point tileCoordForPosition(Point position);



	cocos2d::Sprite *_player;
	cocos2d::Sprite *_backgroundSprite;
	cocos2d::Label *_substanceLabel;

	CPlayerState *_playerState;
	
	// implement the "static create()" method manually
	CREATE_FUNC(LevelDemo);
	
	Keyboard* _newKeyboard;

	std::vector<std::string> _scrollNames;

	std::clock_t _start;
	double _duration;


private:
	PhysicsWorld *sceneWorld;
	void SetPhysicsWorld(PhysicsWorld *world) { sceneWorld = world; }

	static std::map<cocos2d::EventKeyboard::KeyCode,
		std::chrono::high_resolution_clock::time_point> keys;
	//cocos2d::Label * label;

	TMXTiledMap*_tileMap;
	TMXLayer* _background;
	TMXLayer* _blockage;
	TMXObjectGroup* _collision;
	TMXObjectGroup* _hazards;
	TMXObjectGroup* _substances;
	TMXObjectGroup* _spawnPoint;
public:
	virtual void update(float delta) override;
};

#endif // __HELLOWORLD_SCENE_H__
