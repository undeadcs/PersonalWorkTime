
#include "common.h"
#include "task.h"

namespace pwt {

	CTask::CTask( ) {
	}

	CTask::CTask( const CTask& objCopy ) {
		m_szLabel	= objCopy.m_szLabel;
		m_szTitle	= objCopy.m_szTitle;
	}

	CTask::CTask( const Glib::ustring& szLabel, const Glib::ustring& szTitle ) {
		m_szLabel	= szLabel;
		m_szTitle	= szTitle;
	}

	CTask::~CTask( ) {
	}
}
