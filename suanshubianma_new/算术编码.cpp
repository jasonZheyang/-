#include <vector>
#include <math.h>
#include <algorithm>
#include <iostream>
#include <cstring>
using namespace std;

#define CHAR_SET_COUNT 256

typedef struct {
    char character;
    double probability;
    double cumulativeProbability;
} SymbolProbability;

struct EncodingInterval {
    double low;
    double high;
};
struct DecodingInterval {
    char character;
    double low;
    double high;
};

//小数转化为二进制
string symbolToBinary(double decimals, int time) {
    string binaryResult = "";
    int integer;
    int a = 1;//计数

    while (a <= time) {
        decimals = decimals * 2;
        int integerPart = floor(decimals);
        binaryResult += to_string(integerPart);
        decimals = decimals - integerPart;
        a++;

        if (decimals == 0) {
            break;
        }
    }

    return binaryResult;
}

void calculateProbabilities(char *input, SymbolProbability *probabilities, int *totalChars) {
    *totalChars = 0;
    int frequencies[CHAR_SET_COUNT] = {0};

    int len = strlen(input);
    for (int i = 0; i < len; ++i) {
        frequencies[(unsigned char)input[i]]++;
    }

    double totalInfo = 0.0;
    double cumulativeProbability = 0.0;
    for (int i = 0; i < CHAR_SET_COUNT; ++i) {
        if (frequencies[i] > 0) {
            probabilities[*totalChars].character = (char)i;
            probabilities[*totalChars].probability = (double)frequencies[i] / len;
            cumulativeProbability += probabilities[*totalChars].probability;
            probabilities[*totalChars].cumulativeProbability = cumulativeProbability;
            totalInfo += -log2(probabilities[*totalChars].probability) * probabilities[*totalChars].probability;
            (*totalChars)++;
        }
    }


    printf("字符\t概率\t\t累计概率\n");
    for (int i = 0; i < *totalChars; ++i) {
        printf("%c\t%f\t%f\n", probabilities[i].character, probabilities[i].probability, probabilities[i].cumulativeProbability);
    }

    printf("\n不确定度: %f bits/符号\n", totalInfo);
}

double arithmeticCoding(char *input, SymbolProbability *probabilities, int totalSymbols) {
    EncodingInterval interval = {0.0, 1.0};
    int inputLength = strlen(input);

    for (int i = 0; i < inputLength; i++) {
        char symbol = input[i];
        double range=interval.high - interval.low;


        for (int j = 0; j < totalSymbols; j++) {
            if (probabilities[j].character == symbol) {
                interval.high = interval.low + range * probabilities[j].cumulativeProbability;
                interval.low = interval.low + range * (probabilities[j].cumulativeProbability - probabilities[j].probability);
                break;
            }
        }
    }

    double encodedValue = (interval.low + interval.high) / 2.0;
    cout << "编码结果: " << encodedValue << endl;
    string encodedBinary = symbolToBinary(encodedValue, 16);
    cout << "编码二进制: " << encodedBinary << endl;
    return encodedValue;
}
string arithmeticDecoding(double encodedValue, const SymbolProbability* probabilities, int totalSymbols,int strlens) {
    //初始化解码字符串和解码区间
    string decodedString = "";
    DecodingInterval interval = {'\0', 0.0, 1.0};
    int num=0;
    // 逐个解码
    while (true) {
        double range = interval.high - interval.low;
        double scaledValue = (encodedValue - interval.low) / range;
        //初始化解码字符
        char decodedCharacter = '\0';

        // 寻找对应的符号
        for (int i = 0; i < totalSymbols; ++i) {
            if (scaledValue >= probabilities[i].cumulativeProbability - probabilities[i].probability &&
                scaledValue < probabilities[i].cumulativeProbability) {
                decodedCharacter = probabilities[i].character;
                break;
            }
        }

        if (decodedCharacter == '\0') {
            cerr << "解码失败：未找到匹配的符号。" << endl;
            break;
        }
        // 将解码的字符添加到结果字符串
        decodedString += decodedCharacter;

        // 更新解码区间
        for (int i = 0; i < totalSymbols; ++i) {
            if (probabilities[i].character == decodedCharacter) {
                interval.high = interval.low + range * probabilities[i].cumulativeProbability;
                interval.low = interval.low + range * (probabilities[i].cumulativeProbability - probabilities[i].probability);
                break;
            }
        }

        // 判断是否解码完成
        if (num == strlens-1) {
            break;
        }
        num+=1;
    }

    return decodedString;
}
int main() {
    char stringInput[1000];
    printf("输入字符串: ");
    fgets(stringInput, sizeof(stringInput), stdin);
    stringInput[strcspn(stringInput, "\n")] = '\0';
    int strlens=strlen(stringInput);
    SymbolProbability probabilities[CHAR_SET_COUNT];
    int totalSymbols;

    calculateProbabilities(stringInput, probabilities, &totalSymbols);
    double encodedvalue=arithmeticCoding(stringInput, probabilities, totalSymbols);
    string decodedString =arithmeticDecoding(encodedvalue, probabilities, totalSymbols,strlens);
    cout << "解码结果: " << decodedString << endl;
    return 0;
}
