
#ifndef _PWT_CONFIG_H
#define _PWT_CONFIG_H

namespace pwt {

	class CConfig {
		Glib::ustring	m_szLog,		// путь до файла лога
						m_szDatabase;	// путь до файла базы данных

	public:
		CConfig( );
		CConfig( const std::string& filename );

		virtual ~CConfig( );

		Glib::ustring	GetLogPath( )		const { return m_szLog;			}
		Glib::ustring	GetDatabasePath( )	const { return m_szDatabase;	}

	private:
		void	LoadConfig( const std::string& filename );
	};

	class CExceptionConfigOpen : public std::exception {
	public:
		virtual const char* what( ) const throw( ) {
			return "failed to open config";
		}
	};

	class CExceptionConfigInvalid : public std::exception {
	public:
		virtual const char* what( ) const throw( ) {
			return "invalid config format";
		}
	};
}

#endif
