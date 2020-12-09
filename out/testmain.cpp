
extern void * stderr;
extern void * stdout;
extern int fprintf(void *, char *, ...);

int main(int argc, char ** argv)
{
	int succ, fail;
	__rl::test::status(succ, fail);

	void * out = fail ? stderr : stdout;

	fprintf(out, "%d/%d TESTS PASSED\n", succ, succ + fail);

	return fail ? 1 : 0;
}