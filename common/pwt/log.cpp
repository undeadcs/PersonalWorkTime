
#include "common.h"
#include "log.h"

namespace pwt {

	CLog::CLog( ) : m_sOut( "pwt.log", std::ios_base::out | std::ios_base::binary ) {
		if ( !m_sOut.is_open( ) ) {
			throw CExceptionLogOpen( );
		}
	}

	CLog::CLog( const std::string& filename ) : m_sOut( filename.c_str( ), std::ios_base::out | std::ios_base::binary ) {
		if ( !m_sOut.is_open( ) ) {
			throw CExceptionLogOpen( );
		}
	}

	CLog::~CLog( ) {
		if ( m_sOut.is_open( ) ) {
			m_sOut.close( );
		}
	}

	void CLog::write( const std::string& msg ) {
		m_sOut << msg << std::endl;
	}

}
