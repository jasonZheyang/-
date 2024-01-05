#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// 哈夫曼树节点结构体
typedef struct HuffmanTreeNode {
    char symbol;
    int frequency;
    float probability; // 新增概率字段
    struct HuffmanTreeNode *left;
    struct HuffmanTreeNode *right;
} HuffmanNode;

// 优先队列节点结构体
typedef struct QueueItem {
    HuffmanTreeNode *node;
    struct QueueItem *next;
} PriorityQueueNode;

// 优先队列结构体
typedef struct {
    QueueItem *front;
} PriorityQueue;

// 全局数组保存哈夫曼编码
char huffmanCodes[128][100];

// 创建一个哈夫曼树节点
HuffmanTreeNode *createHuffmanNode(char symbol, int frequency, float probability) {
    HuffmanTreeNode *node = (HuffmanTreeNode *)malloc(sizeof(HuffmanTreeNode));
    node->symbol = symbol;//符号
    node->frequency = frequency;//频率
    node->probability = probability; // 设置概率
    node->left = NULL;
    node->right = NULL;
    return node;
}

// 创建一个优先队列节点
QueueItem *createPriorityQueueNode(HuffmanTreeNode *node) {
    QueueItem *queueNode = (QueueItem *)malloc(sizeof(QueueItem));
    queueNode->node = node;
    queueNode->next = NULL;
    return queueNode;
}

// 初始化优先队列
PriorityQueue *initPriorityQueue() {
    PriorityQueue *queue = (PriorityQueue *)malloc(sizeof(PriorityQueue));
    queue->front = NULL;
    return queue;
}

// 插入节点到优先队列
void insertIntoPriorityQueue(PriorityQueue *queue, HuffmanTreeNode *node) {
    QueueItem *newNode = createPriorityQueueNode(node);

    if (queue->front == NULL || node->frequency < queue->front->node->frequency) {
        newNode->next = queue->front;
        queue->front = newNode;
    } else {
        QueueItem *temp = queue->front;
        while (temp->next != NULL && temp->next->node->frequency <= node->frequency) {
            temp = temp->next;
        }
        newNode->next = temp->next;
        temp->next = newNode;
    }
}

// 构建哈夫曼树
HuffmanTreeNode *buildHuffmanTree(PriorityQueue *priorityqueue) {
    while (priorityqueue->front->next != NULL) {
        HuffmanTreeNode *node1 = priorityqueue->front->node;
        QueueItem *temp1 = priorityqueue->front;
        priorityqueue->front = priorityqueue->front->next;
        free(temp1);

        HuffmanTreeNode *node2 = priorityqueue->front->node;
        QueueItem *temp2 = priorityqueue->front;
        priorityqueue->front = priorityqueue->front->next;
        free(temp2);

        float probability = node1->probability + node2->probability; // 新节点的概率为两子节点概率之和
        HuffmanTreeNode *newNode = createHuffmanNode('$', node1->frequency + node2->frequency, probability);
        newNode->left = node1;
        newNode->right = node2;
        insertIntoPriorityQueue(priorityqueue, newNode);
    }

    return priorityqueue->front->node;
}

// 递归打印哈夫曼编码和概率
void printHuffmanCodes(HuffmanTreeNode *root, char *code, int top) {
    if (root->left != NULL) {
        code[top] = '0';
        printHuffmanCodes(root->left, code, top + 1);
    }

    if (root->right != NULL) {
        code[top] = '1';
        printHuffmanCodes(root->right, code, top + 1);
    }

    if (root->left == NULL && root->right == NULL) {
        code[top] = '\0';  // 结束编码字符串
        printf("符号: %c, 编码: %s, 概率: %.2f%%\n", root->symbol, code, root->probability * 100);
        strcpy(huffmanCodes[root->symbol], code);
    }
}

// 计算不确定度
float calculateShannonEntropy(char *input, int *frequencies) {
    float entropy = 0.0;
    int totalSymbols = 0;

    for (int i = 0; i < 128; ++i) {
        if (frequencies[i] > 0) {
            totalSymbols += frequencies[i];
            float probability = (float)frequencies[i] / strlen(input);
            entropy += probability * log2(1 / probability);
        }
    }

    return entropy;
}

// 计算哈夫曼编码的平均比特数
float calculateAverageBits(HuffmanTreeNode *root, int depth, int totalSymbols) {
    if (root->left == NULL && root->right == NULL) {
        return (float)depth * (float)root->frequency / (float)totalSymbols;
    }

    float leftBits = calculateAverageBits(root->left, depth + 1, totalSymbols);
    float rightBits = calculateAverageBits(root->right, depth + 1, totalSymbols);

    return leftBits + rightBits;
}

// 计算平均码长
float calculateAverageCodeLength(HuffmanTreeNode *root, int depth) {
    if (root->left == NULL && root->right == NULL) {
        return (float)depth * root->probability;  // 修改此处
    }

    float leftLength = calculateAverageCodeLength(root->left, depth + 1);
    float rightLength = calculateAverageCodeLength(root->right, depth + 1);

    return leftLength + rightLength;
}

// 输出编码后的信息
void printEncodedInfo(char *input) {
    printf("********编码表***************\n");
    for (int i = 0; input[i] != '\0'; ++i) {
        printf("%s", huffmanCodes[input[i]]);
    }
    printf("\n");
}

int main() {
    char input[100];
    printf("输入一段字母: ");
    scanf(" %[^\n]", input);

    int frequencies[128] = {0}; // 假设使用 ASCII 字符集

    // 统计每个字母的出现频率
    for (int i = 0; input[i] != '\0'; ++i) {
        frequencies[input[i]]++;
    }

    // 初始化优先队列
    PriorityQueue *queue = initPriorityQueue();

    // 插入节点到优先队列，并计算概率
    for (int i = 0; i < 128; ++i) {
        if (frequencies[i] > 0) {
            float probability = (float)frequencies[i] / strlen(input);
            HuffmanTreeNode *node = createHuffmanNode(i, frequencies[i], probability);
            insertIntoPriorityQueue(queue, node);
        }
    }

    // 构建哈夫曼树
    HuffmanTreeNode *root = buildHuffmanTree(queue);

    // 打印哈夫曼编码和出现概率
    char code[100];
    int top = 0;
    printf("哈夫曼编码和概率:\n");
    printHuffmanCodes(root, code, top);

    // 输出编码后的信息
    printEncodedInfo(input);

    // 计算哈夫曼编码的香农熵

    float averageBits =calculateShannonEntropy(input, frequencies);
    // 计算平均码长
    float averageCodeLength = calculateAverageCodeLength(root, 0);
    // 计算编码效率
    float efficiency = calculateShannonEntropy(input, frequencies) / averageCodeLength;
    // 输出结果
    printf("香农熵: %.2f\n", averageBits);
    printf("平均码长: %.2f\n", averageCodeLength);
    printf("编码效率: %.2f%%\n", efficiency * 100);
    printf("****************************");
    return 0;
}

