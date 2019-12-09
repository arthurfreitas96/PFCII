#include <stdio.h>
#include <math.h>


int sunrise(int d){
    return 353.01038794989137 + 37.76769361817808 * sin(0.017154264597295837 * d - 13.767582285791088) + 9.765047933471491 * sin(0.03430852919459167 * d + 0.29167023527088937);
}
int sunset(int d){
    return 1077.6340472841093+ 34.31640727187583 * sin(0.017109590509994832 * d + 7.838272010108373)+ 9.991726175642217 * sin(0.034219181019989664 * d + 0.4515337407144377);
}

int main(void) {
	int d, r, s;
	while(scanf("%d %d %d", &d, &r, &s) == 3){
		int cr = sunrise(d);
		int cs = sunset(d);
		printf("%d,%d,%d,%d,%d\n", d, cr, cs, cr-r,cs-s);
	}
	return 0;
}
