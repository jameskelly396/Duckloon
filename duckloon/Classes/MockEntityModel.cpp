//
//  MockEntityModel.cpp
//  MyGame
//
//  Created by Ariant Uka on 2015-03-03.
//
//

#include "MockEntityModel.h"


#ifndef __MyGame__EntityModel__
#define __MyGame__EntityModel__

MockEntityModel::MockEntityModel() {
    
}

std::shared_ptr<Entity> MockEntityModel::findEntityById(int id) {
    return std::make_shared<Entity>(1, "my fake entity");
    
}

#endif /* defined(__MyGame__EntityModel__) */
