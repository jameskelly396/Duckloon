#include "Keyboard.h"

Keyboard* Keyboard::pSingleton = 0;

//Keyboard event handling based on http://www.gamefromscratch.com/post/2014/10/07/Cocos2d-x-Tutorial-Series-Handling-the-Keyboard.aspx 

 Keyboard::Keyboard()
{
	 this->keyboardListener = EventListenerKeyboard::create();
	 Director::getInstance()->getOpenGLView()->setIMEKeyboardState(true);
	 this->keyboardListener->onKeyPressed = [](EventKeyboard::KeyCode keyCode, Event* event){
		// // If a key already exists, do nothing as it will already have a time stamp
		// // Otherwise, set's the timestamp to now
		 keys.end();
		 if (keys.find(keyCode) == keys.end()){
			 keys[keyCode] = std::chrono::high_resolution_clock::now();
		 }
	 };
	 this->keyboardListener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event){
		 //remove the key
		 keys.erase(keyCode);
	 };
}

 Keyboard* Keyboard::getInstance(){
	 if (Keyboard::pSingleton == 0)
	 {
		 Keyboard::pSingleton = new Keyboard();
	 }


	 return Keyboard::pSingleton;
 }

Keyboard::~Keyboard()
{
	
}


/*
Name: isKeyPressed
Params: An EventKeyboard::KeyCode object representing the keycode being checked
Returns: A bool representing whether or not the key has been pressed
*/
bool Keyboard::isKeyPressed(cocos2d::EventKeyboard::KeyCode code)
{
	// Check if the key is currently pressed by seeing it it's in the std::map keys
	if (keys.find(code) != keys.end())
		return true;

	return false;
}

// Because cocos2d-x requres createScene to be static, we need to make other non-pointer members static
std::map<cocos2d::EventKeyboard::KeyCode, std::chrono::high_resolution_clock::time_point> Keyboard::keys;