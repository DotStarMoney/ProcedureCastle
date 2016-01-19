#include "debug.h"
//#include "painter.h"
#include "utility.h"
#include "linhash2.h"



//	TODO:
//		Create GraphicsCollection style bitmap fonts
//		Add in PC generation stuff



#if DEBUG_SHOULD_ERROR_CHECK
INITIALIZE_EASYLOGGINGPP
#endif

int main(int argv, const char** argc) 
{
#if DEBUG_SHOULD_ERROR_CHECK
	START_EASYLOGGINGPP(argv, argc);
	el::Configurations conf(DEBUG_LOG_CONFIG_FILE);
	el::Loggers::reconfigureAllLoggers(conf);
#endif



	
	

}