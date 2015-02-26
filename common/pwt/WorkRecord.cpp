
#include "common.h"
#include "task.h"
#include "WorkRecord.h"

namespace pwt {

	CWorkRecord::CWorkRecord( ) {
	}

	CWorkRecord::CWorkRecord( const Glib::ustring& dtBegin, const Glib::ustring& dtEnd, const CTask& objTask ) :
		m_dtBegin( dtBegin ), m_dtEnd( dtEnd ), m_objTask( objTask ) {
	}

	CWorkRecord::~CWorkRecord( ) {
	}
}
