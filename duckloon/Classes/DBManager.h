#ifndef __DBManager__
#define __DBManager__

#include "sqlite3.h"
#include <string>

class DBManager
{
public:

	struct HighScore
	{
		unsigned int id;
		std::string username;
		unsigned int highScore;
	};

	static int sqlCallback(void *NotUsed, int argc, char **argv, char **azColName)
	{
		int i;
		for (i = 0; i<argc; i++)
		{
			printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		}
		printf("\n");
		return 0;
	}

	static DBManager* getInstance()
	{
		if (!m_instance)
			m_instance = new DBManager;

		return m_instance;
	}

	bool LoadDatabase(std::string dbFile);
	bool CloseDatabase();

	void DBManager::GetHighScores();
	void SetHighScore(std::string username, unsigned int highScore);

private:
	DBManager() {};

	static DBManager* m_instance;

	sqlite3 *db;
	sqlite3_stmt * stmt;
	char *zErrMsg = 0;
	int rc;
	char *sql;
	char* data;
};

#endif