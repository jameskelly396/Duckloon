#include "DBManager.h"

#include <fstream>

#include "global.h"

DBManager* DBManager::m_instance = 0;

//Callback for getting the High Schores from SQLite Database
static int getHSCallback(void *NotUsed, int argc, char **argv, char **azColName)
{
	DBManager::HighScore hs;

	//Iterate through the rows in the current column
	for (unsigned int i = 0; i < argc; i++)
	{
		std::string colName = azColName[i];
		std::string value = argv[i];

		if (colName == "ID")
			hs.id = atoi(value.c_str());
		else if (colName == "USERNAME")
			hs.username = value;
		else if (colName == "SCORE")
			hs.highScore = atoi(value.c_str());
	}

	//Add High Score to global vector
	::highScores.push_back(hs);

	return 0;
}

//Loads the database into the DOA
bool DBManager::LoadDatabase(std::string dbFile)
{
	this->rc = sqlite3_open(dbFile.c_str(), &this->db);

	if (this->rc) return false;
	else return true;
}

//Retreives High Scores
void DBManager::GetHighScores()
{
	std::vector<DBManager::HighScore> hs;

	this->sql = "Select * From LEADERBOARD Order By SCORE Desc Limit 20;";
	this->rc = sqlite3_exec(db, sql, getHSCallback, this->data, &this->zErrMsg);
}

//Adds High Score to the Database
void DBManager::SetHighScore(std::string username, unsigned int highScore)
{
	//This is why I drink....coffee
	std::string stringSQL = this->sql;
	stringSQL = "Insert Into LEADERBOARD (USERNAME, SCORE) Values ('" + username + "', " + std::to_string(highScore) + ");";

	sqlite3_prepare_v2(db, stringSQL.c_str(), -1, &this->stmt, NULL);
	sqlite3_step(this->stmt);
	sqlite3_finalize(this->stmt);
}

//Closes the database file (duh)
bool DBManager::CloseDatabase()
{
	sqlite3_close(this->db);

	return true;
}