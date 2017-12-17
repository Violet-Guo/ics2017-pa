#include <stdio.h>
#include <assert.h>

int main() {
	printf("in the text main!!\n");
	FILE *fp = fopen("/share/texts/num", "r+");
  assert(fp);

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  assert(size == 5000);

  fseek(fp, 500 * 5, SEEK_SET);
  int i, n;
  printf("in the 11111\n");
  for (i = 500; i < 1000; i ++) {
		fscanf(fp, "%d", &n);
    assert(n == i + 1);
  }

  fseek(fp, 0, SEEK_SET);
	printf("before 22222\n");
  for (i = 0; i < 500; i ++) {
    fprintf(fp, "%4d\n", i + 1 + 1000);
  }

	printf("before 33333\n");
  for (i = 500; i < 1000; i ++) {
    fscanf(fp, "%d", &n);
    assert(n == i + 1);
  }

	printf("before 44444\n");
  fseek(fp, 0, SEEK_SET);
  for (i = 0; i < 500; i ++) {
    fscanf(fp, "%d", &n);
    assert(n == i + 1 + 1000);
  }

  fclose(fp);

  printf("PASS!!!\n");

  return 0;
}
