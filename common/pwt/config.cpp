
#include "common.h"
#include "config.h"

namespace pwt {

	CConfig::CConfig( ) : m_szLog( "pwt.log" ), m_szDatabase( "pwt.sqlite" ) {
		LoadConfig( "pwt.config" );
	}

	CConfig::CConfig( const std::string& filename ) : m_szLog( "pwt.log" ), m_szDatabase( "pwt.sqlite" ) {
		LoadConfig( filename );
	}

	CConfig::~CConfig( ) {
	}

	void CConfig::LoadConfig( const std::string& filename ) {
		xmlpp::DomParser parser;

		//parser.set_validate( true );
		parser.set_substitute_entities( true );
		parser.parse_file( Glib::ustring( filename ) );

		if ( !parser ) {
			throw CExceptionConfigOpen( );
		}

		xmlpp::Node *pRoot = parser.get_document( )->get_root_node( );
		if ( pRoot->get_name( ) != "config" ) {
			throw CExceptionConfigInvalid( );
		}

		bool	bFoundLog		= false,
				bFoundDatabase	= false;

		const xmlpp::Node::NodeList lstLog = pRoot->get_children( );
		const xmlpp::Element* elem = NULL;
		const xmlpp::TextNode* txt = NULL;

		for( xmlpp::Node::NodeList::const_iterator i = lstLog.begin( ); i != lstLog.end( ); ++i ) {
			elem = dynamic_cast< const xmlpp::Element* >( *i );
			if ( elem ) {
				if ( ( elem->get_name( ) == "log" ) && ( txt = elem->get_child_text( ) ) && !txt->get_content( ).empty( ) ) {
					m_szLog = txt->get_content( );
					bFoundLog = true;
				}
				if ( ( elem->get_name( ) == "database" ) && ( txt = elem->get_child_text( ) ) && !txt->get_content( ).empty( ) ) {
					m_szDatabase = txt->get_content( );
					bFoundDatabase = true;
				}
			}

			if ( bFoundLog && bFoundDatabase ) {
				break;
			}
		}

		if ( !bFoundLog || !bFoundDatabase ) {
			throw CExceptionConfigInvalid( );
		}
	}
}
