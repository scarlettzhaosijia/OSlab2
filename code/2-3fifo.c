#include<stdio.h>
#include<stdlib.h>
#define PAGEMAX 100  // 定义页面序列的最大长度

// 物理内存的一个页框
typedef struct page{
    int num;          // 页面编号
    int status;       // 页框的状态：0=空闲，1=被占用
    struct page *next;// 指向下一个页框节点的指针
}page;

// 页框链表的头尾指针
page *head;
page *tail;

//通过repos变量记录下一个要替换的页框位置，每次替换后repos循环递增
//（repos = (repos+1)%blocknum），实现 “最早进入的页面先被替换”。
int repos=0;		// 记录下一个要替换的页框位置
int pagenum;         // 页面访问序列的长度
int blocknum;        // 物理内存的页框数量
int *page_seq;       // 存储页面访问序列的动态数组
int missingcount=0;  // 缺页次数（初始化为0）

// 初始化函数：创建包含blocknum个页框的单链表
void init(){
    int i;
    head = (page *)malloc(sizeof(page));// 动态分配头节点内存（第一个页框）
    tail=head;        // 尾指针初始指向头节点
    head->num = 0;    // 初始页面编号为0（未分配）
    head->status = 0; // 初始状态为空闲

   
    page *Pp[blocknum]; // 存储后续页框节点的地址（简化链表连接）
    for(i =0; i < blocknum-1;i++){// 循环创建blocknum-1个页框节点
        Pp[i] = (page *)malloc(sizeof(page));
        Pp[i]->num = 0;       // 初始页面编号为0
        Pp[i]->status = 0;    // 初始是空闲
    }
    
    for(i =0;i<blocknum-2;i++){// 连接后续页框节点，形成单链表
        Pp[i]->next = Pp[i+1];
    }
    
    if(Pp[0])// 如果创建了后续节点，将头节点的next指向第一个后续节点
        head->next =Pp[0];
    // 如果页框数≥2，尾指针指向最后一个后续节点
    if(blocknum>=2)
        tail = Pp[blocknum-2];
    tail->next=NULL;  // 链表尾节点的next置空
}

// 打印函数：输出当前所有页框的页面编号、头尾节点的页面编号
void print(){
    page *p = head;
    printf("Page ：");
    // 遍历链表，打印每个页框的页面编号
    while (p != NULL) {
        printf("%d ", p->num);
        p = p->next;
    }
    printf("\n");
    printf("The poisition of head：%d\n", head->num); // 打印头节点页面编号
    printf("The poisition of tail：%d\n", tail->num); // 打印尾节点页面编号
    printf("\n");
}

// 查找函数：判断指定页面是否已在物理内存的页框中（命中返回1，缺页返回0）
int search(int p) {
    page *q = head;
    while (q != NULL) {
        if (q->num == p) {
            return 1; // 页面存在，命中
        }
        q = q->next;
    }
    return 0; // 页面不存在，缺页
}

// 查找空闲页框函数：返回第一个空闲（status=0）的页框节点指针，无空闲返回NULL
page *find_free() {
    page *q = head;
    while (q != NULL) {
        if (q->status == 0) {
            return q; // 找到空闲页框
        }
        q = q->next;
    }
    return NULL; // 无空闲页框
}

// 替换函数：实现FIFO置换规则，将指定页面替换到当前repos指向的页框
void replace(int p) {
    int i;
    page *P =(page *)malloc(sizeof(page)); // 注意：此处malloc会造成内存泄漏，后续优化
    P =head; // P指向头节点，开始遍历到repos位置的页框
    // 循环遍历到第repos个页框节点
    for(i=0;i<repos;i++){
        P=P->next;
    }
    P->num = p; // 将新页面编号写入该页框
    repos = (repos+1)%blocknum; // repos循环递增，指向下一个要替换的页框（FIFO核心）
}

// 模拟函数：遍历页面访问序列，模拟页面访问、缺页、置换的全过程
void simulate(){
    int i;
    // 遍历每个页面访问请求
    for(i=0;i<pagenum;i++){
        printf("serch page: %d\n",page_seq[i]); // 打印当前访问的页面
        if(search(page_seq[i])){ // 查找页面是否在内存中
            printf("page hit! \n"); // 页面命中
        }
        else{ // 页面缺页
            printf("page missing \n");
            missingcount++; // 缺页次数加1
            page *fb = find_free(); // 查找空闲页框
            if(fb){ // 有空闲页框
                printf("There is free block, fill it in\n");
                fb->num = page_seq[i]; // 将页面写入空闲页框
                fb->status = 1; // 标记页框为占用状态
            }
            else{ // 无空闲页框，需要置换
                printf("No free block, replace\n");
                replace(page_seq[i]); // 调用FIFO置换函数
            }
            print(); // 打印置换后的页框状态
        }
    }
}

// 主函数：实验入口，处理输入、初始化、模拟、结果输出
int main(){
    int i;int random=0;
    printf("input page number：\n");
    scanf("%d", &pagenum); // 输入页面访问序列的长度
    printf("input block number：\n");
    scanf("%d", &blocknum); // 输入物理内存的页框数量
    // 动态分配内存存储页面访问序列
    page_seq = (int *)malloc(sizeof(int) * pagenum);
    printf("choose the way of creating page sequence\n");
    printf("input 1 for random sequence, 0 for your own sequence\n");
    scanf("%d",&random); // 选择序列生成方式：1=随机，0=手动输入
    if(random){ // 随机生成页面序列
        printf("\nrandom page sequence:");
        for ( i = 0; i < pagenum; i++) {
            // 随机生成页面编号：范围[1, blocknum+2]，确保有足够的缺页
            page_seq[i] = rand() % ((blocknum) + 2)+1;
            printf("%d ", page_seq[i]);
        }
        printf("\n");
    }
    else{ // 手动输入页面序列
        printf("input page sequence: ");
        for ( i = 0; i < pagenum; i++) {
            scanf("%d", &page_seq[i]);
        }
    }
    init(); // 初始化页框链表
    simulate(); // 模拟页面访问过程
    // 输出实验结果：缺页次数和缺页率（保留2位小数）
    printf("page missing number：%d\n", missingcount);
    printf("page missing rate：%.2f%%\n", (double)missingcount / pagenum * 100);
    return 0;
}