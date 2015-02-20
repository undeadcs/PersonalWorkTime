
#include <pwt/common.h>
#include <pwt/pwt.h>

void ShowUsage( ); // показ использования программы
void CmdAdd( int argc, char** argv );

int main( int argc, char** argv ) {
	try {
		if ( argc < 2 ) {
			ShowUsage( );
		} else {
			std::string cmd( argv[ 1 ] );

			if ( cmd == "add" ) {
				CmdAdd( argc, argv );
			}
		}
		//pwt::CPersonalWorkTime pwtMain;
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
		std::cout << "AddTask вернул непонятную ошибку" << std::endl;
		break;
	}
}
