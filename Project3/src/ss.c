#include <stdio.h>

int main() {
	FILE *fp = fopen("projectfile.txt", "w");
	char *s = "abcdefg";
    	fprintf(fp, "%s", s);

	return 0;
}
