//
//  MockEntityModel.h
//  MyGame
//
//  Created by Ariant Uka on 2015-03-03.
//
//

#ifndef __MyGame__MockEntityModel__
#define __MyGame__MockEntityModel__

#include "IEntityModel.h"

class MockEntityModel: public IEntityModel {
public:

    MockEntityModel();
    
    std::shared_ptr<Entity> findEntityById(int entityId);
    
};

#endif /* defined(__MyGame__MockEntityModel__) */
