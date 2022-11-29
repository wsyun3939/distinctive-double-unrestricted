#include <stdio.h>
#include <stdlib.h>
#include "IntDequeue.h"
#include "lower_bound.h"
#include "branch_and_bound.h"
#include "data_info.h"

int insert_sort(IntDequeue *q)
{
	IntDequeue q_temp = q[0];
	int i;
	if (IsEmpty(&q[0]))
	{
		for (i = 1; i < STACK; i++)
		{
			if (IsEmpty(&q[i]))
			{
				break;
			}
			q[i - 1] = q[i];
		}
		q[i - 1] = q_temp;
		if (i == 1)
			return 1;
		return 0;
	}
	else
	{
		for (i = 1; i < STACK; i++)
		{
			if (IsEmpty(&q[i]))
			{
				break;
			}
			if (q_temp.que[q_temp.min_idx] < q[i].que[q[i].min_idx])
			{
				break;
			}
			q[i - 1] = q[i];
		}
		q[i - 1] = q_temp;
		return 0;
	}
}

int insert_media(IntDequeue *q, int stack_idx)
{
	IntDequeue q_temp = q[stack_idx];
	int i = 0;
	if (stack_idx != STACK - 1)
	{
		if (IsEmpty(&q_temp))
		{
			for (i = stack_idx + 1; i < STACK; i++)
			{
				q[i - 1] = q[i];
			}
			q[i - 1] = q_temp;
		}
		else
		{
			for (i = stack_idx + 1; i < STACK; i++)
			{
				if (q_temp.que[q_temp.min_idx] < q[i].que[q[i].min_idx] || IsEmpty(&q[i]))
				{
					break;
				}
				q[i - 1] = q[i];
			}
			q[i - 1] = q_temp;
		}
		if (i != stack_idx + 1)
			return i - 1;
	}
	if (stack_idx != 0)
	{
		if (IsEmpty(&q_temp))
			;
		else
		{
			for (i = stack_idx - 1; i >= 0; i--)
			{
				if (IsEmpty(&q[i]))
					;
				else if (q_temp.que[q_temp.min_idx] > q[i].que[q[i].min_idx])
				{
					break;
				}
				q[i + 1] = q[i];
			}
			q[i + 1] = q_temp;
		}
		return i + 1;
	}
	return 0;
}

//*---優先順位の高くなったスタックを挿入ソート---*//
void insert_front(IntDequeue *q, int stack_idx)
{
	IntDequeue q_temp = q[stack_idx];
	int i = 0;
	if (stack_idx == 0)
		return;
	else
	{
		for (i = stack_idx - 1; i >= 0; i--)
		{
			if (IsEmpty(&q[i]))
				;
			else if (q_temp.que[q_temp.min_idx] > q[i].que[q[i].min_idx])
			{
				break;
			}
			q[i + 1] = q[i];
		}
		q[i + 1] = q_temp;
		return;
	}
}

//*---ブロックを取り出す方向を決める---*//
direction retrieval_direction(IntDequeue *q, int *LB)
{
	int Nblocking_upper = nblocking_upper(q);
	int Nblocking_lower = nblocking_lower(q);
	direction dir;
	if (q->num < 6)
	{
		dir = Nblocking_upper < Nblocking_lower ? upper : Nblocking_upper > Nblocking_lower ? lower
																							: both;
		if (dir == both)
			*LB = Nblocking_lower;
		else
			*LB = min_relocation(q, dir);
	}
	else
	{
		IntDequeue *q_temp = NULL;
		if (Nblocking_lower < Nblocking_upper)
		{
			if ((Nblocking_lower + 1 + num_expo(Nblocking_upper)) < Nblocking_upper)
			{
				dir = lower;
				*LB = min_relocation(q, dir);
			}
			else
			{
				q_temp = malloc(sizeof(*q_temp));
				Initialize(q_temp);
				Copy(q_temp, q);
				//*---上側から取り出した場合の積み替え最小回数---*//
				Nblocking_upper = min_relocation(q, upper);

				//*---下側から取り出した場合の積み替え最小回数---*//
				Nblocking_lower = min_relocation(q_temp, lower);

				Terminate(q_temp);
				free(q_temp);
				dir = Nblocking_upper < Nblocking_lower ? upper : Nblocking_upper > Nblocking_lower ? lower
																									: both;
				if (dir == upper)
					*LB = Nblocking_upper;
				else
					*LB = Nblocking_lower;
			}
		}
		else
		{
			if ((Nblocking_upper + 1 + num_expo(Nblocking_lower)) < Nblocking_lower)
			{
				dir = upper;
				*LB = min_relocation(q, dir);
			}
			else
			{
				q_temp = malloc(sizeof(*q_temp));
				Initialize(q_temp);
				Copy(q_temp, q);
				//*---上側から取り出した場合の積み替え最小回数---*//
				Nblocking_upper = min_relocation(q, upper);

				//*---下側から取り出した場合の積み替え最小回数---*//
				Nblocking_lower = min_relocation(q_temp, lower);

				Terminate(q_temp);
				free(q_temp);
				dir = Nblocking_upper < Nblocking_lower ? upper : Nblocking_upper > Nblocking_lower ? lower
																									: both;
				if (dir == upper)
					*LB = Nblocking_upper;
				else
					*LB = Nblocking_lower;
			}
		}
	}
	return dir;
}

direction pre_retrieval_direction(IntDequeue *q, int *LB)
{
	int Nblocking_upper = nblocking_upper(q);
	int Nblocking_lower = nblocking_lower(q);
	direction dir;
	if (q->num <= 2)
	{
		dir = Nblocking_upper < Nblocking_lower ? upper : Nblocking_upper > Nblocking_lower ? lower
																							: both;
		if (dir == both)
			*LB = Nblocking_lower;
		else
			*LB = min_relocation(q, dir);
	}
	else
	{
		IntDequeue *q_temp = NULL;
		if (Nblocking_lower < Nblocking_upper)
		{
			q_temp = malloc(sizeof(*q_temp));
			Initialize(q_temp);
			Copy(q_temp, q);
			//*---上側から取り出した場合の積み替え最小回数---*//
			Nblocking_upper = min_relocation(q, upper);

			//*---下側から取り出した場合の積み替え最小回数---*//
			Nblocking_lower = min_relocation(q_temp, lower);

			Terminate(q_temp);
			free(q_temp);
			dir = Nblocking_upper < Nblocking_lower ? upper : Nblocking_upper > Nblocking_lower ? lower
																								: both;
			if (dir == upper)
				*LB = Nblocking_upper;
			else
				*LB = Nblocking_lower;
		}
		else
		{
			q_temp = malloc(sizeof(*q_temp));
			Initialize(q_temp);
			Copy(q_temp, q);
			//*---上側から取り出した場合の積み替え最小回数---*//
			Nblocking_upper = min_relocation(q, upper);

			//*---下側から取り出した場合の積み替え最小回数---*//
			Nblocking_lower = min_relocation(q_temp, lower);

			Terminate(q_temp);
			free(q_temp);
			dir = Nblocking_upper < Nblocking_lower ? upper : Nblocking_upper > Nblocking_lower ? lower
																								: both;
			if (dir == upper)
				*LB = Nblocking_upper;
			else
				*LB = Nblocking_lower;
		}
	}
	return dir;
}

//*---下界値優先探索のための比較関数---*//
int LBcmp(const LB_idx *n1, const LB_idx *n2)
{
	return n1->blocking < n2->blocking ? -1 : n1->blocking > n2->blocking ? 1
																		  : 0;
}

int UpperBound(const IntDequeue *q)
{
	IntDequeue *q_temp = malloc(STACK * (sizeof *q_temp));
	Array_initialize(q_temp);
	Array_copy(q_temp, q);
	int num_ret;
	int i = 0;
	int j = 0;
	int k = 0;
	int UB = 0;
	int PriorityEdge = 0;
	direction dir = lower;
	direction DirNext = lower;

	qsort(q_temp + 1, STACK - 1, sizeof(IntDequeue), (int (*)(const void *, const void *))pricmp);
	while (!(IsEmpty(&q_temp[0])))
	{
		switch (DirNext)
		{
		case both:
			qsort(q_temp + 1, STACK - 1, sizeof(IntDequeue), (int (*)(const void *, const void *))pricmp);

#if TEST == 0
			Array_print(q_temp);
#endif

			break;
		}

		while ((q_temp[0].front == q_temp[0].min_idx) || ((q_temp[0].front + q_temp[0].num - 1) % q_temp[0].max == q_temp[0].min_idx))
		{
			if (q_temp[0].front == q_temp[0].min_idx)
			{
				DequeFront(&q_temp[0], &num_ret);

#if TEST == 0
				printf("Number Retrieval:%d\n", num_ret);
				Array_print(q_temp);
#endif
			}
			else if ((q_temp[0].front + q_temp[0].num - 1) % q_temp[0].max == q_temp[0].min_idx)
			{
				DequeRear(&q_temp[0], &num_ret);

#if TEST == 0
				printf("Number Retrieval:%d\n", num_ret);
				Array_print(q_temp);
#endif
			}
			if (insert_sort(q_temp))
			{
				Array_terminate(q_temp);
				free(q_temp);
				return UB;
			}

#if TEST == 0
			Array_print(q_temp);
#endif
		}
		//　積み替え操作　
		UB++;

#if TEST == 0
		printf("Block Relocation(depth=%d)\n", UB);
#endif

		dir = q_temp[0].dir;
		int low = nblocking_lower(q_temp);
		int upp = nblocking_upper(q_temp);
		if (dir == both)
		{
			if (low > upp)
			{
				dir = upper;
			}
			else if (upp > low)
			{
				dir = lower;
			}
			else
			{
				dir = q_temp[0].que[q_temp[0].front] < q_temp[0].que[(q_temp[0].front + q_temp[0].num - 1) % q_temp[0].max] ? lower : upper;
			}
		}
		if (dir == lower)
		{
			PriorityEdge = q_temp[0].que[q_temp[0].front];
		}
		else if (dir == upper)
		{
			PriorityEdge = q_temp[0].que[(q_temp[0].front + q_temp[0].num - 1) % q_temp[0].max];
		}
		if (nblocking(q_temp, dir) == 1)
			DirNext = both;
		else
			DirNext = upper;
		Deque(&q_temp[0], &num_ret, dir);
		for (j = STACK - 1; j >= 0; j--)
		{
			if (Enque(&q_temp[j], PriorityEdge, dir) != -1)
			{

#if TEST == 0
				Array_print(q_temp);
#endif

				break;
			}
		}
	}
	Array_terminate(q_temp);
	free(q_temp);
	return UB;
}
