//
//  IEntityModel.h
//  MyGame
//
//  Created by Ariant Uka on 2015-03-03.
//
//

#ifndef __MyGame__IEntityModel__
#define __MyGame__IEntityModel__

#include <stdio.h>
#include "Entity.h"
#include <memory.h>
#include <memory>

class IEntityModel {
public:
    
    virtual ~IEntityModel() {}
    virtual std::shared_ptr<Entity> findEntityById(int entityId) = 0;
    
};

#endif /* defined(__MyGame__IEntityModel__) */
