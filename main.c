#include "vars.h"
#include "zcore.h"
#include "zresm.h"

//int main(int argc, char *argv[]) //unused arguments
int main()
{
    zresminit();

    zcoreinit();
    zcoreloop();

    zresmdown();

    zcoredown();

    return 0;
}
