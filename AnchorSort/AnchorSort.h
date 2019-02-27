int main();

struct cell;
int * getRandom(int n, int max);
void peakSort(int * nums, int n);
void printArray(int * nums, cell * cells, int n);
void printArray(int * nums, int n);
int * sort(int * nums, int n);
cell cmp(int * nums, int l, int x, int r);
bool isPeak(int * nums, int n, cell * tableRow, int index);
bool isLastCellOfRow(cell c, int index);
int lCell(cell * row, int hSize, int index);
int rCell(cell * row, int hSize, int index);

