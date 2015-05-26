//
//  Entity.cpp
//  MyGame
//
//  Created by Ariant Uka on 2015-03-03.
//
//

#include "Entity.h"

Entity::Entity(int entityId, std::string name) :_id(entityId), _name(name) {
    
}

std::string Entity::getName() {
    return _name;
}