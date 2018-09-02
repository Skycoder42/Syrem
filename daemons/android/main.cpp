#include "syremservice.h"

int main(int argc, char *argv[])
{
	SyremService service{argc, argv};
	return service.exec();
}
