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

//С��ת��Ϊ������
string symbolToBinary(double decimals, int time) {
    string binaryResult = "";
    int integer;
    int a = 1;//����

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


    printf("�ַ�\t����\t\t�ۼƸ���\n");
    for (int i = 0; i < *totalChars; ++i) {
        printf("%c\t%f\t%f\n", probabilities[i].character, probabilities[i].probability, probabilities[i].cumulativeProbability);
    }

    printf("\n��ȷ����: %f bits/����\n", totalInfo);
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
    cout << "������: " << encodedValue << endl;
    string encodedBinary = symbolToBinary(encodedValue, 16);
    cout << "���������: " << encodedBinary << endl;
    return encodedValue;
}
string arithmeticDecoding(double encodedValue, const SymbolProbability* probabilities, int totalSymbols,int strlens) {
    //��ʼ�������ַ����ͽ�������
    string decodedString = "";
    DecodingInterval interval = {'\0', 0.0, 1.0};
    int num=0;
    // �������
    while (true) {
        double range = interval.high - interval.low;
        double scaledValue = (encodedValue - interval.low) / range;
        //��ʼ�������ַ�
        char decodedCharacter = '\0';

        // Ѱ�Ҷ�Ӧ�ķ���
        for (int i = 0; i < totalSymbols; ++i) {
            if (scaledValue >= probabilities[i].cumulativeProbability - probabilities[i].probability &&
                scaledValue < probabilities[i].cumulativeProbability) {
                decodedCharacter = probabilities[i].character;
                break;
            }
        }

        if (decodedCharacter == '\0') {
            cerr << "����ʧ�ܣ�δ�ҵ�ƥ��ķ��š�" << endl;
            break;
        }
        // ��������ַ���ӵ�����ַ���
        decodedString += decodedCharacter;

        // ���½�������
        for (int i = 0; i < totalSymbols; ++i) {
            if (probabilities[i].character == decodedCharacter) {
                interval.high = interval.low + range * probabilities[i].cumulativeProbability;
                interval.low = interval.low + range * (probabilities[i].cumulativeProbability - probabilities[i].probability);
                break;
            }
        }

        // �ж��Ƿ�������
        if (num == strlens-1) {
            break;
        }
        num+=1;
    }

    return decodedString;
}
int main() {
    char stringInput[1000];
    printf("�����ַ���: ");
    fgets(stringInput, sizeof(stringInput), stdin);
    stringInput[strcspn(stringInput, "\n")] = '\0';
    int strlens=strlen(stringInput);
    SymbolProbability probabilities[CHAR_SET_COUNT];
    int totalSymbols;

    calculateProbabilities(stringInput, probabilities, &totalSymbols);
    double encodedvalue=arithmeticCoding(stringInput, probabilities, totalSymbols);
    string decodedString =arithmeticDecoding(encodedvalue, probabilities, totalSymbols,strlens);
    cout << "������: " << decodedString << endl;
    return 0;
}
