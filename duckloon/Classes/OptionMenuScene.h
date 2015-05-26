#ifndef __OptionMenu_Scene_H__
#define __OptionMenu_Scene_H__

#include "cocos2d.h"

class OptionMenu : public cocos2d::Layer  /*public cocos2d::LayerColor*/
{
public:
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene* createScene();

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();

	// a selector callback
	void menuCloseCallback(cocos2d::Ref* pSender);

	void PopScene(cocos2d::Ref *pSender);

	//Keyboard callbacks
	bool isKeyPressed(cocos2d::EventKeyboard::KeyCode);

	cocos2d::Sprite *_player;

	void CreateMenu(int tag, int curLanguage);

	// implement the "static create()" method manually
	CREATE_FUNC(OptionMenu);

private:
	static std::map<cocos2d::EventKeyboard::KeyCode,
		std::chrono::high_resolution_clock::time_point> keys;
	//cocos2d::Label * label;
public:
	virtual void update(float delta) override;
};

#endif // __HELLOWORLD_SCENE_H__
