//
//  Entity.h
//  MyGame
//
//  Created by Ariant Uka on 2015-03-03.
//
//

#ifndef __MyGame__Entity__
#define __MyGame__Entity__
#include <stdio.h>
#include <string>

class Entity {
public:
    
    Entity(int entityId, std::string name);
    std::string getName();
    
private:
    int _id;
    std::string _name;
    
};


#endif /* defined(__MyGame__Entity__) */
