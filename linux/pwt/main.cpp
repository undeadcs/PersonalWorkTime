
#include <pwt/common.h>
#include <pwt/pwt.h>

void ShowUsage( ); // показ использования программы
void CmdAdd( int argc, char** argv );
void CmdList( int argc, char** argv );
void CmdUpd( int argc, char** argv );
void CmdDel( int argc, char** argv );
void CmdStart( int argc, char** argv );
void CmdStop( );
void CmdStatus( );

int main( int argc, char** argv ) {
	std::locale::global( std::locale( "" ) );

	try {
		if ( argc < 2 ) {
			ShowUsage( );
		} else {
			std::string cmd( argv[ 1 ] );

			if ( cmd == "add" ) {
				CmdAdd( argc, argv );
			} else if ( cmd == "list" ) {
				CmdList( argc, argv );
			} else if ( cmd == "upd" ) {
				CmdUpd( argc, argv );
			} else if ( cmd == "del" ) {
				CmdDel( argc, argv );
			} else if ( cmd == "start" ) {
				CmdStart( argc, argv );
			} else if ( cmd == "stop" ) {
				CmdStop( );
			} else if ( cmd == "status" ) {
				CmdStatus( );
			} else {
				ShowUsage( );
			}
		}
	}
	catch( std::exception& e ) {
		std::cout << "Exception: " << e.what( ) << std::endl;

		return EXIT_FAILURE;
	}
	/*catch( ... ) {
		std::cout << "unknown exception" << std::endl;

		return EXIT_FAILURE;
	}*/

	return EXIT_SUCCESS;
}

/**
 * Показ использования программы
 */
void ShowUsage( ) {
	std::cout << "pwt command [options]" << std::endl
		<< "command:" << std::endl
		<< "add task_label task_title" << std::endl << "  добавление задачи, task_label - метка задачи, task_title - наименование задачи" << std::endl
		<< "upd task_label task_new_title" << std::endl << "  изенение задачи, task_label - метка задачи, task_new_title - новое наименование задачи" << std::endl
		<< "del task_label" << std::endl << "  удаление задачи, task_label - метка задачи" << std::endl
		<< "copy task_label task_new_label task_new_title" << std::endl << "  создание копии задачи (копируются все записи о времени работы), task_label - метка задачи, task_new_label - метка новой задачи," << std::endl << "  task_new_title - наименование новой задачи" << std::endl
		<< "list text_pattern" << std::endl << "  вывод списка задач по шаблону наименования" << std::endl
		<< "start task_label" << std::endl << "  запуск работы над задачей, task_label - метка задачи" << std::endl
		<< "stop" << std::endl << "  остановка работы над текущей задачей" << std::endl
		<< "status" << std::endl << "  показ текущего статуса работы; выводит надпись \"статус: \" затем текущий статус, если нет текущей работы, то \"СВОБОДЕН\"," << std::endl << "  если есть, то наименование задачи, время текущего периода, время полное по данной задаче (сумма времени всех периодов)" << std::endl
		<< std::endl << "task_label - метка задачи должна состоять только из латиницы" << std::endl
		<< "task_title - название задачи не должно быть пустым" << std::endl
		<< std::endl;
}

void CmdAdd( int argc, char** argv ) {
	if ( argc < 4 ) {
		ShowUsage( );
		return;
	}

	pwt::CPersonalWorkTime pwtMain;
	pwt::CPersonalWorkTime::PWTResult result = pwtMain.AddTask( argv[ 2 ], argv[ 3 ] );

	switch( result ) {
	case pwt::CPersonalWorkTime::PWTRESULT_OK:
		std::cout << "задача добавлена" << std::endl;
		break;

	case pwt::CPersonalWorkTime::PWTRESULT_TASK_ALREADY_EXISTS:
		std::cout << "задача уже зарегистрирована" << std::endl;
		break;

	case pwt::CPersonalWorkTime::PWTRESULT_INVALID_TASK_LABEL:
		std::cout << "недопустимая метка" << std::endl;
		break;

	case pwt::CPersonalWorkTime::PWTRESULT_INVALID_TASK_TITLE:
		std::cout << "недопустимое название" << std::endl;
		break;

	default:
		std::cout << "AddTask вернул неизвстный результат" << std::endl;
		break;
	}
}

void CmdList( int argc, char** argv ) {
	if ( argc < 3 ) {
		ShowUsage( );
		return;
	}

	pwt::CPersonalWorkTime pwtMain;
	std::list< pwt::CTask > list = pwtMain.ListTask( Glib::ustring( argv[ 2 ] ) );

	if ( list.empty( ) ) {
		std::cout << "задач не найдено" << std::endl;
	} else {
		for( std::list< pwt::CTask >::const_iterator i = list.begin( ); i != list.end( ); ++i ) {
			std::cout << i->GetLabel( ) << " \"" << i->GetTitle( ) << "\"" << std::endl;
		}
	}
}

void CmdUpd( int argc, char** argv ) {
	if ( argc < 4 ) {
		ShowUsage( );
		return;
	}

	pwt::CPersonalWorkTime pwtMain;
	pwt::CPersonalWorkTime::PWTResult result = pwtMain.UpdTask( argv[ 2 ], argv[ 3 ] );

	switch( result ) {
	case pwt::CPersonalWorkTime::PWTRESULT_OK:
		std::cout << "задача обновлена" << std::endl;
		break;

	case pwt::CPersonalWorkTime::PWTRESULT_TASK_NOT_FOUND:
		std::cout << "задача не найдена" << std::endl;
		break;

	case pwt::CPersonalWorkTime::PWTRESULT_INVALID_TASK_LABEL:
		std::cout << "недопустимая метка" << std::endl;
		break;

	case pwt::CPersonalWorkTime::PWTRESULT_INVALID_TASK_TITLE:
		std::cout << "недопустимое название" << std::endl;
		break;

	default:
		std::cout << "UpdTask вернул неизвстный результат" << std::endl;
		break;
	}
}

void CmdDel( int argc, char** argv ) {
	if ( argc < 3 ) {
		ShowUsage( );
		return;
	}

	pwt::CPersonalWorkTime pwtMain;
	pwt::CPersonalWorkTime::PWTResult result = pwtMain.DelTask( argv[ 2 ] );

	switch( result ) {
	case pwt::CPersonalWorkTime::PWTRESULT_OK:
		std::cout << "задача удалена" << std::endl;
		break;

	case pwt::CPersonalWorkTime::PWTRESULT_TASK_NOT_FOUND:
		std::cout << "задача не найдена" << std::endl;
		break;

	default:
		std::cout << "DelTask вернул неизвстный результат" << std::endl;
		break;
	}
}

void CmdStart( int argc, char** argv ) {
	if ( argc < 3 ) {
		ShowUsage( );
		return;
	}

	pwt::CPersonalWorkTime pwtMain;
	pwt::CPersonalWorkTime::PWTResult result = pwtMain.StartTask( argv[ 2 ] );

	switch( result ) {
	case pwt::CPersonalWorkTime::PWTRESULT_OK:
		std::cout << "задача запущена" << std::endl;
		break;

	case pwt::CPersonalWorkTime::PWTRESULT_TASK_NOT_FOUND:
		std::cout << "задача не найдена" << std::endl;
		break;

	case pwt::CPersonalWorkTime::PWTRESULT_ALREADY_RUNNING:
		std::cout << "данная задача уже запущена" << std::endl;
		break;

	default:
		std::cout << "StartTask вернул неизвстный результат" << std::endl;
		break;
	}
}

void CmdStop( ) {
	pwt::CPersonalWorkTime pwtMain;
	pwt::CPersonalWorkTime::PWTResult result = pwtMain.StopTask( );

	switch( result ) {
	case pwt::CPersonalWorkTime::PWTRESULT_OK:
		std::cout << "задача остановлена" << std::endl;
		break;

	case pwt::CPersonalWorkTime::PWTRESULT_TASK_NOT_FOUND:
		std::cout << "задача не найдена" << std::endl;
		break;

	case pwt::CPersonalWorkTime::PWTRESULT_NOTHING_RUNNING:
		std::cout << "в данный момент ничего не запущено" << std::endl;
		break;

	default:
		std::cout << "StopTask вернул неизвестный результат" << std::endl;
		break;
	}
}

static int ToInt( const std::string& value ) {
	std::istringstream convert( value );
	int ret;

	return ( convert >> ret ) ? ret : 0;
}

static Glib::ustring GetTimeSpent( const Glib::ustring& szFrom ) {
	time_t tFrom, tTo;
	std::tm *tmFrom;

	time( &tTo );
	time( &tFrom );
	tmFrom = localtime( &tFrom );

	tmFrom->tm_year	= ToInt( szFrom.substr( 0, 4 ).c_str( ) ) - 1900;
	tmFrom->tm_mon	= ToInt( szFrom.substr( 5, 2 ).c_str( ) ) - 1;
	tmFrom->tm_mday	= ToInt( szFrom.substr( 8, 2 ).c_str( ) );
	tmFrom->tm_hour = ToInt( szFrom.substr( 11, 2 ).c_str( ) );
	tmFrom->tm_min	= ToInt( szFrom.substr( 14, 2 ).c_str( ) );
	tmFrom->tm_sec	= ToInt( szFrom.substr( 17, 2 ).c_str( ) );

	tFrom = mktime( tmFrom );

	double diff = difftime( tTo, tFrom );

	std::cout.setf( std::ios::fixed, std:: ios::floatfield );
	std::cout.precision( 6 );

	int days = diff / ( 60 * 60 * 24 );
	int hours = ( diff / ( 60 * 60 ) );
	int minutes = ( diff / 60 );
	int seconds = diff;

	hours %= 24;
	minutes %= 60;
	seconds %= 60;

	std::ostringstream out;

	if ( days ) {
		out << days << " дней ";
	}
	if ( hours ) {
		out << hours << " часов ";
	}
	if ( minutes ) {
		out << minutes << " минут ";
	}

	out << seconds << " секунд";

	return Glib::ustring( out.str( ) );
}

void CmdStatus( ) {
	pwt::CWorkRecord record;
	pwt::CPersonalWorkTime pwtMain;
	pwt::CPersonalWorkTime::PWTResult result = pwtMain.Status( record );

	switch( result ) {
	case pwt::CPersonalWorkTime::PWTRESULT_NOTHING_RUNNING:
		std::cout << std::endl << "статус: \"СВОБОДЕН\"" << std::endl << std::endl;
		break;

	case pwt::CPersonalWorkTime::PWTRESULT_ALREADY_RUNNING:
		std::cout << std::endl << "статус: \"РАБОТА НАД ЗАДАЧЕЙ\"" << std::endl;
		std::cout << "задача: " << record.GetTask( ).GetLabel( ) << " \"" << record.GetTask( ).GetTitle( ) << "\"" << std::endl;
		std::cout << "начата: " << record.GetBegin( ) << std::endl;
		std::cout << "затрачено времени: " << GetTimeSpent( record.GetBegin( ) ) << std::endl << std::endl;
		break;

	default:
		std::cout << "StopTask вернул неизвестный результат" << std::endl;
		break;
	}
}
