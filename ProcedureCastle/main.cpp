#include "debug.h"
#include "painter.h"
#include "utility.h"

//TODO:
// write shaders
// fix logging code so that it actually writes the log/shows the error on crash
// test!


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


	Painter::screen(640, 480);
	
	while (Painter::runApp())
	{

		Painter::cls();

		Painter::line(
			rnd_intRange(0, 639), rnd_intRange(0, 479),
			rnd_intRange(0, 639), rnd_intRange(0, 479),
			Painter::Color_t(1.0, rnd(), rnd(), 1.0)
			);


		Painter::sync();
	}
	
	Painter::screen(1024, 768);

	Painter::cls();
	Painter::line(
		rnd_intRange(0, 639), rnd_intRange(0, 479),
		rnd_intRange(0, 639), rnd_intRange(0, 479),
		Painter::Color_t(1.0, rnd(), rnd(), 1.0)
		);
	Painter::sync();
	
	Painter::waitForKeyPress();

}