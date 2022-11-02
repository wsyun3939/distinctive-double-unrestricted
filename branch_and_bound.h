#ifndef ___branch_and_bound
#define ___branch_and_bound

#include <time.h>
#include "IntDequeue.h"
#include "data_info.h"

#ifdef EITHER
typedef struct
{
	int idx;	  //*スタックの添え字*//
	int blocking; //*ブロッキングとなるかどうか*//
} LB_idx;
#else
typedef struct
{
	int idx;	  //*スタックの添え字*//
	int blocking; //*ブロッキングとなるかどうか*//
	direction dir;
} LB_idx;
#endif

int branch_and_bound(IntDequeue *q, int UB, int UB_cur, int LB, direction Dir, int DstDeque, clock_t start);

int insert_sort(IntDequeue *q);

int insert_media(IntDequeue *q, int stack_idx);

//*---優先順位の高くなったスタックを挿入ソート---*//
void insert_front(IntDequeue *q, int stack_idx);

//*---ブロックを取り出す方向を決める---*//
direction retrieval_direction(IntDequeue *q, int *LB);

direction pre_retrieval_direction(IntDequeue *q, int *LB);

//*---下界値優先探索のための比較関数---*//
int LBcmp(const LB_idx *n1, const LB_idx *n2);

//*---近似解法により上界値を求める---*//
int UpperBound(const IntDequeue *q);

#endif
