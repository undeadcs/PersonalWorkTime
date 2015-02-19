
#ifndef _PWT_LOG_H
#define _PWT_LOG_H

namespace pwt {

	/**
	 * Лог
	 */
	class CLog {
		std::ofstream m_sOut;

	public:
		CLog( );
		CLog( const std::string& filename );

		virtual ~CLog( );

		void write( const std::string& msg );
	};

	class CExceptionLogOpen : public std::exception {
	public:
		virtual const char* what( ) const throw( ) {
			return "failed to open log";
		}
	};

}

#endif
