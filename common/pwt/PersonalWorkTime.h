
#ifndef _PERSONAL_WORK_TIME_H
#define _PERSONAL_WORK_TIME_H

namespace pwt {

	class CPersonalWorkTime {
	public:
		enum PWTResult {
			PWTRESULT_OK,
			PWTRESULT_TASK_NOT_FOUND,
			PWTRESULT_INVALID_TASK_LABEL,
			PWTRESULT_INVALID_TASK_TITLE,
			PWTRESULT_INVALID_LIST_PATTERN
		};

	private:
		CConfig	m_objConfig;
		CLog	m_objLog;

	public:
		CPersonalWorkTime( );
		CPersonalWorkTime( const std::string& szConfigFile );

		virtual ~CPersonalWorkTime( );

		int	AddTask( const Glib::ustring& label, const Glib::ustring& title );
		int UpdTask( const Glib::ustring& label, const Glib::ustring& title );
		int DelTask( const Glib::ustring& label );
		int ListTask( const Glib::ustring& pattern );
		int StartTask( const Glib::ustring& label );
		int StopTask( const Glib::ustring& label );
	};

}

#endif
