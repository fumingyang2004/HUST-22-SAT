/****程序文件导入****/
#include<iostream>
using namespace std;
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/*******************/

/****程序常量定义****/
#define OK 1
#define ERROR 0
#define HANIDOKU 1//表示选择数独游戏
#define SAT 0//表示选择解决SAT问题
/*******************/

/****程序变量定义****/
typedef int status;
//返回int函数错误参数用数
const int inf = 2147328174;
//分别为cnf文件中bool变元数，子句数，数独解数，选元方式
int Bool_cnt,Clause_cnt,Ans_num,BranchRule;
char fileName[2222],HANIDOKUFileName[2222];//文件名
int MARK[9+1][17+1]={0};//标记数独
int fullBoard[9+1][17+1];
int board[9+1][17+1];
/*******************/

/****数据结构定义****/
typedef struct cNode{
    int data;
    cNode *next;
}cNode;//链表结点定义
typedef struct clause{
    int num;
    cNode *head;
    clause *next;
}clause;//二维链表表头
/*******************/

/****程序函数声明****/
int read(FILE *fp);
int translaterow(int a);
int SelectMaxBool(clause *cnf);
int translatecol(int i,int j);
int SelectNextBool_MOM(clause *cnf);
int SelectMaxBool_verhanidoku(clause *cnf);
int SelectNextBool_Positive_MOM(clause *cnf);
int SelectNextBool_MOM_optimized(clause *cnf);
////////////////////////////////////////////////////////
bool isUnitClause(clause *cl);
bool emptyClause(clause *cnf);
bool DPLL(clause *&cnf, int *v, int satORhanidoku);
////////////////////////////////////////////////////////
void HELLO();
void REFLECT();
void GAMEOVER();
void WIN();void GHOST();
void TEMP(int &a,int &b);
void FLIP2();void FLIP3();
void FLIP4();void FLIP5();
void FLIP6();void FLIP1();
void CREATE();void ROTATE();
void printHANIDOKU(int a[][18]);
void destroyClause(clause *&cnf);
void createFullBoard(int a[][18]);
void removeNode(clause *cl, cNode *&nd);
void dig(int a[][18],int b[][18],int holes);
void removeClause(clause *&cnf, clause *&cl);
void getForgery(clause *&forgery, clause *cnf);
void delelteSingleClause(clause *s, clause *&cnf);
void game(int fullBoard[][18], int board[][18], int holes);
void gameStart(int fullBoard[][18], int board[][18], int modes);
////////////////////////////////////////////////////////
status SaveRes(int s, int *v, double t);
status addClause(clause *cl, clause *&cnf);
status ReadCNF(clause *&cnf, int satORhanidoku);
status transhanidokuToCNF(int a[][18], int holes);
status CreateGame(int a[][18], int row, int col);
status checkmark(int test,int m1[10],int m2[10],int m3[10],int x1,int x2,int x3);