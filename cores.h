#ifndef CORES_H
#define CORES_H

#define DEFAULT "\e[0m"

#define YELLOW "\e[0;33m"
#define RED "\e[0;31m"
#define BLUE "\e[0;34m"
#define GREEN "\e[0;32m"
#define CYAN "\e[0;36m"

#define BOLD_YELLOW "\e[1;33m"
#define BOLD_RED "\e[1;31m"
#define BOLD_BLUE "\e[1;34m"
#define BOLD_GREEN "\e[1;32m"
#define BOLD_CYAN "\e[1;36m"

void reset(void);
void white(void);
void red(void);
void bold_red(void);
void green(void);
void yellow(void);
void blue(void);
void cyan(void);
void bold_yellow(void);

#endif
