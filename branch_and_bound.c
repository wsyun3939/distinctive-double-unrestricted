#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "IntDequeue.h"
#include "lower_bound.h"
#include "branch_and_bound.h"
#include "data_info.h"
#include "time.h"

#ifdef EITHER
int branch_and_bound(IntDequeue *q, int UB, int UB_cur, int LB, direction Dir, int DstDeque, clock_t start)
{
	static int num_ret;
	static int depth = 0;
	static int SecondPosition = 0;
	static int MinRelocation = 0;
	int BG_index[STACK][STACK] = {0};

	if (depth + LB == UB_cur - 1)
	{
		int UB_temp = UpperBound(q);
		if (UB > UB_temp + depth && UB_temp != -1)
		{
			UB = UB_temp + depth;
		}
	}
	
	if (UB == UB_cur)
	{
		depth = 0;
		return MinRelocation = UB;
	}
	int i = 0;
	int j = 0;
	int d = 0;
	int LB_temp = 0;
	int PriorityEdge = 0;
	direction dir = Dir;
	direction DirNext = Dir;
	IntDequeue *q_temp = NULL;
	static int ans = 0;
	clock_t end;

	end = clock();
	if (((double)(end - start) / CLOCKS_PER_SEC) > 7200)
	{
		depth = 0;
		return -1;
	}

	if (dir == both)
	{
		qsort(q + 1, STACK - 1, sizeof(IntDequeue), (int (*)(const void *, const void *))pricmp);

#if TEST == 0
		Array_print(q);
#endif

		while ((q[0].front == q[0].min_idx) || ((q[0].front + q[0].num - 1) % q[0].max == q[0].min_idx))
		{
			if (Array_check(q) == 1)
			{
				MinRelocation = LB + depth;
				depth = 0;
				return MinRelocation;
			}
			if (q[0].front == q[0].min_idx)
			{
				DequeFront(&q[0], &num_ret);

#if TEST == 0
				printf("Number Retrieval:%d\n", num_ret);
				Array_print(q);
#endif
			}
			else if ((q[0].front + q[0].num - 1) % q[0].max == q[0].min_idx)
			{
				DequeRear(&q[0], &num_ret);

#if TEST == 0
				printf("Number Retrieval:%d\n", num_ret);
				Array_print(q);
#endif
			}
			if (insert_sort(q))
			{
				depth = 0;
				return MinRelocation = UB_cur;
			}

#if TEST == 0
			Array_print(q);
#endif
		}
	}
	if (Array_check(q) == 1)
	{
		MinRelocation = LB + depth;
		depth = 0;
		return MinRelocation;
	}

	depth++;

#if TEST == 0
	printf("Block relocation(depth=%d)\n", depth);
#endif

	//　積み替え操作　デックiからデックjへ
	// BGもしくはGG積み替えを優先．
	//ない場合は，優先順位の最も低いデックに積んでおく
	for (i = 0; i < STACK; i++)
	{
		dir = q[i].dir;
		if (dir == both)
			dir = q[i].que[q[i].front] < q[i].que[(q[i].front + q[i].num - 1) % q[i].max] ? lower : upper;
		for (d = 0; d <= 1; d++)
		{ //下界値を与える方向から順に積み替えを吟味する
			if (d == 1)
			{
				if (dir == lower)
					dir = upper;
				else
					dir = lower;
			}
			if (dir == lower)
			{
				PriorityEdge = q[i].que[q[i].front];
				SecondPosition = (q[i].front + 1) % q[i].max;
			}
			else if (dir == upper)
			{
				PriorityEdge = q[i].que[(q[i].front + q[i].num - 1) % q[i].max];
				SecondPosition = (q[i].front + q[i].num - 2) % q[i].max;
			}
			LB_temp = LB - q[i].LB;
			Deque(&q[i], &num_ret, dir);
			LB_temp += q[i].LB;
			if (LB_temp + depth > UB_cur)
				;
			else
			{
				if (i == 0)
				{ //ターゲットデックにおける積み替え
					if (SecondPosition == q[0].min_idx)
					{ //積み替え後，取り出しが可能な場合
						DirNext = both;
						q_temp = malloc(STACK * (sizeof *q_temp));
						Array_initialize(q_temp);
						Array_copy(q_temp, q);
						if (PriorityEdge == q[0].que[q[0].min_idx] + 1)
						{ //ドミナンス

#if TEST == 0
							printf("Priority Retrieval:%d,%d\n", num_ret - 1, num_ret);
#endif

							Deque(&q_temp[0], &num_ret, dir);

							insert_media(q_temp, 0);
							ans = branch_and_bound(q_temp, UB, UB_cur, LB_temp, DirNext, DstDeque, start);
							if (ans != 0 && ans != -1)
							{
								Array_terminate(q_temp);
								free(q_temp);
								return MinRelocation;
							}
							else if (ans == -1)
							{
								Array_terminate(q_temp);
								free(q_temp);
								return -1;
							}

#if TEST == 0
							printf("Lower termination.\n");
							Array_print(q_temp);
#endif
						}
						else
						{
							for (j = 1; j < STACK; j++)
							{
								BG_index[i][j] = Enque(&q_temp[j], PriorityEdge, dir);
								if (BG_index[i][j] != -1)
									Deque(&q_temp[j], &num_ret, dir);
							}
							for (j = STACK - 1; j > 0; j--)
							{
								if (BG_index[i][j] == 0)
								{
									Enque(&q_temp[j], PriorityEdge, dir);

#if TEST == 0
									printf("relocation(%d,%d)\n", i, j);
									Array_print(q_temp);
#endif

									ans = branch_and_bound(q_temp, UB, UB_cur, LB_temp, DirNext, j, start);
									if (ans != 0 && ans != -1)
									{
										Array_terminate(q_temp);
										free(q_temp);
										return MinRelocation;
									}
									else if (ans == -1)
									{
										Array_terminate(q_temp);
										free(q_temp);
										return -1;
									}
									Array_copy(q_temp, q);

#if TEST == 0
									Array_print(q_temp);
#endif
								}
							}
						}
						Array_terminate(q_temp);
						free(q_temp);
					}
					else
					{
						for (j = 1; j < STACK; j++)
						{
							BG_index[i][j] = Enque(&q[j], PriorityEdge, dir);
							if (BG_index[i][j] != -1)
								Deque(&q[j], &num_ret, dir);
						}
						for (j = STACK - 1; j > 0; j--)
						{
							if (BG_index[i][j] == 0 && !(i == DstDeque && dir == Dir))
							{
								Enque(&q[j], PriorityEdge, dir);

#if TEST == 0
								printf("relocation(%d,%d)\n", i, j);
								Array_print(q);
#endif

								ans = branch_and_bound(q, UB, UB_cur, LB_temp, lower, j, start);
								if (ans != 0 && ans != -1)
								{
									return MinRelocation;
								}
								else if (ans == -1)
								{
									return -1;
								}
								Deque(&q[j], &num_ret, dir);

#if TEST == 0
								Array_print(q);
#endif
							}
						}
					}
				}
				else
				{
					for (j = 0; j < STACK; j++)
					{
						if (j == i)
							continue;
						else
						{
							BG_index[i][j] = Enque(&q[j], PriorityEdge, dir);
							if (BG_index[i][j] != -1)
								Deque(&q[j], &num_ret, dir);
						}
					}
					for (j = STACK - 1; j >= 0; j--)
					{
						if (j == i)
							continue;
						else
						{
							if (BG_index[i][j] == 0 && j != i && !(i == DstDeque && dir == Dir))
							{
								Enque(&q[j], PriorityEdge, dir);

#if TEST == 0
								printf("relocation(%d,%d)\n", i, j);
								Array_print(q);
#endif

								ans = branch_and_bound(q, UB, UB_cur, LB_temp, lower, j, start);
								if (ans != 0 && ans != -1)
								{
									return MinRelocation;
								}
								else if (ans == -1)
								{
									return -1;
								}
								Deque(&q[j], &num_ret, dir);

#if TEST == 0
								Array_print(q);
#endif
							}
						}
					}
				}
			}
			Enque(&q[i], PriorityEdge, dir);

#if TEST == 0
			Array_print(q);
#endif
		}
	}

	//　積み替え操作　デックiからデックjへ
	//ブロッキングとなる積み替え
	for (i = 0; i < STACK; i++)
	{
		dir = q[i].dir;
		if (dir == both)
			dir = q[i].que[q[i].front] < q[i].que[(q[i].front + q[i].num - 1) % q[i].max] ? lower : upper;
		for (d = 0; d <= 1; d++)
		{ //下界値を与える方向から順に積み替えを吟味する
			if (d == 1)
			{
				if (dir == lower)
					dir = upper;
				else
					dir = lower;
			}
			if (dir == lower)
			{
				PriorityEdge = q[i].que[q[i].front];
				SecondPosition = (q[i].front + 1) % q[i].max;
			}
			else if (dir == upper)
			{
				PriorityEdge = q[i].que[(q[i].front + q[i].num - 1) % q[i].max];
				SecondPosition = (q[i].front + q[i].num - 2) % q[i].max;
			}
			LB_temp = LB - q[i].LB;
			Deque(&q[i], &num_ret, dir);
			LB_temp += q[i].LB;
			if (LB_temp + depth + 1 > UB_cur)
				;
			else
			{
				if (i == 0)
				{ //ターゲットデックにおける積み替え
					if (SecondPosition == q[0].min_idx)
					{ //積み替え後，取り出しが可能な場合
						DirNext = both;
						q_temp = malloc(STACK * (sizeof *q_temp));
						Array_initialize(q_temp);
						Array_copy(q_temp, q);
						Deque(&q_temp[0], &num_ret, dir);
						for (j = 1; j < STACK; j++)
						{
							BG_index[i][j] = Enque(&q_temp[j], PriorityEdge, dir);
							if (BG_index[i][j] != -1)
								Deque(&q_temp[j], &num_ret, dir);
						}
						for (j = STACK - 1; j > 0; j--)
						{
							if (BG_index[i][j] == 1 && !(i == DstDeque && dir == Dir))
							{
								Enque(&q_temp[j], PriorityEdge, dir);

#if TEST == 0
								printf("relocation(%d,%d)\n", i, j);
								Array_print(q_temp);
#endif

								ans = branch_and_bound(q_temp, UB, UB_cur, LB_temp + 1, DirNext, j, start);
								if (ans != 0 && ans != -1)
								{
									Array_terminate(q_temp);
									free(q_temp);
									return MinRelocation;
								}
								else if (ans == -1)
								{
									Array_terminate(q_temp);
									free(q_temp);
									return -1;
								}
								Array_copy(q_temp, q);

#if TEST == 0
								Array_print(q_temp);
#endif
							}
						}
						Array_terminate(q_temp);
						free(q_temp);
					}
					else
					{
						for (j = 1; j < STACK; j++)
						{
							BG_index[i][j] = Enque(&q[j], PriorityEdge, dir);
							if (BG_index[i][j] != -1)
								Deque(&q[j], &num_ret, dir);
						}
						for (j = STACK - 1; j >= 0; j--)
						{
							if (BG_index[i][j] == 1 && !(i == DstDeque && dir == Dir))
							{
								Enque(&q[j], PriorityEdge, dir);

#if TEST == 0
								printf("relocation(%d,%d)\n", i, j);
								Array_print(q);
#endif

								ans = branch_and_bound(q, UB, UB_cur, LB_temp + 1, lower, j, start);
								if (ans != 0 && ans != -1)
								{
									return MinRelocation;
								}
								else if (ans == -1)
								{
									return -1;
								}
								Deque(&q[j], &num_ret, dir);

#if TEST == 0
								Array_print(q);
#endif
							}
						}
					}
				}
				else
				{
					for (j = 0; j < STACK; j++)
					{
						if (j == i)
							continue;
						else
						{
							BG_index[i][j] = Enque(&q[j], PriorityEdge, dir);
							if (BG_index[i][j] != -1)
								Deque(&q[j], &num_ret, dir);
						}
					}
					for (j = STACK - 1; j >= 0; j--)
					{
						if (BG_index[i][j] == 1 && j != i && !(i == DstDeque && dir == Dir))
						{
							Enque(&q[j], PriorityEdge, dir);

#if TEST == 0
							printf("relocation(%d,%d)\n", i, j);
							Array_print(q);
#endif

							ans = branch_and_bound(q, UB, UB_cur, LB_temp + 1, lower, j, start);
							if (ans != 0 && ans != -1)
							{
								return MinRelocation;
							}
							else if (ans == -1)
							{
								return -1;
							}
							Deque(&q[j], &num_ret, dir);

#if TEST == 0
							Array_print(q);
#endif
						}
					}
				}
			}
			Enque(&q[i], PriorityEdge, dir);

#if TEST == 0
			Array_print(q);
#endif
		}
	}

	depth--;
	if (depth == 0)
	{
		UB_cur++;

#if TEST == 0
		printf("UB++\n");
#endif

		ans = branch_and_bound(q, UB, UB_cur, LB, both, DstDeque, start);
		if (ans != 0 && ans != -1)
		{
			return MinRelocation;
		}
		else if (ans == -1)
			return -1;
		return -1;
	}
	else
	{
		return 0;
	}
}
#endif
