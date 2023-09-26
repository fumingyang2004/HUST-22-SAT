#include"part1.h"
#include<iostream>
using namespace std;

void TEMP(int &a,int &b)
{
    int t;
    t=a;
    a=b;
    b=t;
}
int ABS(int a){return a > 0 ? a : -a;}
int read(FILE *fp){
    int flag = 0;
    int x = 0, f = 1;
    char c;
    c = getc(fp);
    while (c < '0' || c > '9'){
        if (c == '-')
            f = -1, c = getc(fp);
        else return inf;}

    while (c >= '0' && c <= '9'){
        x = x * 10 + c - '0';
        c = getc(fp);
        if (c == -1)
            flag = 1;}

    if (!flag && c != ' ' && c != '\n')
        return inf;
    return x * f;
}
status ReadCNF(clause *&cnf, int satORhanidoku){
    FILE *fp;
    char ch;
    cNode *pn;
    clause *pc;
    char check[5] = {' ', 'c', 'n', 'f', ' '};
    if (satORhanidoku == SAT)
        fp = fopen(fileName, "r");
    else
        fp = fopen(HANIDOKUFileName, "r");
    if (!fp)
        return ERROR;
    while ((ch = getc(fp)) == 'c')
        while ((ch = getc(fp)) != '\n')
            continue;//跳过所有注释行
    if (ch != 'p')
        return ERROR;
    for (int i = 0; i < 5; i++)
        if ((ch = getc(fp)) != check[i])
            return ERROR;

    if ((Bool_cnt = read(fp)) == inf || (Clause_cnt = read(fp)) == inf)
        return ERROR;//p后面都是int变量!!

    //构建二维链表
    cnf = (clause *)malloc(sizeof(clause));
    cnf->next = NULL;
    cnf->head = (cNode *)malloc(sizeof(cNode));
    cnf->head->next = NULL;
    cnf->num = 0;
    pc = cnf;
    pn = cnf->head;

    //^ init part ^
    for (int i = 1; i <= Clause_cnt; i++)
    {
        int dat;
        if ((dat = read(fp)) == inf)
            return ERROR;
        while (dat)
        {
            pc->num++;
            pn->data = dat;
            pn->next = (cNode *)malloc(sizeof(cNode));
            if ((dat = read(fp)) == inf)
                return ERROR;
            if (!dat)
                pn->next = NULL;
            pn = pn->next;
        }
        pc->next = (clause *)malloc(sizeof(clause));
        pc->next->num = 0;
        pc->next->head = (cNode *)malloc(sizeof(cNode));
        if (i == Clause_cnt){
            pc->next = NULL;
            break;}
        pc = pc->next;
        pn = pc->head;
    }
    //^ read part ^
    fclose(fp);
    return OK;
}
/*销毁二维链表
函数名称：destroyClause(clause *&cnf)
函数参数说明：cnf表示整个二维链表头。
操作结果：销毁整个二维链表。
设计思想：基本同读取CNF文件，但执行的是释放空间。*/
void destroyClause(clause *&cnf)
{
    clause *ppc, *pc2;
    cNode *pn1, *pn2;
    ppc = cnf;
    while (ppc)
    {
        pn1 = ppc->head;
        while (pn1)
            pn2 = pn1->next, free(pn1), pn1 = pn2;
        pc2 = ppc->next, free(ppc), ppc = pc2;
    }
    cnf = NULL;
}
/*删除子句
函数名称：removeClause(clause *&cnf, clause *&cl)
函数参数说明：cnf表示整个二维链表头，cl表示删除子句头结点。
操作结果：在二维链表中删除cl子句。
设计思想：搜索表头，找到cl并释放其与其子节点。*/
void removeClause(clause *&cnf, clause *&cl)
{
    if (cl == cnf)
        cnf = cnf->next;
    else{
        clause *ppc = cnf;
        while (ppc && ppc->next != cl)
            ppc = ppc->next;
        ppc->next = ppc->next->next;
    }
    cNode *pn1, *pn2;
    for (pn1 = cl->head; pn1;)
        pn2 = pn1->next, free(pn1), pn1 = pn2;
    free(cl);
    cl = NULL;
}
/*删除句元
函数名称：removeNode(clause *cl, cNode *&nd)
函数参数说明：nd表示要删除的句元，cl表示所在的子句。
操作结果：在子句cl中删除nd句元。
设计思想：搜索子句，找到nd句元并释放结点。*/
void removeNode(cNode *&head, cNode *&nd)
{
    cNode *ppn = head;
    if (ppn == nd)
        head = head->next;
    else{
        while (ppn && ppn->next != nd)
            ppn = ppn->next;
        ppn->next = ppn->next->next;}
    free(nd);
    nd = NULL;
}
/*增加子句
函数名称：addClause(clause *cl, clause *&cnf)
函数参数说明：cl表示增添的子句，cnf表示整个二维链表表头。
操作结果：在二维链表中插入cl子句。
设计思想：直接将cnf子句接到cl后，并使cl成为新的二维链表头。*/
status addClause(clause *cl, clause *&cnf)
{
    if (cl){
        cl->next = cnf;
        cnf = cl;
        return OK;}return ERROR;
}
/*判空子句
函数名称：emptyClause(clause *cnf)
函数参数说明：cl表示需要判断的子句的表头。
操作结果：判断cl是否为空并返回判断结果。
设计思想：直接判断cl子句是否有句元结点。*/
bool emptyClause(clause *cnf)
{
    for (clause *p = cnf; p; p = p->next)
        if (!p->head)
            return true;
    return false;
}
/*删除单子句
函数名称：delelteSingleClause(clause *s, clause *&cnf)
函数参数说明：s表示单子句，cnf表示整个二维链表。
操作结果：对二维链表的所有子句中包含单子句句元的，
         若句元真值相同，直接删除该子句，否则删除该子句中
         的单子句句元结点。
设计思想：遍历整个二维链表，符合条件则直接调用相应函数。*/
void delelteSingleClause(clause *s, clause *&cnf)
{
    clause *tmp;
    int n = s->head->data;
    for (clause *ppc = cnf; ppc; ppc = tmp)
    {
        tmp = ppc->next;
        for (cNode *ppn = ppc->head; ppn; ppn = ppn->next)
        {
            if (ppn->data == n)
            {
                removeClause(cnf, ppc);
                break;
            }
            if (ppn->data == -n)
            {
                removeNode(ppc->head, ppn);
                ppc->num--;
                break;
            }
        }
    }
}
/*判断是否为单子句
函数名称：isUnitClause(clause *cl)
函数参数说明：cl表示子句。
操作结果：判断cl是否为单子句并返回判断结果。
设计思想：直接找出子句第一个句元结点后是否还有句元结点。*/
bool isUnitClause(clause *cl)
{

    if (cl->head != NULL && cl->head->next == NULL)
        return true;
    return false;
}
/*复制二维链表
函数名称：getForgery(clause *&forgery, clause *cnf)
函数参数说明：forgery表示复制的二维链表，cnf表示原链表。
操作结果：得到原二维链表的复制品。
设计思想：基本同读取CNF文件，但执行的是复制结点。*/
void getForgery(clause *&forgery, clause *cnf) // A great forgery is just the same.
{
    clause *ppc, *pc;
    cNode *ppn, *pn;
    forgery = (clause *)malloc(sizeof(clause));
    forgery->head = (cNode *)malloc(sizeof(cNode));
    forgery->next = NULL;
    forgery->head->next = NULL;
    forgery->num = 0;
    for (pc = cnf, ppc = forgery; pc != NULL; pc = pc->next, ppc = ppc->next)
    {
        for (pn = pc->head, ppn = ppc->head; pn != NULL; pn = pn->next, ppn = ppn->next)
        {
            ppc->num++;
            ppn->data = pn->data;
            ppn->next = (cNode *)malloc(sizeof(cNode));
            ppn->next->next = NULL;
            if (pn->next == NULL)
                free(ppn->next), ppn->next = NULL;
        }
        ppc->next = (clause *)malloc(sizeof(clause));
        ppc->next->head = (cNode *)malloc(sizeof(cNode));
        ppc->next->next = NULL;
        ppc->next->head->next = NULL;
        ppc->next->num = 0;
        if (pc->next == NULL)
            free(ppc->next->head), free(ppc->next), ppc->next = NULL;
    }
}
/*选元
函数名称：SelectNextBool(clause *&cnf)
函数参数说明：cnf表示整个二维链表头。
操作结果：通过某种方式选择一个较为优越的变元进行下一分支判断。*/
int SelectMaxBool(clause *cnf)
{
    int *cnt = (int *)malloc(sizeof(int) * (Bool_cnt * 2 + 1));
    for (int i = 0; i <= Bool_cnt * 2; i++)
        cnt[i] = 0;
    for (clause *pc = cnf; pc; pc = pc->next)
        for (cNode *pn = pc->head; pn; pn = pn->next)
        {
            if (pn->data > 0)
                cnt[pn->data]++;
            else
                cnt[Bool_cnt - pn->data]++;
        }
    int maxBool, maxTimes = 0;
    for (int i = 1; i <= Bool_cnt; i++)
        if (cnt[i] > maxTimes)
            maxTimes = cnt[i], maxBool = i;
    if (maxTimes == 0)
    {
        for (int i = Bool_cnt + 1; i <= Bool_cnt * 2; i++)
            if (cnt[i] > maxTimes)
                maxTimes = cnt[i], maxBool = i - Bool_cnt;
    }
    free(cnt);
    return maxBool;
}
int SelectMaxBool_verhanidoku(clause *cnf)
{
    int cnt[2222] = {0};
    for (clause *pc = cnf; pc; pc = pc->next)
        for (cNode *pn = pc->head; pn; pn = pn->next)
        {
            if (pn->data > 0)
                cnt[pn->data]++;
            else
                cnt[1000 - pn->data]++;
        }
    int maxBool, maxTimes = 0;
    for (int i = 1; i <= 1000; i++)
        if (cnt[i + 1000] > maxTimes)
            maxTimes = cnt[i], maxBool = -i;
    if (maxTimes == 0)
    {
        for (int i = 1; i <= 1000; i++)
            if (cnt[i] > maxTimes)
                maxTimes = cnt[i], maxBool = i;
    }
    return maxBool;
}
int SelectNextBool_MOMS(clause *cnf)
{
    int minNode = Bool_cnt;
    int *cnt = (int *)malloc((Bool_cnt + 1) * sizeof(int));
    for (int i = 0; i < Bool_cnt; i++)
        cnt[i] = 0;
    for (clause *pc = cnf; pc; pc = pc->next)
        if (pc->num < minNode)
            minNode = pc->num;
    for (clause *pc = cnf; pc; pc = pc->next)
        if (pc->num == minNode)
            for (cNode *pn = pc->head; pn; pn = pn->next)
                cnt[ABS(pn->data)]++;
    int maxAppear = 0, maxBool;
    for (int i = 1; i <= Bool_cnt; i++)
        if (cnt[i] > maxAppear)
            maxAppear = cnt[i], maxBool = i;
    free(cnt);
    return maxBool;
}
double J(int n){return pow(2.0, (double)(-n));}
int SelectNextBool_MOM(clause *cnf)
{
    double *weight = (double *)malloc(sizeof(double) * (Bool_cnt + 1));
    for (int i = 0; i <= Bool_cnt; i++)
        weight[i] = 0.0;
    for (clause *pc = cnf; pc; pc = pc->next)
        for (cNode *pn = pc->head; pn; pn = pn->next)
            weight[ABS(pn->data)] += J(pc->num);
    double maxw = 0;
    int maxBool;
    for (int i = 1; i <= Bool_cnt; i++)
        if (weight[i] > maxw)
            maxw = weight[i], maxBool = i;
    free(weight);
    return maxBool;
}
int SelectNextBool_MOM_optimized(clause *cnf)
{
    double *weight = (double *)malloc(sizeof(double) * (Bool_cnt * 2 + 1));
    for (int i = 0; i <= Bool_cnt * 2; i++)
        weight[i] = 0.0;
    for (clause *pc = cnf; pc; pc = pc->next)
        for (cNode *pn = pc->head; pn; pn = pn->next)
        {
            if (pn->data > 0)
                weight[pn->data] += J(pc->num);
            else
                weight[Bool_cnt - pn->data] += J(pc->num);
        }
    double maxw = 0.0;
    int maxBool;
    for (int i = 1; i <= Bool_cnt; i++)
        if (weight[i] + weight[i + Bool_cnt] > maxw)
        {
            maxw = weight[i] + weight[i + Bool_cnt], maxBool = i;
        }
    if (weight[maxBool] < weight[maxBool + Bool_cnt])
        maxBool = -maxBool;
    free(weight);
    return maxBool;
}
int SelectNextBool_Positive_MOM(clause *cnf)
{
    int *mark = (int *)malloc(sizeof(int) * (Bool_cnt + 1));
    double *weight = (double *)malloc(sizeof(double) * (Bool_cnt * 2 + 1));
    for (int i = 0; i <= Bool_cnt; i++)
        mark[i] = 0;
    for (int i = 0; i <= Bool_cnt * 2; i++)
        weight[i] = 0.0;
    for (clause *pc = cnf; pc; pc = pc->next)
        for (cNode *pn = pc->head; pn; pn = pn->next)
        {
            if (pn->data < 0)
                break;
            else if (pn->next == NULL)
            {
                for (cNode *tmp = pc->head; tmp; tmp = tmp->next)
                    mark[tmp->data] = 1;
                break;
            }
        }
    for (clause *pc = cnf; pc; pc = pc->next)
        for (cNode *pn = pc->head; pn; pn = pn->next)
        {
            if (mark[ABS(pn->data)])
            {
                if (pn->data > 0)
                    weight[pn->data] += J(pc->num);
                else
                    weight[Bool_cnt - pn->data] += J(pc->num);
            }
        }
    free(mark);
    double maxw = 0.0;
    int maxBool = 0;
    for (int i = 1; i <= Bool_cnt; i++)
        if (weight[i] + weight[i + Bool_cnt] > maxw)
        {
            maxw = weight[i] + weight[i + Bool_cnt], maxBool = i;
        }
    if (weight[maxBool] < weight[maxBool + Bool_cnt])
        maxBool = -maxBool;
    free(weight);
    if (maxBool == 0)
        maxBool = SelectNextBool_MOM_optimized(cnf);
    return maxBool;
}
/*DPLL
函数名称：DPLL(clause *&cnf, int *v, int satORhanidoku)
函数参数说明：cnf表示整个二维链表头，v是储存变元真值的数组，satORhanidoku用于判断当前子系统。
操作结果：解CNF文件并将结果保存到v中。*/
bool DPLL(clause *&cnf, int *v, int satORhanidoku)
{
    int flag = 1;
    clause *pc;
    while (flag)
    {
        flag = 0;
        pc = cnf;
        while (pc && !isUnitClause(pc))
            pc = pc->next;
        if (pc != NULL)
        {
            if (pc->head->data > 0)
                v[pc->head->data] = 1;
            else
                v[-pc->head->data] = 0;
            delelteSingleClause(pc, cnf);
            if (cnf == NULL)
                return true;
            else if (emptyClause(cnf))
                return false;
            flag = 1;
        }
    }
    //^ delete single clause ^
    int maxBool;
    if (satORhanidoku == HANIDOKU)
        maxBool = SelectMaxBool_verhanidoku(cnf);
    else
    {
        if (BranchRule == 1)
            maxBool = SelectMaxBool(cnf);//SelectMaxBool_verhanidoku(cnf);
        //MO
        else if (BranchRule == 2)
            maxBool = SelectMaxBool_verhanidoku(cnf);//SelectNextBool_MOMS(cnf);
        //MPO
        else if (BranchRule == 3)
            maxBool = SelectNextBool_MOM(cnf);//SelectNextBool_MOM(cnf);
        //MOM
        else if (BranchRule == 4)
            maxBool = SelectNextBool_Positive_MOM(cnf);//SelectNextBool_MOM_optimized(cnf);
        // pos MOM
        //else if (BranchRule == 5)
            //maxBool = SelectNextBool_Positive_MOM(cnf);
        else maxBool = SelectMaxBool(cnf);
        
    }
    //printf("**%d**\n",maxBool);
    //^ pick the best bool. ^
    clause *newSingleClause = (clause *)malloc(sizeof(clause)), *forgery;
    newSingleClause->head = (cNode *)malloc(sizeof(cNode));
    newSingleClause->next = NULL;
    newSingleClause->head->data = maxBool;
    newSingleClause->head->next = NULL;
    newSingleClause->num = 1;
    getForgery(forgery, cnf);
    addClause(newSingleClause, forgery);
    if (DPLL(forgery, v, satORhanidoku) == true)
        return true; // The first Branch in which the new Single Clause is true;
    destroyClause(forgery);
    newSingleClause = (clause *)malloc(sizeof(clause));
    newSingleClause->head = (cNode *)malloc(sizeof(cNode));
    newSingleClause->next = NULL;
    newSingleClause->head->data = -maxBool;
    newSingleClause->head->next = NULL;
    newSingleClause->num = 1;
    addClause(newSingleClause, cnf);
    bool ans = DPLL(cnf, v, satORhanidoku); // The second Branch in which the new Single Clause is false.
    destroyClause(cnf);
    return ans;
}
/*保存res文件
函数名称：SaveRes(int s, int *v, double t)
函数参数说明：s是解cnf的结果(是否可满足)，v是记录变元最终结果的数组，t记录运行时间。
操作结果：根据要求保存解cnf最后的结果。*/
status SaveRes(int s, int *v, double t)
{
    FILE *fp;
    for (int i = 0; fileName[i] != '\0'; i++)
        if (fileName[i] == '.' && fileName[i + 4] == '\0')
        {
            fileName[i + 1] = 'r', fileName[i + 2] = 'e', fileName[i + 3] = 's';
            break;
        }
    if (!(fp = fopen(fileName, "w")))
    {
        printf("File Creating ERROR!\n");
        return ERROR;
    }
    fprintf(fp, "s %d\nv ", s);
    if (s)
        for (int i = 1; i <= Bool_cnt; i++)
        {
            if (v[i])
                fprintf(fp, "%d ", i);
            else
                fprintf(fp, "%d ", -i);
        }
    fprintf(fp, "\nt %lf", t * 1000);
    fclose(fp);
    return OK;
}

///////////////////////////数独游戏/////////////////////////////
void gameStart(int fullBoard[][18], int board[][18], int modes){
    //createFullBoard(fullBoard);
    CREATE();
    dig(fullBoard, board, modes);
   // printHANIDOKU(fullBoard);
    printHANIDOKU(board);
}
void CREATE()
{
    int a1[10][18]=
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,5,0,4,0,7,0,3,0,6,0,0,0,0,
     0,0,0,0,6,0,3,0,5,0,8,0,7,0,4,0,0,0,
     0,0,0,4,0,8,0,6,0,7,0,9,0,5,0,3,0,0,
     0,0,7,0,6,0,2,0,9,0,8,0,3,0,4,0,5,0,
     0,3,0,5,0,1,0,4,0,2,0,9,0,6,0,8,0,7,
     0,0,7,0,3,0,5,0,1,0,8,0,2,0,4,0,6,0,
     0,0,0,4,0,2,0,3,0,7,0,1,0,6,0,5,0,0,
     0,0,0,0,6,0,4,0,2,0,5,0,7,0,3,0,0,0,
     0,0,0,0,0,5,0,6,0,7,0,3,0,4,0,0,0,0};
    int a2[10][18]=
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,6,0,4,0,5,0,3,0,7,0,0,0,0,
     0,0,0,0,5,0,9,0,7,0,6,0,4,0,8,0,0,0,
     0,0,0,4,0,7,0,3,0,8,0,9,0,5,0,6,0,0,
     0,0,3,0,8,0,6,0,2,0,1,0,4,0,7,0,5,0,
     0,7,0,6,0,9,0,1,0,5,0,3,0,8,0,2,0,4,
     0,0,5,0,4,0,7,0,8,0,1,0,2,0,3,0,6,0,
     0,0,0,8,0,5,0,3,0,2,0,4,0,6,0,7,0,0,
     0,0,0,0,4,0,2,0,6,0,3,0,7,0,5,0,0,0,
     0,0,0,0,0,6,0,7,0,5,0,4,0,8,0,0,0,0};

     int arr[21];
     srand(time(0));
     arr[0]=rand()%2;
     
     if(arr[0]==0)
     for(int i=1;i<=9;i++)
     for(int j=1;j<=17;j++)
     fullBoard[i][j]=a1[i][j];

     if(arr[0]==1)
     for(int i=1;i<=9;i++)
     for(int j=1;j<=17;j++)
     fullBoard[i][j]=a2[i][j];    

     for(int i=1;i<=150;i++)
     {
        arr[i]=1+rand()%8;//取12345678
        switch (arr[i])
        {
        case 1:
        FLIP1();break;
        case 2:
        FLIP2();break;       
        case 3:
        FLIP4();break;
        case 4:
        FLIP4();break;
        case 5:
        FLIP5();break;
        case 6:
        FLIP6();break;
        case 7:
        ROTATE();break;
        case 8:
        REFLECT();break;
        }
     }

}
void FLIP1()
{
    for(int i=1;i<=9;i++)
    {
        for(int j=1;j<=8;j++)
        {
            int t;
            t=fullBoard[i][j];
            fullBoard[i][j]=fullBoard[i][18-j];
            fullBoard[i][18-j]=t;
        }
    }
}
void FLIP2()
{
    for(int i=1;i<=5;i++)
    {//i+4,i -> 1,3+2i
        for(int j=1;j<=(i+4)/2;j++)
        {
            TEMP(fullBoard[i+4+1-j][i+j-1],fullBoard[j][3+2*i+1-j]);
        }
    }
    for(int i=1;i<=4;i++)
    {//9,5+2i -> i+1,i+13
        for(int j=1;j<=(9-i)/2;j++)
        {
            TEMP(fullBoard[9+1-j][j-1+5+2*i],fullBoard[i+1+j-1][i+13+1-j]);
        }
    }
}
void FLIP3()
{
    FLIP1();FLIP2();FLIP1();
}
void FLIP4()
{
    for(int i=1;i<=17;i++)
    for(int j=1;j<=4;j++)
    TEMP(fullBoard[j][i],fullBoard[10-j][i]);
}
void FLIP5()
{
    for(int i=1;i<=5;i++)
    {//i+4,i -> 9,15-2i
        for(int j=1;j<=(6-i)/2;j++)
        {
            TEMP(fullBoard[i+4+j-1][i+3*(j-1)],fullBoard[9-j+1][15-2*i-3*(j-1)]);
        }
    }
    for(int i=1;i<=4;i++)
    {//5-i,i+1 -> 9-i,13+i
        for(int j=1;j<=2;j++)
        {
            TEMP(fullBoard[5-i+j-1][i+1+3*(j-1)],fullBoard[9-i-j+1][13+i-3*(j-1)]);
        }
    }
    for(int i=1;i<=4;i++)
    {//6-i,i+2 -> 9-i,11+i
        for(int j=1;j<=2;j++)
        {
            TEMP(fullBoard[6-i+j-1][i+2+3*(j-1)],fullBoard[9-i-j+1][11+i-3*(j-1)]);
        }
    }
    for(int i=1;i<=4;i++)
    {//1,2i+5 -> 5-i,17-i
        for(int j=1;j<=(5-i)/2;j++)
        {
            TEMP(fullBoard[1+j-1][2*i+5+3*(j-1)],fullBoard[5-i-j+1][17-i-3*(j-1)]);
        }
    }
}
void FLIP6()
{
    FLIP4();
    FLIP5();
    FLIP4();
}
void ROTATE()
{
    int arr[10][18]={0};
    for(int i=1;i<=9;i++)
     for(int j=1;j<=17;j++)
     arr[i][j]=fullBoard[i][j];
    
    for(int i=1;i<=5;i++)
    {//4+i,i -> i,6-i
        for(int j=1;j<=4+i;j++)
        {
            fullBoard[i][6-i+2*(j-1)]=arr[4+i-j+1][i+j-1];
        }
    }
    for(int i=1;i<=4;i++)
    {//9,2i+5 -> 5+i,1+i
        for(int j=1;j<=9-i;j++)
        {
            fullBoard[5+i][1+i+2*(j-1)]=arr[9-j+1][2*i+5+j-1];
        }

    }
}
void REFLECT()
{
    for(int i=1;i<=9;i++)
    for(int j=1;j<=17;j++)
    if(1<=fullBoard[i][j]&&fullBoard[i][j]<=9)
    fullBoard[i][j]=10-fullBoard[i][j];
}
void createFullBoard(int a[][18]){
    srand(time(0));
    //第一行要连续!
    int x=rand()%5+5;//x=5,6,7,8,9,取值范围x-4 ≤ a ≤ x.
    for(int i=5;i<=13;i=i+2){ 
        a[1][i]=rand() % 9 + 1;
        while(a[1][i]>x||a[1][i]<x-4)
        {
            a[1][i]=rand() % 9 + 1;
        }
        int j=5;
        while(j<i){
            if(a[1][i]==a[1][j]&&a[1][i]!=0)
             {a[1][i] = rand() % 9 + 1, j = 5;
             while(a[1][i]>x||a[1][i]<x-4)
        {
            a[1][i]=rand() % 9 + 1;
        }}
            else j=j+2;
        }
    }//随机生成第一行的数字.
    CreateGame(a, 2, 4);
}
status checkmark(int test,int m1[10],int m2[10],int m3[10],int x1,int x2,int x3)
{
    int max=9,min=1;
    int MAX[4]={0,9,9,9};
    int MIN[4]={0,1,1,1};
for(int i=1;i<=3;i++)
{
    int x;
    int *p=NULL;
    switch (i)
    {
    case 1:
    x=x1;p=m1;
    break;
    case 2:
    x=x2;p=m2;
    break;
    case 3:
    x=x3;p=m3;
    }
    switch (x)
    {
    case 5:{
        for(int j=1;j<=9;j++)
        {
            if(p[j]==0)continue;
            int t=j+4;
            if(t>9)t=9;
            if(t<MAX[i])MAX[i]=t;
            
            int tt=j-4;
            if(tt<1)tt=1;
            if(tt>MIN[i])MIN[i]=tt;
        }break;
    }
    case 6:{
        for(int j=1;j<=9;j++)
        {
            if(p[j]==0)continue;
            int t=j+5;
            if(t>9)t=9;
            if(t<MAX[i])MAX[i]=t;
            
            int tt=j-5;
            if(tt<1)tt=1;
            if(tt>MIN[i])MIN[i]=tt;
        }break;
    }
    case 7:{
        for(int j=1;j<=9;j++)
        {
            if(p[j]==0)continue;
            int t=j+6;
            if(t>9)t=9;
            if(t<MAX[i])MAX[i]=t;
            
            int tt=j-6;
            if(tt<1)tt=1;
            if(tt>MIN[i])MIN[i]=tt;
        }break;
    }
    case 8:{
        for(int j=1;j<=9;j++)
        {
            if(p[j]==0)continue;
            int t=j+7;
            if(t>9)t=9;
            if(t<MAX[i])MAX[i]=t;
            
            int tt=j-7;
            if(tt<1)tt=1;
            if(tt>MIN[i])MIN[i]=tt;
        }break;
    }
    case 9:{
        MAX[i]=9;
        MIN[i]=1;
        break;
    }
    }
}
max=MAX[1]>MAX[2]?MAX[2]:MAX[1];
int e=MAX[3]>max?max:MAX[3];

min=MIN[1]>MIN[2]?MIN[1]:MIN[2];
int ee=MIN[3]>min?MAX[3]:min;

if(test>=ee&&test<=e)return 1;
else return 0;

}
status CreateGame(int a[][18], int row, int col){
    if(row<=9&& ( col>=1+ABS(5-row) && col<=17-ABS(5-row) )&&row%2==col%2 )//行列取值范围,只有行列合法才可以进去
    {
        int mark[10]={0};
        int flag=1;
        int x1,x2=1,x3=1;
        //x1:直接取这一行全部的有效元素个数.
        x1=9-ABS(5-row);
        //x2:每次变换后,row-i,col+i与row-i,col+i
        for(int i=1; row-i>=1 && (col+i>=1+ABS(5-row+i)&&col+i<=17-ABS(5-row+i)) ;i++)x2++;
        for(int i=1; row+i<=9 && (col-i>=1+ABS(5-row-i)&&col-i<=17-ABS(5-row-i)) ;i++)x2++;
        //x3:每次变换后,row-i,col-i与row+i,col+i
        for(int i=1; row-i>=1 && (col-i>=1+ABS(5-row+i)&&col-i<=17-ABS(5-row+i)) ;i++)x3++;
        for(int i=1; row+i<=9 && (col+i>=1+ABS(5-row-i)&&col+i<=17-ABS(5-row-i)) ;i++)x3++;

        int mark1[10]={0},mark2[10]={0},mark3[10]={0};
        for(int i=1+ABS(5-row);i<=col;i++) {mark[a[row][i]]=1;mark1[a[row][i]]=1;}//对行做标记
        for(int i=0;i<=row-1;i++) {mark[a[row-i][col+i]]=1;mark2[a[row-i][col+i]]=1;}//对↗做标记
        for(int i=0;i<=row-1;i++) {mark[a[row-i][col-i]]=1;mark3[a[row-i][col-i]]=1;}//对↖做标记
        
        for(int i=1;i<=9&&flag;i++)
        {
            if((!mark[i])&&checkmark(i,mark1,mark2,mark3,x1,x2,x3))//这个数字没被用过，并且符合check
            {
                flag=0;
                a[row][col]=i;
                if(col==17-ABS(5-row)&&row!=9)
                {
                    if(CreateGame(a,row+1,1+ABS(4-row))==OK)
                    return OK;
                    else flag=1;
                }
                else if(col!=17-ABS(5-row))
                {
                    if (CreateGame(a, row, col + 2) == OK)
                    return OK;
                    else flag = 1;                    
                }
            }
            
        }
        if (flag)
        {
            a[row][col] = 0; // Proper Number Not Found -> delete and travel back.
            return ERROR;
        }
    }
    //printhanidoku(a);
    return OK;
}
/*void printHANIDOKU(int a[][18])
{
    printf("              _   _   _   _   _\n");
    for(int i=1;i<=19;i++)
    {
        int x=ABS(10-i)+4;
        if(i%2==1&&i<=9)
        {
            for(int j=1;j<=x-1;j++)printf(" ");
            printf("_");
        }
        else
        {
            for(int j=1;j<=x;j++)printf(" ");
        }
        if(i%2==1&&i<=9)
        {
            int n1=19-ABS(10-i);
            int n2=n1/2;
            for(int j=1;j<=n2;j++)printf("/ \\\033[4;3;1m \033[0;3;0m");
            printf("\n");
            continue;
        }
        if(i%2==1&&i>9)
        {
            int n1=19-ABS(10-i);
            int n2=n1/2;
            for(int j=1;j<=n2;j++)printf("\\_/ ");
            printf("\n");
            continue;
        }
        if(i%2==0)
        {
            int n1=19-ABS(10-i);
            int t=i/2;
            int k=1+ABS(5-t);
            for(int j=1;j<=n1;j++)
            {
                if(j%2==1&&j<n1)printf("\033[4;3;1m|\033[0;3;0m");
                if(j==n1){printf("\033[4;3;1m|\033[0;3;0m");
                printf("　");}
                if(j%2==0)
                {
                    if(MARK[i/2][k]==1)
                    {
                    if(a[i/2][k]==0) printf("\033[1;32;1m . \033[0;3;0m");
                    else printf("\033[1;32;1m %d \033[0;3;0m",a[i/2][k]);
                    }
                    if(MARK[i/2][k]==0)
                    {
                    if(a[i/2][k]==0) printf(" . ");
                    else printf(" %d ",a[i/2][k]);
                    }
                    k=k+2;
                }
            }
            printf("\n");
            continue;
        }
    }
}*/
void printHANIDOKU(int a[][18])
{
    printf("              _   _   _   _   _\n");
    for(int i=1;i<=19;i++)
    {
        int x=ABS(10-i)+4;
        if(i%2==1&&i<=9)
        {
            for(int j=1;j<=x-1;j++)printf(" ");
            printf("_");
        }
        else
        {
            for(int j=1;j<=x;j++)printf(" ");
        }
        if(i%2==1&&i<=9)
        {
            int n1=19-ABS(10-i);
            int n2=n1/2;
            for(int j=1;j<=n2;j++)printf("/ \\\033[4;3;1m \033[0;3;0m");
            printf("\n");
            continue;
        }
        if(i%2==1&&i>9)
        {
            int n1=19-ABS(10-i);
            int n2=n1/2;
            for(int j=1;j<=n2;j++)printf("\\_/ ");
            printf("\n");
            continue;
        }
        if(i%2==0)
        {
            int n1=19-ABS(10-i);
            int t=i/2;
            int k=1+ABS(5-t);
            for(int j=1;j<=n1;j++)
            {
                if(j%2==1&&j<n1)printf("\033[4;3;1m|\033[0;3;0m");
                if(j==n1){printf("\033[0;3;0m|\033[0;3;0m");
                printf("　");}
                if(j%2==0)
                {
                    if(MARK[i/2][k]==1)
                    {
                    if(a[i/2][k]==0) printf("\033[1;32;1m . \033[0;3;0m");
                    else printf("\033[1;32;1m %d \033[0;3;0m",a[i/2][k]);
                    }
                    if(MARK[i/2][k]==0)
                    {
                    if(a[i/2][k]==0) printf(" . ");
                    else printf(" %d ",a[i/2][k]);
                    }
                    k=k+2;
                }
            }
            printf("\n");
            continue;
        }
    }
}
void dig(int a[][18],int b[][18],int holes)
{
    srand(time(0));
    for(int i=1;i<=9;i++)
        for(int j=1;j<=18;j++)
            b[i][j]=a[i][j];//复制数据
    int i=1;
    while(i<=holes)
    {
        int m=rand()%9+1;
        int n=rand()%17+1;
        while(!(n>=1+ABS(5-m)&&n<=17-ABS(5-m)&&(m%2==n%2)))
            n=rand()%17+1;
        if(b[m][n]!=0)
        {
            b[m][n]=0;
            MARK[m][n]=1;
            i++;
            continue;
        }
        if(b[m][n]==0)
            continue;
    }
}
void game(int fullBoard[][18], int board[][18], int holes)
{
    int x=holes;
    int M=-1,N=-1;
    int order=-1;
    double start=clock();
    while(1){
    system("cls");//清屏
    printf("●------------------*MENU*------------------●\n");
    printf("        You are in the \"PLAY HANIDOKU\"!\n\n");
    printf("    Here are some tips to help you complete.\n");
    printf(" ============================================\n");
    printf(" ◆YOU CAN ONLY PUT YOUR NUMBER ON THE GREEN.\n");
    printf(" ◆ENTER (M,N,E) TO PUT (E) IN ROW(M),COL(N).\n");
    printf(" ◆ENTER (11,M,N) TO GET OUR HELP!\n");
    printf(" ◆ENTER 10 TO CHECK IF YOU ARE RIGHT.\n");
    printf(" ◆ENTER 0 TO END THE GAME.\033[1;31;1m(HAHA,YOU~LOST!!)\033[0;3;0m\n");
    printf(" ============================================\n");
    printf(" \033[1;3;1m★HOLES LEFT:\033[1;31;1m%d\033[0;3;1m              ★",holes);
    switch (x)
    {
    case 7:
        printf("MODE:\033[1;32;1mEASY\033[0;3;0m\n");
        break;
    case 14:
        printf("MODE:\033[1;36;1mNORMAL\033[0;3;0m\n");
        break;
    case 21:
        printf("MODE:\033[1;34;1mHARD\033[0;3;0m\n");
        break;
    case 28:
        printf("MODE:\033[1;33;1mEXPERT\033[0;3;0m\n");
        break;
    case 35:
        printf("MODE:\033[1;35;1mCRAZY\033[0;3;0m\n");
        break;    
    case 42:
        printf("MODE:\033[1;31;1mINSANE\033[0;3;0m\n");
        break;    
    case 49:
        printf("MODE:\033[0;31;1mHELL\033[0;3;0m\n");
        break;        
    default:
        printf("MODE:\033[1;30;1mCUSTOM\033[0;3;0m\n");
        break;
    }
    printHANIDOKU(board);
    printf("            ◆-----------------◆\n");
        if(holes==0)
    {
        printf(" ◇PAY ATTENTION: YOU HAD COMPLETED.\n");
        printf(" ◇YOU CAN ENTER \033[1;32;1m12\033[0;3;0m TO CHECK YOUR ANSWER.\n");
    }
        printf(" ◇YOU WANT:");
    scanf("%d",&order);
        if(order>=1&&order<=9)
        {
            int E=0;
            M=order;
            scanf("%d %d",&N,&E);
            if (N>9-ABS(5-M)||N<=0||MARK[M][ABS(5-M)+2*N-1]==0||E<=0||E>9)
            {
                printf(" PLEASE BE CAREFUL!!!!!\n");
                printf(" ============================================\n");
                printf("         Press any key to continue...\n");
                        printf("●------------------*MENU*------------------●\n");
                        getchar(),getchar();//按任意键以继续
                continue;
            }
            if (MARK[M][ABS(5-M)+2*N-1]==1&&board[M][ABS(5-M)+2*N-1]==0)
            {holes--;}
            board[M][ABS(5-M)+2*N-1]=E;
            printf(" \033[1;32;1mPUT SUCCESSFULLY!!\033[0;3;0m\n");
            printf(" ============================================\n");
            printf("         Press any key to continue...\n");
                        printf("●------------------*MENU*------------------●\n");
                        getchar(),getchar();//按任意键以继续
            continue;
        }
        switch (order)
        {
            case 0:{
                for(int i=1;i<=9;i++)
                {
                    for(int j=1;j<=17;j++)
                    {
                        if(MARK[i][j]==1)
                        board[i][j]=0;
                    }
                } 
                system("cls");GAMEOVER();getchar();getchar();
                return;
            }//case 0
            case 114514:{
                holes=0;
                for(int i=1;i<=9;i++)
                for(int j=1;j<=17;j++)
                board[i][j]=fullBoard[i][j];
                printf("\033[1;31;1m WHAT♂ARE♂YOU♂DOING??\033[0;3;0m\n");
                printf(" ============================================\n");
                printf("         Press any key to continue...\n");
                        printf("●------------------*MENU*------------------●\n");
                        getchar(),getchar();//按任意键以继续
                break;
            }//case 114514
            case 12:{
                if(holes!=0)break;
                int judge=1;
                for(int i=1;i<=9;i++)
                for(int j=1;j<=17;j++)
                if(board[i][j]!=fullBoard[i][j])judge=0;
                if(judge==0)
                {
                    printf("\033[1;31;1m WRONG ANSWER~~HAHA!\033[0;3;0m\n");
                    printf(" ============================================\n");
                    printf("         Press any key to continue...\n");
                        printf("●------------------*MENU*------------------●\n");
                        getchar(),getchar();//按任意键以继续
                    break;
                }
                else
                {
                   printf("\033[1;32;1m RIGHT ANSWER!!!NO!!YOU WIN!!!\033[0;3;0m\n");
                   double finish=clock();
                   printf(" YOU CONSUMED:\033[1;32;1m %.3fs\033[0;3;0m.\n",(finish-start)/1000);
                    printf(" ============================================\n");
                    printf("         Press any key to continue...\n");
                        printf("●------------------*MENU*------------------●\n");
                        getchar(),getchar();//按任意键以继续
                    system("cls");//清屏
                    WIN();
                    for(int i=1;i<=9;i++)
                    for(int j=1;j<=17;j++)
                        if(MARK[i][j]==1)
                        board[i][j]=0;
                    getchar(),getchar();//按任意键以继续
                    return;
                    break;
                }
            }//case 12
            case 11:{
                scanf("%d %d",&M,&N);
                if(M<=0||N<=0||N>9-ABS(5-M)||M>9)
                {
                    printf(" PLEASE BE CAREFUL!!!!!\n");
                    printf(" ============================================\n");
                    printf("         Press any key to continue...\n");
                        printf("●------------------*MENU*------------------●\n");
                        getchar(),getchar();//按任意键以继续
                    break;
                }
                int x=fullBoard[M][ABS(5-M)+2*N-1];
                printf(" THE NUMBER IN \033[1;32;1m(%d,%d)\033[0;3;0m IS \033[1;32;1m%d.\033[0;3;0m\n",M,N,x);
                printf(" ============================================\n");
                printf("         Press any key to continue...\n");
                        printf("●------------------*MENU*------------------●\n");
                        getchar(),getchar();//按任意键以继续
                break;
            }//case 11
            case 10:{
                printf(" WHICH ANSWER YOU PUT IN (M,N) YOU WANT CHECK?\n");
                printf(" PLEASE ENTER (M,N):");
                scanf("%d %d",&M,&N);
                if(M<=0||N<=0||N>9-ABS(5-M)||M>9)
                {
                    printf(" YOU CAN'T PUT IT IN WALLS!!\n");
                    printf(" ============================================\n");
                    printf("         Press any key to continue...\n");
                        printf("●------------------*MENU*------------------●\n");
                        getchar(),getchar();//按任意键以继续
                    break;
                }
                else{
                    int x=board[M][ABS(5-M)+2*N-1];
                    if(x==fullBoard[M][ABS(5-M)+2*N-1])
                    {
                        printf("\033[1;32;1m RIGHT ANSWER!!!\033[0;3;0m\n");
                        printf(" ============================================\n");
                        printf("         Press any key to continue...\n");
                        printf("●------------------*MENU*------------------●\n");
                        getchar(),getchar();//按任意键以继续
                        break;
                    }
                    else
                    {
                        printf("\033[1;31;1m WRONG ANSWER~~HAHA!\033[0;3;0m\n");
                        printf(" ============================================\n");
                        printf("         Press any key to continue...\n");
                        printf("●------------------*MENU*------------------●\n");
                        getchar(),getchar();//按任意键以继续
                        break;
                    }
                }
            }//case 10
        }
    }

    printf("         Press any key to continue...\n");
                        printf("●------------------*MENU*------------------●\n");
                        getchar(),getchar();//按任意键以继续
}
void WIN()
{
printf("\033[1;32;1m\n\n\n\n\n\n\n");
printf(" ●===============================================================================================================●\n\n");
printf("   01010       01010  0101010101    01010   01010          10101         10101  10101010  10101      10101   10101\n");
printf("    01010     01010  010101010101   01010   01010          10101         10101   010101   101010     10101   10101\n");
printf("     01010   01010   01010  01010   01010   01010          10101         10101   010101   1010101    10101   10101\n");
printf("      01010 01010    01010  01010   01010   01010          10101         10101   010101   10101010   10101   10101\n");
printf("       010101010     01010  01010   01010   01010          10101  10101  10101   010101   101010101  10101   10101\n");
printf("        0101010      01010  01010   01010   01010          10101 1010101 10101   010101   1010101010 10101   10101\n");
printf("         01010       01010  01010   01010   01010          1010110101010101011   010101   10101 1010110101   10101\n");
printf("         01010       01010  01010   01010   01010          101010101 101010101   010101   10101  101010101   10101\n");
printf("         01010       01010  01010   01010   01010          10101010   10101011   010101   10101   10101011  \n");
printf("         01010       01010  01010   0101010101010          1010101     1010101   010101   10101    1010101   10101\n");
printf("         01010       010101010101   0101010101010          101010       101011   010101   10101     101011   10101\n");
printf("         01010        0101010101     01010101010           10101         10101  01010101  10101      10101   10101\n");
printf("\n ●===============================================================================================================●\n");
printf("\033[0;3;0m");
}
void GHOST()
{
   // printf("\033[1;31;5m");
    printf("                   01010101\n");
    printf("                01010101010101\n");
    printf("              010101010101010101\n");
    printf("             01010101010101010101\n");
    printf("             01010101010101010101\n");
    printf("             010     0101     010\n");
    printf("             01     010101     01\n");
    printf("             010   01010101   101\n");
    printf("              01010101  01010101\n");
    printf("               0101010101010101\n");
    printf("               0 01 010101 01 0\n");
    printf("                01 01 01 01 01\n");
    printf("                 01        01\n");
    printf("                  1010  1010\n");
    printf("                   01010101 \n\n");
   // printf("\033[0;3;5m");
}
//
status transhanidokuToCNF(int a[][18],int holes)
{//自3开始到5823 ---> 最多有5821个子句,实际上有5821-holes个
    FILE *fp;
    if (!(fp = fopen(HANIDOKUFileName, "w")))
    {
        fprintf(fp, "File creating error!\n");
        return ERROR;
    }
    fprintf(fp, "c A hanidoku cnf file.  Holes count:%d\n", holes);
    char s[5];//开辟四位数长度空间
    int num=5821-9*holes;
    for(int i=0;i<4;i++)
    {
        int x=num%10;
        num=num/10;
        s[3-i]=x+48;
    }s[4]='\0';
        fprintf(fp, "p cnf 959 ");//全部是要用字符写.
    for(int i=0;i<4;i++)
    fprintf(fp,"%c",s[i]);
    fprintf(fp,"\n");
    ////////////////////////////////////////////////////
    for(int i=1;i<=9;i++)//已结束
    {//读取提示数字
        for(int j=1+ABS(5-i);j>=1+ABS(5-i)&&j<=17-ABS(5-i);j=j+2)//行列数字符合取值范围
        {
            int row=i;//行是一样的.
            int col=(j+1-ABS(5-row))/2;
            if(board[i][j]!=0)
            {
                fprintf(fp,"%c%c%c 0\n",row+48,col+48,board[i][j]+48);
                for(int k=1;k<=9;k++)
                {
                    if(k!=board[i][j])fprintf(fp,"-%c%c%c 0\n",row+48,col+48,k+48);
                }

            }
            
        }
    }
    /**/
    for(int i=1;i<=9;i++)
    {
        for(int j=1+ABS(5-i);j<=17-ABS(5-i);j+=2)
        {
            for(int k=1;k<=9;k++)
            {
                fprintf(fp,"%c%c%c ",i+48,translatecol(i,j)+48,k+48);
            }fprintf(fp,"0\n");
        }
    }
    //读取行连续性约束
    for(int i=1;i<=9;i++)
    {//代表数字1~9
        for(int j=1;j<=9;j++)
        {//代表行1~9
            int col=9-ABS(5-j);//一行有几个数字
            for(int k=1;k<=col-1;k++)//连续性约束的第一个数字
            {
                for(int l=k+1;l<=col;l++)
                {
                    fprintf(fp,"-%c%c%c -%c%c%c 0\n",j+48,k+48,i+48,j+48,l+48,i+48);
                }
            }
        }
    }
    //读取↗行连续性约束
    for(int i=1;i<=9;i++)
    {//代表数字1~9
        for(int j=5;j<=13;j=j+2)//列的下标
        {
            int x=translatecol(1,j)+4;//对应12345+4,每列有几个数字
            for(int k=1;k<=x-1;k++)//每列大循环次数是x-1
            {
                for(int l=1;l<=x-k;l++)//大循环的小循环次数是x-k
                {
                    //k,j+1-k -> k+l,j+1-k-l
                    int r1=k,r2=k+l;
                    int c1=translatecol(r1,j+1-k);
                    int c2=translatecol(r2,j+1-k-l);
                    fprintf(fp,"-%c%c%c -%c%c%c 0\n",r1+48,c1+48,i+48,r2+48,c2+48,i+48);
                }
            }
        }
        for(int j=7;j<=13;j=j+2)//另一部分的列下标
        {
            int x=10-translatecol(9,j);//对应10-2345,每列有几个数字
            for(int k=1;k<=x-1;k++)//每列大循环次数是x-1
            {
                for(int l=1;l<=x-k;l++)//大循环的小循环次数是x-k
                {
                    //9+1-k,j+k-1 -> 9+1-k-l,j+k-1+l
                    int r1=10-k,r2=10-k-l;
                    int c1=translatecol(r1,j+k-1);
                    int c2=translatecol(r2,j+k-1+l);
                    fprintf(fp,"-%c%c%c -%c%c%c 0\n",r1+48,c1+48,i+48,r2+48,c2+48,i+48);
                }
            }
        }
    }
    //读取↖行连续性约束
    for(int i=1;i<=9;i++)
    {//代表数字1~9
        for(int j=5;j<=13;j=j+2)
        {
            int x=translatecol(9,j)+4;//对应12345+4,每列有几个数字
            for(int k=1;k<=x-1;k++)
            {
                for(int l=1;l<=x-k;l++)
                {
                    //9+1-k,j+1-k -> 9+1-k-l,j+1-k-l
                    int r1=10-k,r2=10-k-l;
                    int c1=translatecol(r1,j+1-k);
                    int c2=translatecol(r2,j+1-k-l);
                    fprintf(fp,"-%c%c%c -%c%c%c 0\n",r1+48,c1+48,i+48,r2+48,c2+48,i+48);
                }
            }
        }
        for(int j=7;j<=13;j=j+2)
        {
            int x=10-translatecol(1,j);//对应10-2345,每列有几个数字
            for(int k=1;k<=x-1;k++)
            {
                for(int l=1;l<=x-k;l++)
                {
                    //k,j+k-1 -> k+l,j+k-1+l
                    int r1=k,r2=k+l;
                    int c1=translatecol(r1,j+k-1);
                    int c2=translatecol(r2,j+k-1+l);
                    fprintf(fp,"-%c%c%c -%c%c%c 0\n",r1+48,c1+48,i+48,r2+48,c2+48,i+48);
                }
            }

        }
    }
    /**/
    for(int i=1;i<=9;i++)
    {//行选填约束
        if(i==5)continue;//这一行有9个,不需要选填约束.
        switch (i)
        {
            case 1:{
                for(int j=1;j<=10;j++)
                {
                    for(int k=1;k<=5;k++)
                    {
                        if(j==1)fprintf(fp,"1%c1 1%c6 ",k+48,k+48);
                        if(j==2)fprintf(fp,"1%c2 1%c6 ",k+48,k+48);
                        if(j==3)fprintf(fp,"1%c3 1%c6 ",k+48,k+48);
                        if(j==4)fprintf(fp,"1%c4 1%c6 ",k+48,k+48);
                        if(j==5)fprintf(fp,"1%c2 1%c7 ",k+48,k+48);
                        if(j==6)fprintf(fp,"1%c3 1%c7 ",k+48,k+48);
                        if(j==7)fprintf(fp,"1%c4 1%c7 ",k+48,k+48);
                        if(j==8)fprintf(fp,"1%c3 1%c8 ",k+48,k+48);
                        if(j==9)fprintf(fp,"1%c4 1%c8 ",k+48,k+48);
                        if(j==10)fprintf(fp,"1%c4 1%c9 ",k+48,k+48);
                    }fprintf(fp,"0\n");
                }break;
            }
            case 2:{
                for(int j=1;j<=6;j++)
                {
                    for(int k=1;k<=6;k++)
                    {
                        if(j==1)fprintf(fp,"2%c1 2%c7 ",k+48,k+48);
                        if(j==2)fprintf(fp,"2%c2 2%c7 ",k+48,k+48);
                        if(j==3)fprintf(fp,"2%c3 2%c7 ",k+48,k+48);
                        if(j==4)fprintf(fp,"2%c2 2%c8 ",k+48,k+48);
                        if(j==5)fprintf(fp,"2%c3 2%c8 ",k+48,k+48);
                        if(j==6)fprintf(fp,"2%c3 2%c9 ",k+48,k+48);
                    }fprintf(fp,"0\n");
                }break;
            }
            case 3:{
                for(int j=1;j<=3;j++)
                {
                    for(int k=1;k<=7;k++)
                    {
                        if(j==1)fprintf(fp,"3%c1 3%c8 ",k+48,k+48);
                        if(j==2)fprintf(fp,"3%c2 3%c8 ",k+48,k+48);
                        if(j==3)fprintf(fp,"3%c2 3%c9 ",k+48,k+48);
                    }fprintf(fp,"0\n");
                }break;
            }
            case 4:{
                for(int j=1;j<=8;j++)
                fprintf(fp,"4%c1 4%c9 ",j+48,j+48);
                fprintf(fp,"0\n");break;
            }
            case 6:{
                for(int j=1;j<=8;j++)
                fprintf(fp,"6%c1 6%c9 ",j+48,j+48);
                fprintf(fp,"0\n");break;
            }
            case 7:{
                for(int j=1;j<=3;j++)
                {
                    for(int k=1;k<=7;k++)
                    {
                        if(j==1)fprintf(fp,"7%c1 7%c8 ",k+48,k+48);
                        if(j==2)fprintf(fp,"7%c2 7%c8 ",k+48,k+48);
                        if(j==3)fprintf(fp,"7%c2 7%c9 ",k+48,k+48);
                    }fprintf(fp,"0\n");
                }break;
            }
            case 8:{
                for(int j=1;j<=6;j++)
                {
                    for(int k=1;k<=6;k++)
                    {
                        if(j==1)fprintf(fp,"8%c1 8%c7 ",k+48,k+48);
                        if(j==2)fprintf(fp,"8%c2 8%c7 ",k+48,k+48);
                        if(j==3)fprintf(fp,"8%c3 8%c7 ",k+48,k+48);
                        if(j==4)fprintf(fp,"8%c2 8%c8 ",k+48,k+48);
                        if(j==5)fprintf(fp,"8%c3 8%c8 ",k+48,k+48);
                        if(j==6)fprintf(fp,"8%c3 8%c9 ",k+48,k+48);
                    }fprintf(fp,"0\n");
                }break;
            }
            case 9:{
                for(int j=1;j<=10;j++)
                {
                    for(int k=1;k<=5;k++)
                    {
                        if(j==1)fprintf(fp,"9%c1 9%c6 ",k+48,k+48);
                        if(j==2)fprintf(fp,"9%c2 9%c6 ",k+48,k+48);
                        if(j==3)fprintf(fp,"9%c3 9%c6 ",k+48,k+48);
                        if(j==4)fprintf(fp,"9%c4 9%c6 ",k+48,k+48);
                        if(j==5)fprintf(fp,"9%c2 9%c7 ",k+48,k+48);
                        if(j==6)fprintf(fp,"9%c3 9%c7 ",k+48,k+48);
                        if(j==7)fprintf(fp,"9%c4 9%c7 ",k+48,k+48);
                        if(j==8)fprintf(fp,"9%c3 9%c8 ",k+48,k+48);
                        if(j==9)fprintf(fp,"9%c4 9%c8 ",k+48,k+48);
                        if(j==10)fprintf(fp,"9%c4 9%c9 ",k+48,k+48);
                    }fprintf(fp,"0\n");
                }break;
            }
        }

    }

    //↗选填约束
    for(int i=1;i<=9;i++)
    {
        switch (i)
        {
        case 1:{
            for(int j=1;j<=10;j++)
            {
                for(int k=1;k<=5;k++)
                {
                    int row=k,col=translatecol(k,2*i+3-k+1);
                    if(j==1)fprintf(fp,"%c%c1 %c%c6 ",row+48,col+48,row+48,col+48);
                    if(j==2)fprintf(fp,"%c%c2 %c%c6 ",row+48,col+48,row+48,col+48);
                    if(j==3)fprintf(fp,"%c%c3 %c%c6 ",row+48,col+48,row+48,col+48);
                    if(j==4)fprintf(fp,"%c%c4 %c%c6 ",row+48,col+48,row+48,col+48);
                    if(j==5)fprintf(fp,"%c%c2 %c%c7 ",row+48,col+48,row+48,col+48);
                    if(j==6)fprintf(fp,"%c%c3 %c%c7 ",row+48,col+48,row+48,col+48);
                    if(j==7)fprintf(fp,"%c%c4 %c%c7 ",row+48,col+48,row+48,col+48);
                    if(j==8)fprintf(fp,"%c%c3 %c%c8 ",row+48,col+48,row+48,col+48);
                    if(j==9)fprintf(fp,"%c%c4 %c%c8 ",row+48,col+48,row+48,col+48);
                    if(j==10)fprintf(fp,"%c%c4 %c%c9 ",row+48,col+48,row+48,col+48);
                }fprintf(fp,"0\n");
            }break;
        }
        case 2:{
            for(int j=1;j<=6;j++)
            {
                for(int k=1;k<=6;k++)
                {
                    int row=k,col=translatecol(k,2*i+3-k+1);
                    if(j==1)fprintf(fp,"%c%c1 %c%c7 ",row+48,col+48,row+48,col+48);
                    if(j==2)fprintf(fp,"%c%c2 %c%c7 ",row+48,col+48,row+48,col+48);
                    if(j==3)fprintf(fp,"%c%c3 %c%c7 ",row+48,col+48,row+48,col+48);
                    if(j==4)fprintf(fp,"%c%c2 %c%c8 ",row+48,col+48,row+48,col+48);
                    if(j==5)fprintf(fp,"%c%c3 %c%c8 ",row+48,col+48,row+48,col+48);
                    if(j==6)fprintf(fp,"%c%c3 %c%c9 ",row+48,col+48,row+48,col+48);
                }fprintf(fp,"0\n");
            }break;
        }
        case 3:{
            for(int j=1;j<=3;j++)
            {
                for(int k=1;k<=7;k++)
                {
                    int row=k,col=translatecol(k,2*i+3-k+1);
                    if(j==1)fprintf(fp,"%c%c1 %c%c8 ",row+48,col+48,row+48,col+48);
                    if(j==2)fprintf(fp,"%c%c2 %c%c8 ",row+48,col+48,row+48,col+48);
                    if(j==3)fprintf(fp,"%c%c2 %c%c9 ",row+48,col+48,row+48,col+48);
                }fprintf(fp,"0\n");
            }break;
        }
        case 4:{
            for(int k=1;k<=8;k++)
            {
                int row=k,col=translatecol(k,2*i+3-k+1);
                fprintf(fp,"%c%c1 %c%c9 ",row+48,col+48,row+48,col+48);
            }fprintf(fp,"0\n");break;
        }
        case 6:{
            for(int k=1;k<=8;k++)
            {
                int row=k+i-5,col=translatecol(row,14+i-6-k+1);
                fprintf(fp,"%c%c1 %c%c9 ",row+48,col+48,row+48,col+48);
            }fprintf(fp,"0\n");break;
        }
        case 7:{
            for(int j=1;j<=3;j++)
            {
                for(int k=1;k<=7;k++)
                {
                    int row=k+i-5,col=translatecol(row,14+i-6-k+1);
                    if(j==1)fprintf(fp,"%c%c1 %c%c8 ",row+48,col+48,row+48,col+48);
                    if(j==2)fprintf(fp,"%c%c2 %c%c8 ",row+48,col+48,row+48,col+48);
                    if(j==3)fprintf(fp,"%c%c2 %c%c9 ",row+48,col+48,row+48,col+48);
                }fprintf(fp,"0\n");
            }break;
        }
        case 8:{
            for(int j=1;j<=6;j++)
            {
                for(int k=1;k<=6;k++)
                {
                    int row=k+i-5,col=translatecol(row,14+i-6-k+1);
                    if(j==1)fprintf(fp,"%c%c1 %c%c7 ",row+48,col+48,row+48,col+48);
                    if(j==2)fprintf(fp,"%c%c2 %c%c7 ",row+48,col+48,row+48,col+48);
                    if(j==3)fprintf(fp,"%c%c3 %c%c7 ",row+48,col+48,row+48,col+48);
                    if(j==4)fprintf(fp,"%c%c2 %c%c8 ",row+48,col+48,row+48,col+48);
                    if(j==5)fprintf(fp,"%c%c3 %c%c8 ",row+48,col+48,row+48,col+48);
                    if(j==6)fprintf(fp,"%c%c3 %c%c9 ",row+48,col+48,row+48,col+48);
                }fprintf(fp,"0\n");
            }break;
        }
        case 9:{
            for(int j=1;j<=10;j++)
            {
                for(int k=1;k<=5;k++)
                {
                    int row=k+i-5,col=translatecol(row,14+i-6-k+1);
                    if(j==1)fprintf(fp,"%c%c1 %c%c6 ",row+48,col+48,row+48,col+48);
                    if(j==2)fprintf(fp,"%c%c2 %c%c6 ",row+48,col+48,row+48,col+48);
                    if(j==3)fprintf(fp,"%c%c3 %c%c6 ",row+48,col+48,row+48,col+48);
                    if(j==4)fprintf(fp,"%c%c4 %c%c6 ",row+48,col+48,row+48,col+48);
                    if(j==5)fprintf(fp,"%c%c2 %c%c7 ",row+48,col+48,row+48,col+48);
                    if(j==6)fprintf(fp,"%c%c3 %c%c7 ",row+48,col+48,row+48,col+48);
                    if(j==7)fprintf(fp,"%c%c4 %c%c7 ",row+48,col+48,row+48,col+48);
                    if(j==8)fprintf(fp,"%c%c3 %c%c8 ",row+48,col+48,row+48,col+48);
                    if(j==9)fprintf(fp,"%c%c4 %c%c8 ",row+48,col+48,row+48,col+48);
                    if(j==10)fprintf(fp,"%c%c4 %c%c9 ",row+48,col+48,row+48,col+48);
                }fprintf(fp,"0\n");
            }break;
        }
        }
    }
    //↖选填约束
    for(int i=1;i<=9;i++)
    {
        switch (i)
        {
        case 1:{
            for(int j=1;j<=10;j++)
            {
                for(int k=1;k<=5;k++)
                {
                    int row=5+k-i,col=translatecol(row,k+i-1);
                    if(j==1)fprintf(fp,"%c%c1 %c%c6 ",row+48,col+48,row+48,col+48);
                    if(j==2)fprintf(fp,"%c%c2 %c%c6 ",row+48,col+48,row+48,col+48);
                    if(j==3)fprintf(fp,"%c%c3 %c%c6 ",row+48,col+48,row+48,col+48);
                    if(j==4)fprintf(fp,"%c%c4 %c%c6 ",row+48,col+48,row+48,col+48);
                    if(j==5)fprintf(fp,"%c%c2 %c%c7 ",row+48,col+48,row+48,col+48);
                    if(j==6)fprintf(fp,"%c%c3 %c%c7 ",row+48,col+48,row+48,col+48);
                    if(j==7)fprintf(fp,"%c%c4 %c%c7 ",row+48,col+48,row+48,col+48);
                    if(j==8)fprintf(fp,"%c%c3 %c%c8 ",row+48,col+48,row+48,col+48);
                    if(j==9)fprintf(fp,"%c%c4 %c%c8 ",row+48,col+48,row+48,col+48);
                    if(j==10)fprintf(fp,"%c%c4 %c%c9 ",row+48,col+48,row+48,col+48);
                }fprintf(fp,"0\n");
            }break;
        }
        case 2:{
            for(int j=1;j<=6;j++)
            {
                for(int k=1;k<=6;k++)
                {
                    int row=5+k-i,col=translatecol(row,k+i-1);
                    if(j==1)fprintf(fp,"%c%c1 %c%c7 ",row+48,col+48,row+48,col+48);
                    if(j==2)fprintf(fp,"%c%c2 %c%c7 ",row+48,col+48,row+48,col+48);
                    if(j==3)fprintf(fp,"%c%c3 %c%c7 ",row+48,col+48,row+48,col+48);
                    if(j==4)fprintf(fp,"%c%c2 %c%c8 ",row+48,col+48,row+48,col+48);
                    if(j==5)fprintf(fp,"%c%c3 %c%c8 ",row+48,col+48,row+48,col+48);
                    if(j==6)fprintf(fp,"%c%c3 %c%c9 ",row+48,col+48,row+48,col+48);
                }fprintf(fp,"0\n");
            }break;
        }
        case 3:{
            for(int j=1;j<=3;j++)
            {
                for(int k=1;k<=7;k++)
                {
                    int row=5+k-i,col=translatecol(row,k+i-1);
                    if(j==1)fprintf(fp,"%c%c1 %c%c8 ",row+48,col+48,row+48,col+48);
                    if(j==2)fprintf(fp,"%c%c2 %c%c8 ",row+48,col+48,row+48,col+48);
                    if(j==3)fprintf(fp,"%c%c2 %c%c9 ",row+48,col+48,row+48,col+48);
                }fprintf(fp,"0\n");
            }break;
        }
        case 4:{
            for(int k=1;k<=8;k++)
            {
                int row=5+k-i,col=translatecol(row,k+i-1);
                fprintf(fp,"%c%c1 %c%c9 ",row+48,col+48,row+48,col+48);
            }fprintf(fp,"0\n");break;
        }
        case 6:{
            for(int k=1;k<=8;k++)
            {
                int row=k,col=translatecol(k,2*i-5+k-1);
                fprintf(fp,"%c%c1 %c%c9 ",row+48,col+48,row+48,col+48);
            }fprintf(fp,"0\n");break;
        }
        case 7:{
            for(int j=1;j<=3;j++)
            {
                for(int k=1;k<=7;k++)
                {
                    int row=k,col=translatecol(k,2*i-5+k-1);
                    if(j==1)fprintf(fp,"%c%c1 %c%c8 ",row+48,col+48,row+48,col+48);
                    if(j==2)fprintf(fp,"%c%c2 %c%c8 ",row+48,col+48,row+48,col+48);
                    if(j==3)fprintf(fp,"%c%c2 %c%c9 ",row+48,col+48,row+48,col+48);
                }fprintf(fp,"0\n");
            }break;
        }
        case 8:{
            for(int j=1;j<=6;j++)
            {
                for(int k=1;k<=6;k++)
                {
                    int row=k,col=translatecol(k,2*i-5+k-1);
                    if(j==1)fprintf(fp,"%c%c1 %c%c7 ",row+48,col+48,row+48,col+48);
                    if(j==2)fprintf(fp,"%c%c2 %c%c7 ",row+48,col+48,row+48,col+48);
                    if(j==3)fprintf(fp,"%c%c3 %c%c7 ",row+48,col+48,row+48,col+48);
                    if(j==4)fprintf(fp,"%c%c2 %c%c8 ",row+48,col+48,row+48,col+48);
                    if(j==5)fprintf(fp,"%c%c3 %c%c8 ",row+48,col+48,row+48,col+48);
                    if(j==6)fprintf(fp,"%c%c3 %c%c9 ",row+48,col+48,row+48,col+48);
                }fprintf(fp,"0\n");
            }break;
        }
        case 9:{
            for(int j=1;j<=10;j++)
            {
                for(int k=1;k<=5;k++)
                {
                    int row=k,col=translatecol(k,2*i-5+k-1);
                    if(j==1)fprintf(fp,"%c%c1 %c%c6 ",row+48,col+48,row+48,col+48);
                    if(j==2)fprintf(fp,"%c%c2 %c%c6 ",row+48,col+48,row+48,col+48);
                    if(j==3)fprintf(fp,"%c%c3 %c%c6 ",row+48,col+48,row+48,col+48);
                    if(j==4)fprintf(fp,"%c%c4 %c%c6 ",row+48,col+48,row+48,col+48);
                    if(j==5)fprintf(fp,"%c%c2 %c%c7 ",row+48,col+48,row+48,col+48);
                    if(j==6)fprintf(fp,"%c%c3 %c%c7 ",row+48,col+48,row+48,col+48);
                    if(j==7)fprintf(fp,"%c%c4 %c%c7 ",row+48,col+48,row+48,col+48);
                    if(j==8)fprintf(fp,"%c%c3 %c%c8 ",row+48,col+48,row+48,col+48);
                    if(j==9)fprintf(fp,"%c%c4 %c%c8 ",row+48,col+48,row+48,col+48);
                    if(j==10)fprintf(fp,"%c%c4 %c%c9 ",row+48,col+48,row+48,col+48);
                }fprintf(fp,"0\n");
            }break;
        }
        }
    }
    //1,9行-5  2,8行-456  3,7行-34567  4,6行-2345678 5,5行-123456789------含有性约束
    //数目:9-2*ABS(5-row),取值范围:1+ABS(5-row)~9-ABS(5-row)
    for(int i=1;i<=9;i++)
    {
        for(int k=1+ABS(5-i);k<=9-ABS(5-i);k++){
        for(int j=1+ABS(5-i);j<=17-ABS(5-i);j+=2)
        {
            fprintf(fp,"%c%c%c ",i+48,translatecol(i,j)+48,k+48);
        }fprintf(fp,"0\n");}//fprintf(fp,"0\n");
    }
    for(int i=5;i<=13;i+=2)
    {
        int t=translatecol(1,i);
        for(int k=1+ABS(5-t);k<=9-ABS(5-t);k++)
        {
            for(int j=1;j<=t+4;j++)
            {
                fprintf(fp,"%c%c%c ",j+48,translatecol(j,i+1-j)+48,k+48);
            }fprintf(fp,"0\n");
        }
    }
    for(int i=14;i<=17;i++)
    {
        int t=i-8;
        int row=i-12;
        for(int k=1+ABS(5-t);k<=9-ABS(5-t);k++)
        {
            for(int j=1;j<=14-t;j++)
            {
                fprintf(fp,"%c%c%c ",row+48+j-1,translatecol(row+j-1,i-j+1)+48,k+48);
            }fprintf(fp,"0\n");
        }
    }
    for(int i=1;i<=5;i++)
    {
        int t=i;
        for(int k=1+ABS(5-t);k<=9-ABS(5-t);k++)
        {
            for(int j=1;j<=i+4;j++)
            {
                fprintf(fp,"%c%c%c ",6-i+j-1+48,translatecol(6-i+j-1,i+j-1)+48,k+48);
            }fprintf(fp,"0\n");
        }
    }
    for(int i=1;i<=4;i++)
    {
        int t=i+5;
        for(int k=1+ABS(5-t);k<=9-ABS(5-t);k++)
        {
            for(int j=1;j<=9-i;j++)
            {
                fprintf(fp,"%c%c%c ",j+48,translatecol(j,2*i+j-1+5)+48,k+48);
            }fprintf(fp,"0\n");
        }
    }
    ////////////////////////////////////////////////////
    fclose(fp);
    return OK;
}
int translaterow(int a)
{return a;}
int translatecol(int i,int j)
{return (j+1-ABS(5-i))/2;}
/*
            _/    _/            _/    _/
           _/    _/    _/_/    _/    _/     _/_/
          _/_/_/_/  _/_/_/_/  _/    _/    _/   _/
         _/    _/  _/        _/    _/    _/   _/
        _/    _/    _/_/_/  _/_/  _/_/   _/_/

*/
void HELLO()
{
    printf("            _/    _/            _/    _/\n");
    printf("           _/    _/    _/_/    _/    _/     _/_/\n");
    printf("          _/_/_/_/  _/_/_/_/  _/    _/    _/   _/\n");
    printf("         _/    _/  _/        _/    _/    _/   _/\n");
    printf("        _/    _/    _/_/_/  _/_/  _/_/   _/_/\n");
}
/*
            _/_/_/_/  _/_/_/_/  _/       _/  _/_/_/_/         _/_/_/_/  _/    _/  _/_/_/_/  _/_/_/_/
          _/     _/  _/    _/  _/_/   _/_/  _/               _/    _/  _/    _/  _/        _/    _/
         _/         _/_/_/_/  _/  _/_/ _/  _/_/_/_/         _/    _/  _/    _/  _/_/_/_/  _/_/_/
        _/   _/_/  _/    _/  _/  _/   _/  _/               _/    _/  _/   _/   _/        _/   _/
         _/_/_/   _/    _/  _/  _/   _/  _/_/_/_/         _/_/_/_/  _/_/_/   _/_/_/_/   _/    _/
*/
void GAMEOVER()
{
    printf("\n\n\n\n\033[5;31;1m");
    printf("SUCH A DISAPPOINTMENT~~YOU LOST~~\n");
    printf("============================================================================================================\n\n");
    printf("            _/_/_/_/  _/_/_/_/  _/       _/  _/_/_/_/         _/_/_/_/  _/    _/  _/_/_/_/  _/_/_/_/\n");
    printf("          _/     _/  _/    _/  _/_/   _/_/  _/               _/    _/  _/   _/   _/        _/    _/\n");
    printf("         _/         _/_/_/_/  _/  _/_/ _/  _/_/_/_/         _/    _/  _/  _/    _/_/_/_/  _/_/_/\n");
    printf("        _/   _/_/  _/    _/  _/  _/   _/  _/               _/    _/  _/ _/     _/        _/   _/\n");
    printf("         _/_/_/   _/    _/  _/  _/   _/  _/_/_/_/         _/_/_/_/  _/_/      _/_/_/_/  _/    _/\n");
    printf("\n============================================================================================================\n");
    printf("TRY AGAIN.\n");
    printf("\033[0;3;0m");
}