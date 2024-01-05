#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ���������ڵ�ṹ��
typedef struct HuffmanTreeNode {
    char symbol;
    int frequency;
    float probability; // ���������ֶ�
    struct HuffmanTreeNode *left;
    struct HuffmanTreeNode *right;
} HuffmanNode;

// ���ȶ��нڵ�ṹ��
typedef struct QueueItem {
    HuffmanTreeNode *node;
    struct QueueItem *next;
} PriorityQueueNode;

// ���ȶ��нṹ��
typedef struct {
    QueueItem *front;
} PriorityQueue;

// ȫ�����鱣�����������
char huffmanCodes[128][100];

// ����һ�����������ڵ�
HuffmanTreeNode *createHuffmanNode(char symbol, int frequency, float probability) {
    HuffmanTreeNode *node = (HuffmanTreeNode *)malloc(sizeof(HuffmanTreeNode));
    node->symbol = symbol;//����
    node->frequency = frequency;//Ƶ��
    node->probability = probability; // ���ø���
    node->left = NULL;
    node->right = NULL;
    return node;
}

// ����һ�����ȶ��нڵ�
QueueItem *createPriorityQueueNode(HuffmanTreeNode *node) {
    QueueItem *queueNode = (QueueItem *)malloc(sizeof(QueueItem));
    queueNode->node = node;
    queueNode->next = NULL;
    return queueNode;
}

// ��ʼ�����ȶ���
PriorityQueue *initPriorityQueue() {
    PriorityQueue *queue = (PriorityQueue *)malloc(sizeof(PriorityQueue));
    queue->front = NULL;
    return queue;
}

// ����ڵ㵽���ȶ���
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

// ������������
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

        float probability = node1->probability + node2->probability; // �½ڵ�ĸ���Ϊ���ӽڵ����֮��
        HuffmanTreeNode *newNode = createHuffmanNode('$', node1->frequency + node2->frequency, probability);
        newNode->left = node1;
        newNode->right = node2;
        insertIntoPriorityQueue(priorityqueue, newNode);
    }

    return priorityqueue->front->node;
}

// �ݹ��ӡ����������͸���
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
        code[top] = '\0';  // ���������ַ���
        printf("����: %c, ����: %s, ����: %.2f%%\n", root->symbol, code, root->probability * 100);
        strcpy(huffmanCodes[root->symbol], code);
    }
}

// ���㲻ȷ����
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

// ��������������ƽ��������
float calculateAverageBits(HuffmanTreeNode *root, int depth, int totalSymbols) {
    if (root->left == NULL && root->right == NULL) {
        return (float)depth * (float)root->frequency / (float)totalSymbols;
    }

    float leftBits = calculateAverageBits(root->left, depth + 1, totalSymbols);
    float rightBits = calculateAverageBits(root->right, depth + 1, totalSymbols);

    return leftBits + rightBits;
}

// ����ƽ���볤
float calculateAverageCodeLength(HuffmanTreeNode *root, int depth) {
    if (root->left == NULL && root->right == NULL) {
        return (float)depth * root->probability;  // �޸Ĵ˴�
    }

    float leftLength = calculateAverageCodeLength(root->left, depth + 1);
    float rightLength = calculateAverageCodeLength(root->right, depth + 1);

    return leftLength + rightLength;
}

// �����������Ϣ
void printEncodedInfo(char *input) {
    printf("********�����***************\n");
    for (int i = 0; input[i] != '\0'; ++i) {
        printf("%s", huffmanCodes[input[i]]);
    }
    printf("\n");
}

int main() {
    char input[100];
    printf("����һ����ĸ: ");
    scanf(" %[^\n]", input);

    int frequencies[128] = {0}; // ����ʹ�� ASCII �ַ���

    // ͳ��ÿ����ĸ�ĳ���Ƶ��
    for (int i = 0; input[i] != '\0'; ++i) {
        frequencies[input[i]]++;
    }

    // ��ʼ�����ȶ���
    PriorityQueue *queue = initPriorityQueue();

    // ����ڵ㵽���ȶ��У����������
    for (int i = 0; i < 128; ++i) {
        if (frequencies[i] > 0) {
            float probability = (float)frequencies[i] / strlen(input);
            HuffmanTreeNode *node = createHuffmanNode(i, frequencies[i], probability);
            insertIntoPriorityQueue(queue, node);
        }
    }

    // ������������
    HuffmanTreeNode *root = buildHuffmanTree(queue);

    // ��ӡ����������ͳ��ָ���
    char code[100];
    int top = 0;
    printf("����������͸���:\n");
    printHuffmanCodes(root, code, top);

    // �����������Ϣ
    printEncodedInfo(input);

    // ����������������ũ��

    float averageBits =calculateShannonEntropy(input, frequencies);
    // ����ƽ���볤
    float averageCodeLength = calculateAverageCodeLength(root, 0);
    // �������Ч��
    float efficiency = calculateShannonEntropy(input, frequencies) / averageCodeLength;
    // ������
    printf("��ũ��: %.2f\n", averageBits);
    printf("ƽ���볤: %.2f\n", averageCodeLength);
    printf("����Ч��: %.2f%%\n", efficiency * 100);
    printf("****************************");
    return 0;
}

