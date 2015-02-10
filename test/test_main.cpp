#include"tut.h"
#include"tut_reporter.h"

namespace tut{
test_runner_singleton runner;

};


int main()
{
 	tut::reporter clbk;
 	tut::runner.get().set_callback( &clbk );
 	tut::runner.get().run_tests();
 	return 0;
}
