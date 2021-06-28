#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int board[128] = {

     0,  0,  0,  0, 19,  0,  0,  0,    0,  0,  5,  5,  0,  0,  5,  0,
     0,  0,  0,  0,  0,  0,  0,  0,    5,  5,  0,  0,  0,  0,  5,  5,
     0,  0,  0,  0,  0,  0,  0,  0,    5, 10, 15, 20, 20, 15, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0,    5, 10, 20, 30, 30, 20, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0,    5, 10, 20, 30, 30, 20, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0,    5, 10, 15, 20, 20, 15, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0,    5,  5,  0,  0,  0,  0,  5,  5,
    14,  0,  0,  0, 11,  0,  0, 14,    0,  0,  5,  5,  0,  0,  5,  0

};

char *notation[] = {

    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",     "i8","j8","k8","l8","m8","n8","o8", "p8",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",     "i7","j7","k7","l7","m7","n7","o7", "p7",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",     "i6","j6","k6","l6","m6","n6","o6", "p6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",     "i5","j5","k5","l5","m5","n5","o5", "p5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",     "i4","j4","k4","l4","m4","n4","o4", "p4",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",     "i3","j3","k3","l3","m3","n3","o3", "p3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",     "i2","j2","k2","l2","m2","n2","o2", "p2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",     "i1","j1","k1","l1","m1","n1","o1", "p1",

};


char pieces[] = ".--k-------K--R";

enum { WHITE = 8, BLACK = 16};

static int move_offsets[] = {

    0,   0,   0,   0,
    0,   0,   0,   0,
    1,  16,  -1, -16,   0,
    1,  16,  -1, -16,  15, -15, 17, -17,  0,
    0,   0,  12,   0,   0,  7

};

int piece_weights[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 500, 0 };
int best_src, best_dst;


void PrintBoard()
{
    for(int sq = 0; sq < 128; sq++)
    {
        if(!(sq % 16))
        {
                printf(" %d  ", 8 - (sq / 16));
        }
        printf(" %c", ((sq & 8) && (sq += 7)) ? '\n' : pieces[board[sq] & 15]);
    }

    printf("\n     a b c d e f g h\n\nYour move: \n");
}

int SearchPosition(int side, int depth, int alpha, int beta)
{
    if(!depth)
    {
        int mat_score = 0, pos_score = 0, pce, eval = 0;

        for(int sq = 0; sq < 128; sq++)
        {
            if(!(sq & 0x88))
            {
                if(pce = board[sq])
                {
                    mat_score += piece_weights[pce & 15];
                    (pce & 8) ? (pos_score += board[sq + 8]) : (pos_score -= board[sq + 8]);
                }
            }
        }

        eval = mat_score + pos_score;

        return (side == 8) ? eval : -eval;
    }

    int old_alpha = alpha;
    int temp_src;
    int temp_dst;
    int score = -10000;


    int piece, type, directions, dst_square, captured_square, captured_piece, step_vector;

    for(int src_square = 0; src_square < 128; src_square++)
    {
        if(!(src_square & 0x88))
        {
            piece = board[src_square];

            if(piece & side)
            {
                type = piece & 7;
                directions = move_offsets[type + 21];

                while(step_vector = move_offsets[++directions])
                {
                    dst_square = src_square;

                    do
                    {
                        dst_square += step_vector;
                        captured_square = dst_square;

                        if(dst_square & 0x88)
                        {
                            break;
                        }
                        captured_piece = board[captured_square];

                        if(captured_piece & side)
                        {
                            break;
                        }

                        if((captured_piece & 7) == 3)
                        {
                              return 10000;
                        }

                        board[captured_square] = 0;
                        board[src_square] = 0;
                        board[dst_square] = piece;

                        score = -SearchPosition(24 - side, depth - 1, -beta, -alpha);


                        board[dst_square] = 0;
                        board[src_square] = piece;
                        board[captured_square] = captured_piece;

                        best_src = src_square;
                        best_dst = dst_square;

                        if(score > alpha)
                        {
                            if(score >= beta)
                            {
                                return beta;
                            }

                            alpha = score;

                            temp_src = src_square;
                            temp_dst = dst_square;
                        }

                        captured_piece += type < 5;

                    }while(!captured_piece);

                }
            }
        }
    }
    if(alpha != old_alpha)
    {
        best_src = temp_src;
        best_dst = temp_dst;
    }

    return alpha;
}

int main()
{
	FILE *fptr;

	fptr = fopen("chess_text.txt","w");

	if(fptr == NULL)
  	{
      		printf("Error!");
      		exit(1);
   	}
	fprintf(fptr,"Statistika na igrata");
	fprintf(fptr,"\n");
	fprintf(fptr,"\n");

	printf("========================================================================================\n");
	printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
	printf("$$$$$$$$$$$$$$$$$$$$$$______$$__$$$$__$$_______$$$_______ $$_______$$$$$$$$$$$$$$$$$$$$$\n");
	printf("$$$$$$$$$$$$$$$$$$$$$/ _____||  |$$|  ||   ____|$/  _____|$/   ____|$$$$$$$$$$$$$$$$$$$$\n");
	printf("$$$$$$$$$$$$$$$$$$$$|  |$$$$$|  |$$|  ||  |____ | (_____$$|   (____$$$$$$$$$$$$$$$$$$$$$\n");
	printf("$$$$$$$$$$$$$$$$$$$$|  |$$$$$|   __   ||   ____|$|____   |$|____   |$$$$$$$$$$$$$$$$$$$$\n");
	printf("$$$$$$$$$$$$$$$$$$$$|  |____$|  |$$|  ||  |____$$_____)  |$_____)  |$$$$$$$$$$$$$$$$$$$$\n");
	printf("$$$$$$$$$$$$$$$$$$$$$|______||__|$$|__||_______||_______/$|_______/$$$$$$$$$$$$$$$$$$$$$\n");
	printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
	printf("======================================================================================= \n");

	printf("Dobre ste doshli v igrata CHESS!\n");
	printf("Za da igraete, trqbva da polzwate mestopolovenieto na figurata vi i da dobavite mestopolojenieto \n kude iskate da q mestite, naprimer figurata vi se namira na a1 \n i iskate da q postavite na a8, pishete prosto: a1a8");
	printf("\n I se e premestila! \n");
	printf("\n");



    char user_move[5];

    int depth = 3;
    int side = WHITE;

    PrintBoard();

    while(1)
    {
        memset(&user_move[0], 0, sizeof(user_move));

        if(!fgets(user_move, 5, stdin))
        {

            continue;
        }
        if(user_move[0] == '\n')
        {
            continue;
        }

        fprintf(fptr,"Hod na igracha: %s\n",user_move);

        int user_src, user_dst;

        for(int sq = 0; sq < 128; sq++)
        {
            if(!(sq & 0x88))
            {
                if(!strncmp(user_move, notation[sq], 2))
                {
                    user_src = sq;
                }

                if(!strncmp(user_move + 2, notation[sq], 2))
                {
                    user_dst = sq;
                }
            }
        }


        board[user_dst] = board[user_src];
        board[user_src] = 0;

        PrintBoard();

        side = 24 - side;

        int score = SearchPosition(side, depth, -10000, 10000);

        board[best_dst] = board[best_src];
        board[best_src] = 0;

	fprintf(fptr,"Hod na AI: %s%s\n",notation[best_src], notation[best_dst]);

        side = 24 - side;

        PrintBoard();
        if(score == 10000 || score == -10000)
        {
            printf("Checkmate!\n");
            break;
        }

    }

    char duma;

    printf("\n Wanna play again? (y/n): ");

    scanf(" %s" , &duma);


    if(duma == 'y' || duma == 'Y')
    {
	    main();

    } else{

	    return 0;
    }

    fclose(fptr);
    return 0;
}
