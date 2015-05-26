//
//  EntityModel.h
//  MyGame
//
//  Created by Ariant Uka on 2015-03-03.
//
//

#ifndef __MyGame__EntityModel__
#define __MyGame__EntityModel__

#include <stdio.h>
#include <memory.h>
#include <memory>
#include "sqlite3.h" 
#include "Entity.h"
#include "IEntityModel.h"
#include "cocos2d.h"

class EntityModel: public IEntityModel {
public:
    
    /**
     * Create the model, requires a sqlite3 connection.
     */
    EntityModel(std::shared_ptr<sqlite3> db);
    
    /**
     * Find an entity by id.
     */
    std::shared_ptr<Entity> findEntityById(int entityId);
    
private:
    std::shared_ptr<sqlite3> _db;
    
};

#endif /* defined(__MyGame__EntityModel__) */
