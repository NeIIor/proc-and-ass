#ifndef COLORS_H__
#define COLORS_H__

#define RED    "\x1B[31m"
#define GREEN  "\x1B[32m"
#define WHITE  "\x1B[37m"

#define PRINT_RED(        s, ...) printf (       RED   s WHITE, ##__VA_ARGS__)
#define PRINT_GREEN(        s, ...) printf (       GREEN s WHITE, ##__VA_ARGS__)
#define PRINT_ERROR(file, s, ...) fprintf(file, RED   s WHITE, ##__VA_ARGS__) 

#endif //COLORS_H__