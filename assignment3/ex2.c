#include <stdio.h>

char *s = "#include <stdio.h>%c%cchar *s = %c%s%c;%c%cvoid main() {%c    printf(s,10,10,34,s,34,10,10,10,10,10);%c}%c";

void main() {
    printf(s,10,10,34,s,34,10,10,10,10,10);
}
