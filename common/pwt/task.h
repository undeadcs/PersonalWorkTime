
#ifndef _PWT_TASK_H
#define _PWT_TASK_H

namespace pwt {

	/**
	 * Задача
	 */
	class CTask {
		Glib::ustring	m_szLabel,
						m_szTitle;

	public:
		CTask( );
		CTask( const CTask& objCopy );
		CTask( const Glib::ustring& szLabel, const Glib::ustring& szTitle );

		virtual ~CTask( );

		CTask&	SetLabel( const Glib::ustring& szLabel	) { m_szLabel = szLabel;	return *this; }
		CTask&	SetTitle( const Glib::ustring& szTitle	) { m_szTitle = szTitle;	return *this; }

		Glib::ustring	GetLabel( ) const { return m_szLabel; }
		Glib::ustring	GetTitle( ) const { return m_szTitle; }
	};

}

#endif
