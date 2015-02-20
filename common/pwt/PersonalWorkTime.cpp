
#include "common.h"
#include "config.h"
#include "log.h"
#include "task.h"
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

		std::cout << "count=" << count << std::endl;

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

		std::cout << "res=" << res << std::endl;

		sqlite3_finalize( pStatement );

		return result;
	}
}
