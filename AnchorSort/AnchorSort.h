int main();

struct cell;
int * getRandom(int n, int max);
void peakSort(int * nums, int n);
void printArray(cell * cells, int n);
void printArray(int * nums, int n);
int * sort(int * nums, int n);
cell cmp(int l, int ldist, int x, int r, int rdist);
bool isPeak(cell c);
bool isZero(cell c);
int lCell(cell * row, int hSize, int index);
int rCell(cell * row, int hSize, int index);

