//
//  EntityModel.cpp
//  MyGame
//
//  Created by Ariant Uka on 2015-03-03.
//
//

#include "EntityModel.h"

USING_NS_CC;

EntityModel::EntityModel(std::shared_ptr<sqlite3> db) : _db(db) {
}

std::shared_ptr<Entity> EntityModel::findEntityById(int id) {
    const char select_sql[] = "SELECT id, name FROM Entities WHERE id = ?";
    sqlite3_stmt *stmt = NULL;
    
    int returnValue = sqlite3_prepare_v2(this->_db.get(), select_sql, -1, &stmt, NULL);
    if(returnValue != SQLITE_OK) {
        CCLOG("Error preparing statement for EntityModel");
        return nullptr;
    }

    returnValue = sqlite3_bind_int(stmt, 1, id);
    if(returnValue != SQLITE_OK) {
        CCLOG("Error binding statement for EntityModel");
        return nullptr;
    }
    
    returnValue = sqlite3_step(stmt);
    if(returnValue != SQLITE_ROW) {
        CCLOG("Error stepping for EntityModel");
        return nullptr;
    }
    
    if (sqlite3_column_count(stmt) != 2) {
        return nullptr;
    }
    
    int retrievedId = sqlite3_column_int(stmt, 0);
    const unsigned char* name = sqlite3_column_text(stmt, 1);
    return std::make_shared<Entity>(retrievedId, std::string(reinterpret_cast<const char*>(name)));
    
}