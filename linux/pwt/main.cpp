
#include <pwt/common.h>
#include <pwt/pwt.h>

int main( int argc, char** argv ) {
	try {
		pwt::CPersonalWorkTime pwtMain;
	}
	catch( std::exception& e ) {
		std::cout << "Exception: " << e.what( ) << std::endl;

		return EXIT_FAILURE;
	}
	catch( ... ) {
		std::cout << "unknown exception" << std::endl;

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
