#include "vars.h"
#include "zcore.h"
#include "zresm.h"
#include <stdio.h>

#ifndef PC
int main(){
#else
int main(int argc, char *argv[]){//set axes for pc gamepad
    if(argc>0){
	printf("%d %d %d %d\n",argv[1][0],argv[2][0],argv[3][0],argv[4][0]);
        axes_pc[0] = argv[1][0]-48;
        axes_pc[1] = argv[2][0]-48;
        axes_pc[2] = argv[3][0]-48;
        axes_pc[3] = argv[4][0]-48;
    }
#endif
    zresminit();

    zcoreinit();
    zcoreloop();

    zresmdown();

    zcoredown();

    return 0;
}
