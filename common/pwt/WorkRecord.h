
#ifndef _PWT_WORK_RECORD_H
#define _PWT_WORK_RECORD_H

namespace pwt {

	class CWorkRecord {
		Glib::ustring	m_dtBegin,
						m_dtEnd;
		CTask			m_objTask;

	public:
		CWorkRecord( );
		CWorkRecord( const Glib::ustring& dtBegin, const Glib::ustring& dtEnd, const CTask& objTask );

		virtual ~CWorkRecord( );

		Glib::ustring	GetBegin( )	const { return m_dtBegin;	}
		Glib::ustring	GetEnd( )	const { return m_dtEnd;		}
		CTask			GetTask( )	const { return m_objTask;	}
	};

}

#endif

