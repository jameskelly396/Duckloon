#ifndef __Keyboard_H__
#define __Keyboard_H__

#include "cocos2d.h"

USING_NS_CC;

class Keyboard
{
public:
	~Keyboard();
	static Keyboard* getInstance();

    EventListenerKeyboard* keyboardListener;

	bool isKeyPressed(cocos2d::EventKeyboard::KeyCode code);

private:
	Keyboard();
	static Keyboard* pSingleton;
	static std::map<cocos2d::EventKeyboard::KeyCode,
		std::chrono::high_resolution_clock::time_point> keys2;
	static std::map<cocos2d::EventKeyboard::KeyCode, std::chrono::high_resolution_clock::time_point> keys;
};
#endif