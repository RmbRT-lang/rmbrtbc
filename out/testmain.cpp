

namespace __rl::c {
	extern "C" void * stderr;
	extern "C" void * stdout;
	extern "C" int fprintf(void *, char const *, ...);
}
int main(int argc, char ** argv)
{
	using namespace __rl;
	int succ, fail;
	__rl::test::status(succ, fail);

	void * out = fail ? c::stderr : c::stdout;

	c::fprintf(out, "%d/%d TESTS PASSED\n", succ, succ + fail);

	return fail ? 1 : 0;
}