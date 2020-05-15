#include <stdio.h>

#include "cores.h"

void reset(void) {
	printf("%s", DEFAULT);
}

void white(void) {
	printf("\033[090m");
}

void red(void) {
	printf("%s", RED);
}

void green(void) {
	printf("%s", GREEN);
}

void yellow(void) {
	printf("%s", YELLOW);
}

void blue(void) {
	printf("%s", BLUE);
}

void cyan(void) {
	printf("%s", CYAN);
}

void bold_green(void) {
	printf("%s", BOLD_GREEN);
}

void bold_blue(void) {
	printf("%s", BOLD_BLUE);
}

void bold_cyan(void) {
	printf("%s", BOLD_CYAN);
}

void bold_red(void) {
	printf("%s", BOLD_RED);
}

void bold_yellow(void) {
	printf("%s", BOLD_YELLOW);
}

