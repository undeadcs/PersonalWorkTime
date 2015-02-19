
#include "common.h"
#include "log.h"
#include "config.h"
#include "PersonalWorkTime.h"

namespace pwt {

	CPersonalWorkTime::CPersonalWorkTime( ) : m_objConfig( ), m_objLog( m_objConfig.GetLogPath( ) ) {
	}

	CPersonalWorkTime::CPersonalWorkTime( const std::string& szConfigFile ) : m_objConfig( szConfigFile ), m_objLog( m_objConfig.GetLogPath( ) ) {
	}

	CPersonalWorkTime::~CPersonalWorkTime( ) {
	}

}
