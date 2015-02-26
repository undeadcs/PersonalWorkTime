
#include "common.h"
#include "config.h"
#include "log.h"
#include "task.h"
#include "WorkRecord.h"
#include "PersonalWorkTime.h"

namespace pwt {

	CPersonalWorkTime::CPersonalWorkTime( ) : m_objConfig( ), m_objLog( m_objConfig.GetLogPath( ) ), m_pDb( NULL ) {
		OpenDatabase( m_objConfig.GetDatabasePath( ) );
	}

	CPersonalWorkTime::CPersonalWorkTime( const std::string& szConfigFile ) :
		m_objConfig( szConfigFile ),
		m_objLog( m_objConfig.GetLogPath( ) ),
		m_pDb( NULL )
	{
		OpenDatabase( m_objConfig.GetDatabasePath( ) );
	}

	CPersonalWorkTime::~CPersonalWorkTime( ) {
		if ( m_pDb ) {
			sqlite3_close( m_pDb );
		}
	}

	void CPersonalWorkTime::OpenDatabase( const Glib::ustring& filename ) {
		if ( sqlite3_open_v2( filename.c_str( ), &m_pDb, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, NULL ) != SQLITE_OK ) {
			throw CExceptionDatabaseOpen( );
		}

		sqlite3_enable_load_extension( m_pDb, 1 );

		Glib::ustring query;
		sqlite3_stmt* pStatement;

		// задачи
		query = Glib::ustring( "CREATE TABLE IF NOT EXISTS pwt_tasks(" ) +
					"task_id INTEGER PRIMARY KEY," +
					"task_label VARCHAR(255)," +
					"task_title TEXT" +
				");";
		if ( sqlite3_prepare_v2( m_pDb, query.c_str( ), query.bytes( ), &pStatement, NULL ) == SQLITE_OK ) {
			if ( sqlite3_step( pStatement ) == SQLITE_ERROR ) {
				sqlite3_finalize( pStatement );

				throw CExceptionDatabaseError( sqlite3_errmsg( m_pDb ) );
			}

			sqlite3_finalize( pStatement );
		}

		// записи о работе
		query = Glib::ustring( "CREATE TABLE IF NOT EXISTS pwt_work_records(" ) +
					"record_id INTEGER PRIMARY KEY," +
					"record_task_id INTEGER," +
					"record_begin DATETIME," +
					"record_end DATETIME" +
				");";
		if ( sqlite3_prepare_v2( m_pDb, query.c_str( ), query.bytes( ), &pStatement, NULL ) == SQLITE_OK ) {
			if ( sqlite3_step( pStatement ) == SQLITE_ERROR ) {
				sqlite3_finalize( pStatement );

				throw CExceptionDatabaseError( sqlite3_errmsg( m_pDb ) );
			}

			sqlite3_finalize( pStatement );
		}
	}

	CPersonalWorkTime::PWTResult CPersonalWorkTime::AddTask( const Glib::ustring& label, const Glib::ustring& title ) {
		sqlite3_stmt* pStatement;
		Glib::ustring query;
		int count = 0;

		query = "SELECT COUNT(*) AS cnt FROM pwt_tasks WHERE task_label=:label";

		if ( sqlite3_prepare_v2( m_pDb, query.c_str( ), query.bytes( ), &pStatement, NULL ) != SQLITE_OK ) {
			return CPersonalWorkTime::PWTRESULT_DATABASE_ERROR;
		}

		sqlite3_bind_text( pStatement, sqlite3_bind_parameter_index( pStatement, ":label" ), label.c_str( ), label.bytes( ), NULL );

		if ( sqlite3_step( pStatement ) == SQLITE_ROW ) {
			count = sqlite3_column_int( pStatement, 0 );
		}

		sqlite3_finalize( pStatement );

		if ( count > 0 ) {
			return CPersonalWorkTime::PWTRESULT_TASK_ALREADY_EXISTS;
		}

		query = "INSERT INTO pwt_tasks(task_label, task_title) VALUES (:label, :title)";

		if ( sqlite3_prepare_v2( m_pDb, query.c_str( ), query.bytes( ), &pStatement, NULL ) != SQLITE_OK ) {
			return CPersonalWorkTime::PWTRESULT_DATABASE_ERROR;
		}

		sqlite3_bind_text( pStatement, sqlite3_bind_parameter_index( pStatement, ":label" ), label.c_str( ), label.bytes( ), NULL );
		sqlite3_bind_text( pStatement, sqlite3_bind_parameter_index( pStatement, ":title" ), title.c_str( ), title.bytes( ), NULL );

		int res = sqlite3_step( pStatement );
		CPersonalWorkTime::PWTResult result = ( res == SQLITE_ERROR ) ? CPersonalWorkTime::PWTRESULT_DATABASE_ERROR : CPersonalWorkTime::PWTRESULT_OK;

		sqlite3_finalize( pStatement );

		return result;
	}

	static Glib::ustring SqliteStrToUstring( const unsigned char* str, int size ) {
		char *tmp = new char[ size + 1 ];

		memset( tmp, 0, size + 1 );
		memcpy( tmp, str, size );

		Glib::ustring ret( tmp );

		delete[ ] tmp;

		return ret;
	}

	std::list< CTask > CPersonalWorkTime::ListTask( const Glib::ustring& pattern ) {
		std::list< CTask > ret;

		Glib::ustring query = "SELECT * FROM pwt_tasks";
		sqlite3_stmt* pStatement;

		if ( pattern != "*" ) {
			query += " WHERE task_title LIKE :pattern";
		}

		if ( sqlite3_prepare( m_pDb, query.c_str( ), query.bytes( ), &pStatement, NULL ) != SQLITE_OK ) {
			return ret;
		}

		if ( pattern != "*" ) {
			Glib::ustring tmpPattern( "%" + pattern + "%" );
			sqlite3_bind_text( pStatement, sqlite3_bind_parameter_index( pStatement, ":pattern" ), tmpPattern.c_str( ), tmpPattern.bytes( ), NULL );
		}

		std::string columnName = "";
		int i = 0, columnCount = 0;
		CTask task;

		while( sqlite3_step( pStatement ) == SQLITE_ROW ) {
			columnCount = sqlite3_column_count( pStatement );

			for( i = 0; i < columnCount; ++i ) {
				if ( sqlite3_column_type( pStatement, i ) == SQLITE_TEXT ) {
					columnName = sqlite3_column_name( pStatement, i );
					if ( columnName == "task_label" ) {
						task.SetLabel( SqliteStrToUstring( sqlite3_column_text( pStatement, i ), sqlite3_column_bytes( pStatement, i ) ) );
					} else if ( columnName == "task_title" ) {
						task.SetTitle( SqliteStrToUstring( sqlite3_column_text( pStatement, i ), sqlite3_column_bytes( pStatement, i ) ) );
					}
				}
			}

			ret.insert( ret.end( ), task );
		}

		return ret;
	}

	CPersonalWorkTime::PWTResult CPersonalWorkTime::UpdTask( const Glib::ustring& label, const Glib::ustring& title ) {
		sqlite3_stmt* pStatement;
		Glib::ustring query;
		int count = 0;

		query = "SELECT COUNT(*) AS cnt FROM pwt_tasks WHERE task_label=:label";

		if ( sqlite3_prepare_v2( m_pDb, query.c_str( ), query.bytes( ), &pStatement, NULL ) != SQLITE_OK ) {
			return CPersonalWorkTime::PWTRESULT_DATABASE_ERROR;
		}

		sqlite3_bind_text( pStatement, sqlite3_bind_parameter_index( pStatement, ":label" ), label.c_str( ), label.bytes( ), NULL );

		if ( sqlite3_step( pStatement ) == SQLITE_ROW ) {
			count = sqlite3_column_int( pStatement, 0 );
		}

		sqlite3_finalize( pStatement );

		if ( count != 1 ) {
			return CPersonalWorkTime::PWTRESULT_TASK_NOT_FOUND;
		}

		query = "UPDATE pwt_tasks SET task_title=:title WHERE task_label=:label";

		if ( sqlite3_prepare_v2( m_pDb, query.c_str( ), query.bytes( ), &pStatement, NULL ) != SQLITE_OK ) {
			return CPersonalWorkTime::PWTRESULT_DATABASE_ERROR;
		}

		sqlite3_bind_text( pStatement, sqlite3_bind_parameter_index( pStatement, ":label" ), label.c_str( ), label.bytes( ), NULL );
		sqlite3_bind_text( pStatement, sqlite3_bind_parameter_index( pStatement, ":title" ), title.c_str( ), title.bytes( ), NULL );

		int res = sqlite3_step( pStatement );
		CPersonalWorkTime::PWTResult result = ( res == SQLITE_ERROR ) ? CPersonalWorkTime::PWTRESULT_DATABASE_ERROR : CPersonalWorkTime::PWTRESULT_OK;

		sqlite3_finalize( pStatement );

		return result;
	}

	CPersonalWorkTime::PWTResult CPersonalWorkTime::DelTask( const Glib::ustring& label ) {
		sqlite3_stmt* pStatement;
		Glib::ustring query;
		int count = 0;

		query = "SELECT COUNT(*) AS cnt FROM pwt_tasks WHERE task_label=:label";

		if ( sqlite3_prepare_v2( m_pDb, query.c_str( ), query.bytes( ), &pStatement, NULL ) != SQLITE_OK ) {
			return CPersonalWorkTime::PWTRESULT_DATABASE_ERROR;
		}

		sqlite3_bind_text( pStatement, sqlite3_bind_parameter_index( pStatement, ":label" ), label.c_str( ), label.bytes( ), NULL );

		if ( sqlite3_step( pStatement ) == SQLITE_ROW ) {
			count = sqlite3_column_int( pStatement, 0 );
		}

		sqlite3_finalize( pStatement );

		if ( count != 1 ) {
			return CPersonalWorkTime::PWTRESULT_TASK_NOT_FOUND;
		}

		query = "DELETE FROM pwt_tasks WHERE task_label=:label";

		if ( sqlite3_prepare_v2( m_pDb, query.c_str( ), query.bytes( ), &pStatement, NULL ) != SQLITE_OK ) {
			return CPersonalWorkTime::PWTRESULT_DATABASE_ERROR;
		}

		sqlite3_bind_text( pStatement, sqlite3_bind_parameter_index( pStatement, ":label" ), label.c_str( ), label.bytes( ), NULL );

		int res = sqlite3_step( pStatement );
		CPersonalWorkTime::PWTResult result = ( res == SQLITE_ERROR ) ? CPersonalWorkTime::PWTRESULT_DATABASE_ERROR : CPersonalWorkTime::PWTRESULT_OK;

		sqlite3_finalize( pStatement );

		return result;
	}

	static Glib::ustring GetCurrentDatetime( ) {
		std::time_t tTime = 0;
		const std::tm* pTm = NULL;
		char buf[ 20 ];

		memset( buf, 0, 20 );
		time( &tTime );
		pTm = localtime( &tTime );
		std::strftime( buf, 20, "%Y-%m-%d %H:%M:%S", pTm );

		return Glib::ustring( buf );
	}

	CPersonalWorkTime::PWTResult CPersonalWorkTime::StartTask( const Glib::ustring& label ) {
		sqlite3_stmt* pStatement;
		Glib::ustring query;
		int taskId = 0;

		query = "SELECT task_id AS cnt FROM pwt_tasks WHERE task_label=:label";

		if ( sqlite3_prepare_v2( m_pDb, query.c_str( ), query.bytes( ), &pStatement, NULL ) != SQLITE_OK ) {
			return CPersonalWorkTime::PWTRESULT_DATABASE_ERROR;
		}

		sqlite3_bind_text( pStatement, sqlite3_bind_parameter_index( pStatement, ":label" ), label.c_str( ), label.bytes( ), NULL );

		if ( sqlite3_step( pStatement ) == SQLITE_ROW ) {
			taskId = sqlite3_column_int( pStatement, 0 );
		}

		sqlite3_finalize( pStatement );

		if ( !taskId ) {
			return CPersonalWorkTime::PWTRESULT_TASK_NOT_FOUND;
		}

		// предварительно необходимо завершить текущую задачу
		query = "SELECT record_id, record_task_id FROM pwt_work_records WHERE record_end IS NULL";
		if ( sqlite3_prepare_v2( m_pDb, query.c_str( ), query.bytes( ), &pStatement, NULL ) != SQLITE_OK ) {
			return CPersonalWorkTime::PWTRESULT_DATABASE_ERROR;
		}

		if ( sqlite3_step( pStatement ) == SQLITE_ROW ) { // есть текущая задача
			sqlite3_stmt* stmtUpdate = NULL;
			int recordId = sqlite3_column_int( pStatement, 0 );
			int recordTaskId = sqlite3_column_int( pStatement, 1 );

			if ( taskId == recordTaskId ) {
				sqlite3_finalize( pStatement );

				return CPersonalWorkTime::PWTRESULT_ALREADY_RUNNING;
			}

			query = "UPDATE pwt_work_records SET record_end=:record_end WHERE record_id=:record_id";
			if ( sqlite3_prepare_v2( m_pDb, query.c_str( ), query.bytes( ), &stmtUpdate, NULL ) != SQLITE_OK ) {
				sqlite3_finalize( pStatement );

				return CPersonalWorkTime::PWTRESULT_DATABASE_ERROR;
			}

			Glib::ustring now = GetCurrentDatetime( );
			sqlite3_bind_text( stmtUpdate, sqlite3_bind_parameter_index( stmtUpdate, ":record_end" ), now.c_str( ), now.bytes( ), NULL );
			sqlite3_bind_int( stmtUpdate, sqlite3_bind_parameter_index( stmtUpdate, ":record_id" ), recordId );

			if ( sqlite3_step( stmtUpdate ) == SQLITE_ERROR ) {
				sqlite3_finalize( pStatement );
				sqlite3_finalize( stmtUpdate );

				return CPersonalWorkTime::PWTRESULT_DATABASE_ERROR;
			}

			sqlite3_finalize( stmtUpdate );
		}

		sqlite3_finalize( pStatement );

		query = "INSERT INTO pwt_work_records(record_task_id, record_begin, record_end) VALUES(:record_task_id, :record_begin, :record_end)";
		if ( sqlite3_prepare_v2( m_pDb, query.c_str( ), query.bytes( ), &pStatement, NULL ) != SQLITE_OK ) {
			return CPersonalWorkTime::PWTRESULT_DATABASE_ERROR;
		}

		Glib::ustring now = GetCurrentDatetime( );
		sqlite3_bind_text( pStatement, sqlite3_bind_parameter_index( pStatement, ":record_begin" ), now.c_str( ), now.bytes( ), NULL );
		sqlite3_bind_int( pStatement, sqlite3_bind_parameter_index( pStatement, ":record_task_id" ), taskId );
		sqlite3_bind_null( pStatement, sqlite3_bind_parameter_index( pStatement, ":record_end" ) );

		CPersonalWorkTime::PWTResult result = CPersonalWorkTime::PWTRESULT_OK;

		if ( sqlite3_step( pStatement ) == SQLITE_ERROR ) {
			result = CPersonalWorkTime::PWTRESULT_DATABASE_ERROR;
		}

		sqlite3_finalize( pStatement );

		return result;
	}

	CPersonalWorkTime::PWTResult CPersonalWorkTime::StopTask( ) {
		sqlite3_stmt* pStatement;
		Glib::ustring query;
		CPersonalWorkTime::PWTResult result = CPersonalWorkTime::PWTRESULT_NOTHING_RUNNING;

		query = "SELECT record_id FROM pwt_work_records WHERE record_end IS NULL";
		if ( sqlite3_prepare_v2( m_pDb, query.c_str( ), query.bytes( ), &pStatement, NULL ) != SQLITE_OK ) {
			return CPersonalWorkTime::PWTRESULT_DATABASE_ERROR;
		}

		if ( sqlite3_step( pStatement ) == SQLITE_ROW ) { // есть текущая задача
			sqlite3_stmt* stmtUpdate = NULL;
			int recordId = sqlite3_column_int( pStatement, 0 );

			query = "UPDATE pwt_work_records SET record_end=:record_end WHERE record_id=:record_id";
			if ( sqlite3_prepare_v2( m_pDb, query.c_str( ), query.bytes( ), &stmtUpdate, NULL ) != SQLITE_OK ) {
				sqlite3_finalize( pStatement );

				return CPersonalWorkTime::PWTRESULT_DATABASE_ERROR;
			}

			Glib::ustring now = GetCurrentDatetime( );
			sqlite3_bind_text( stmtUpdate, sqlite3_bind_parameter_index( stmtUpdate, ":record_end" ), now.c_str( ), now.bytes( ), NULL );
			sqlite3_bind_int( stmtUpdate, sqlite3_bind_parameter_index( stmtUpdate, ":record_id" ), recordId );

			int res = sqlite3_step( stmtUpdate );
			if ( res == SQLITE_ERROR ) {
				result = CPersonalWorkTime::PWTRESULT_DATABASE_ERROR;
			} else {
				result = CPersonalWorkTime::PWTRESULT_OK;
			}

			sqlite3_finalize( stmtUpdate );
		}

		sqlite3_finalize( pStatement );

		return result;
	}

	CPersonalWorkTime::PWTResult CPersonalWorkTime::Status( CWorkRecord& record ) {
		sqlite3_stmt* pStatement;
		Glib::ustring query;
		CPersonalWorkTime::PWTResult result = CPersonalWorkTime::PWTRESULT_NOTHING_RUNNING;

		query = "SELECT record_task_id, record_begin, record_end FROM pwt_work_records WHERE record_end IS NULL LIMIT 1";
		if ( sqlite3_prepare_v2( m_pDb, query.c_str( ), query.bytes( ), &pStatement, NULL ) != SQLITE_OK ) {
			return CPersonalWorkTime::PWTRESULT_DATABASE_ERROR;
		}

		if ( sqlite3_step( pStatement ) == SQLITE_ROW ) { // есть текущая задача
			int recordTaskId = sqlite3_column_int( pStatement, 0 );

			sqlite3_stmt* stmtSelect = NULL;

			query = "SELECT task_label, task_title FROM pwt_tasks WHERE task_id=:task_id";
			if ( sqlite3_prepare_v2( m_pDb, query.c_str( ), query.bytes( ), &stmtSelect, NULL ) != SQLITE_OK ) {
				sqlite3_finalize( pStatement );

				return CPersonalWorkTime::PWTRESULT_DATABASE_ERROR;
			}

			sqlite3_bind_int( stmtSelect, sqlite3_bind_parameter_index( stmtSelect, ":task_id" ), recordTaskId );

			int res = sqlite3_step( stmtSelect );
			if ( res == SQLITE_ROW ) {
				Glib::ustring dtBegin = "", dtEnd = "";

				if ( sqlite3_column_type( pStatement, 1 ) == SQLITE_TEXT ) {
					dtBegin = SqliteStrToUstring( sqlite3_column_text( pStatement, 1 ), sqlite3_column_bytes( pStatement, 1 ) );
				}
				if ( sqlite3_column_type( pStatement, 2 ) == SQLITE_TEXT ) {
					dtEnd = SqliteStrToUstring( sqlite3_column_text( pStatement, 2 ), sqlite3_column_bytes( pStatement, 2 ) );
				}

				CTask task;

				if ( sqlite3_column_type( stmtSelect, 0 ) == SQLITE_TEXT ) {
					task.SetLabel( SqliteStrToUstring( sqlite3_column_text( stmtSelect, 0 ), sqlite3_column_bytes( stmtSelect, 0 ) ) );
				}
				if ( sqlite3_column_type( stmtSelect, 1 ) == SQLITE_TEXT ) {
					task.SetTitle( SqliteStrToUstring( sqlite3_column_text( stmtSelect, 1 ), sqlite3_column_bytes( stmtSelect, 1 ) ) );
				}

				record = CWorkRecord( dtBegin, dtEnd, task );

				result = CPersonalWorkTime::PWTRESULT_ALREADY_RUNNING;
			} else if ( res == SQLITE_ERROR ) {
				result = CPersonalWorkTime::PWTRESULT_DATABASE_ERROR;
			}

			sqlite3_finalize( stmtSelect );
		}

		sqlite3_finalize( pStatement );

		return result;
	}
}
