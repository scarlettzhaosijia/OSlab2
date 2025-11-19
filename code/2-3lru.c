#include <stdio.h>
#include <stdlib.h>

// 单链表，物理内存的页框
typedef struct Node {
    int data;          // 页面编号
    struct Node* next; // 指向下一个页框节点的指针
} Node;

int cpn=0;//当前内存中的页面数量
int pageMissCount = 0;//缺页次数

// 创建新节点
Node* createNode(int data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

// 头插法插入节点：将新页面插入到链表头部（标记为最近使用）
void insertAtHead(Node** head, int data) {
    Node* newNode = createNode(data);
    newNode->next = *head; // 新节点的next指向原链表头
    *head = newNode;       // 链表头指针指向新节点
}

// 查找节点：判断指定页面是否在内存（链表）中，存在返回节点指针，否则返回NULL
Node* findNode(Node* head, int data) {
    Node* current = head;
    while (current != NULL) {
        if (current->data == data) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// 删除指定节点：从链表中删除指定页面的节点（用于命中时将页面移到头部）
void deleteNode(Node** head, int data) {
    Node* current = *head;
    Node* prev = NULL;

    // 遍历链表，找到要删除的节点及其前驱节点
    while (current != NULL && current->data != data) {
        prev = current;
        current = current->next;
    }

    // 如果找到节点，执行删除操作
    if (current != NULL) {
        if (prev == NULL) { // 要删除的是头节点
            *head = current->next;
        } else { // 要删除的是中间/尾部节点
            prev->next = current->next;
        }
        free(current); // 释放节点内存，避免泄漏
    }
}

// 删除尾节点：删除链表最后一个节点（淘汰最久未使用的页面）
void deleteTail(Node** head) {
    Node* current = *head;
    Node* prev = NULL;

    // 遍历到链表尾部
    while (current != NULL && current->next != NULL) {
        prev = current;
        current = current->next;
    }

    // 执行尾节点删除
    if (current != NULL) {
        if (prev == NULL) { // 链表只有一个节点
            *head = NULL;
        } else { // 链表有多个节点
            prev->next = NULL;
        }
        free(current); // 释放尾节点内存
    }
}

// 打印链表：输出当前内存中的所有页面（从最近使用到最久未使用）
void printList(Node* head) {
    Node* current = head;
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\n");
}

// 主函数：实验入口，处理输入、模拟LRU置换、输出结果
int main() {
    int i;
    int generate=0; // 标记是否生成随机页面序列：1=随机，0=手动输入
    int memorySize, pageSize; // memorySize=物理内存页框数，pageSize=页面序列长度

    // 输入物理内存的页框数量
    printf("input block number: ");
    scanf("%d", &memorySize);

    // 输入页面访问序列的长度
    printf("input page number: ");
    scanf("%d", &pageSize);

    // 动态分配内存，存储页面访问序列
    int* pages = (int*)malloc(sizeof(int) * pageSize);

    // 选择页面序列的生成方式
    printf("choose the way of creating page sequence\n");
    printf("input 1 for random sequence, 0 for your own sequence\n");
    scanf("%d",&generate);

    if(generate){ // 随机生成页面序列
        printf("\nrandom page sequence:");
        for ( i = 0; i < pageSize; i++) {
            // 随机生成页面编号：范围[1, memorySize+2]，确保有足够的缺页
            pages[i] = rand() % ((memorySize) + 2)+1;
            printf("%d ", pages[i]);
        }
        printf("\n");
    }
    else{ // 手动输入页面序列
        printf("input page sequence: ");
        for ( i = 0; i < pageSize; i++) {
            scanf("%d", &pages[i]);
        }
    }
    printf("\n");

    // 初始化链表：memory为物理内存的链表头指针，pageList未实际使用（可忽略）
    Node* memory = NULL;
    Node* pageList = NULL;

    // 遍历页面访问序列，模拟LRU置换过程
    for (i = 0; i < pageSize; i++) {
        int currentPage = pages[i]; // 当前访问的页面编号
        printf("serch for page %d\n", currentPage); // 打印当前访问的页面（笔误：search）

        // 第一步：判断页面是否在内存中（命中）
        if (findNode(memory, currentPage) != NULL) {
            printf("page hit\n"); // 页面命中
            // 命中处理：将该页面从原位置删除，重新插入到链表头部（标记为最近使用）
            deleteNode(&memory, currentPage);
            insertAtHead(&memory, currentPage);
        } 
        // 第二步：页面缺页，执行置换逻辑
        else {
            printf("page missing\n"); // 打印缺页提示
            // 如果内存已满（当前页面数≥页框数），淘汰尾部的最久未使用页面
            if (cpn>=memorySize) {
                printf("page evict\n"); // 打印页面淘汰提示
                deleteTail(&memory); // 删除尾节点（淘汰最久未使用页面）
                cpn--; // 内存页面数减1
            }
            // 将新页面插入到链表头部（标记为最近使用）
            insertAtHead(&memory, currentPage);
            cpn++; // 内存页面数加1
            pageMissCount++; // 缺页次数加1
        }

        // 打印当前内存中的页面状态（从最近使用到最久未使用）
        printf("current page situation：");
        printList(memory);
        printf("\n");
    }

    // 输出实验结果：缺页次数和缺页率（保留2位小数）
    printf("missing number: %d\n", pageMissCount);
    printf("missing rate: %.2f%%\n", (float)pageMissCount / pageSize * 100);

    // 释放动态分配的内存，避免内存泄漏
    free(pages); // 释放页面序列的内存
    // 释放物理内存链表的所有节点
    Node* current = memory;
    while (current != NULL) {
        Node* next = current->next;
        free(current);
        current = next;
    }
    // 释放pageList链表（未实际使用，可忽略）
    current = pageList;
    while (current != NULL) {
        Node* next = current->next;
        free(current);
        current = next;
    }

    return 0;
}