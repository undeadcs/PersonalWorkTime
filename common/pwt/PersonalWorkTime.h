
#ifndef _PERSONAL_WORK_TIME_H
#define _PERSONAL_WORK_TIME_H

namespace pwt {

	class CPersonalWorkTime {
	public:
		enum PWTResult {
			PWTRESULT_OK,
			PWTRESULT_DATABASE_ERROR,
			PWTRESULT_TASK_NOT_FOUND,
			PWTRESULT_TASK_ALREADY_EXISTS,
			PWTRESULT_INVALID_TASK_LABEL,
			PWTRESULT_INVALID_TASK_TITLE,
			PWTRESULT_INVALID_LIST_PATTERN
		};

	private:
		CConfig		m_objConfig;
		CLog		m_objLog;
		sqlite3*	m_pDb;

	public:
		CPersonalWorkTime( );
		CPersonalWorkTime( const std::string& szConfigFile );

		virtual ~CPersonalWorkTime( );

		PWTResult	AddTask( const Glib::ustring& label, const Glib::ustring& title );
		PWTResult UpdTask( const Glib::ustring& label, const Glib::ustring& title );
		PWTResult DelTask( const Glib::ustring& label );
		std::list< CTask > ListTask( const Glib::ustring& pattern );
		PWTResult StartTask( const Glib::ustring& label );
		PWTResult StopTask( const Glib::ustring& label );

	private:
		void	OpenDatabase( const Glib::ustring& filename );
	};

	class CExceptionDatabaseOpen : public std::exception {
	public:
		virtual const char* what( ) const throw( ) {
			return "failed to open database";
		}
	};

	class CExceptionDatabaseError : public std::exception {
		Glib::ustring m_szMsg;

	public:
		CExceptionDatabaseError( const char* msg ) : m_szMsg( msg ) { }

		virtual ~CExceptionDatabaseError( ) throw( ) { }

		virtual const char* what( ) const throw( ) {
			return ( Glib::ustring( "sqlite error: " ) + m_szMsg ).c_str( );
		}
	};

}

#endif
