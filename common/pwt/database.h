
#ifndef _PWT_DATABASE_H
#define _PWT_DATABASE_H

namespace pwt {

	class CDatabase {
	public:
		CDatabase( );
		CDatabase( const std::string& filename );

		virtual ~CDatabase( );
	};

}

#endif
