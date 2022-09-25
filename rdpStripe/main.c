//
//  main.c
//  rdpStripe
//
//  Created by 李萍萍 on 2021/10/20.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

/**
 取模操作
 */
int getMod(int m, int n) {
    if(m >= 0) {
        return m % n;
    }else {
        return (m + n) % n;
    }
}

/**
 编码
 */
void encoding(uint8_t **codeArray, int p) {
    int row;
    // 行校验
    for(int i = 0; i < p - 1; i++) {
        for(int j = 0; j < p - 1; j++) {
           codeArray[i][p-1] ^= codeArray[i][j];
        }
    }
    
    // 斜校验
    for(int i = 0; i < p - 1; i++) {
        for(int j = 0; j < p; j++) {
            row = getMod(i - j, p);
            if(row < p - 1) {
                codeArray[i][p] ^= codeArray[row][j];
            }
        }
    }
}

/**
 编码：条带上
 */
void encodingOnStripe(uint8_t ***codeArrayStripes, int stripeNum, int p) {
    for(int i = 0; i < stripeNum; i++) {
        encoding(codeArrayStripes[i], p);
    }
}

/**
 解码
 */
void decoding(uint8_t **codeArray, int p, int *errorCols) {
    //两列之间的距离
    int distance = errorCols[1] - errorCols[0];
    //与第一个失效列相交且不与第二个失效列相交的斜线
    int diagonal_1 = errorCols[1] - 1;
    //与第二个失效列相交且不与第一个失效列相交的斜线
    int diagonal_2 = getMod(errorCols[0] - 1, p);
    //存储解码过程中的斜线
    int diagonal = diagonal_1;
    //存储取模值
    int k = getMod(diagonal - errorCols[0], p), krow;
    //先用diagonal1开始解码
    while(diagonal != p - 1) {
        codeArray[k][errorCols[0]] = 0;
        //用斜线解
        for(int j = 0; j <= p; j++) {
            krow = getMod(diagonal - j, p);
            if(krow < p - 1 && j != errorCols[0]) {
                codeArray[k][errorCols[0]] ^= codeArray[krow][j];
            }
        }
        //用行解
        codeArray[k][errorCols[1]] = 0;
        for(int i = 0; i < p; i++) {
            //printf("codeArray[%d][%d]=%d,\n", k, i, codeArray[k][i]);
            if(i != errorCols[1])
               codeArray[k][errorCols[1]] ^= codeArray[k][i];
        }
        //当前斜线
        diagonal = getMod(diagonal+distance, p);
        //改变k，进行下一轮求解
        k = getMod(diagonal - errorCols[0], p);
    }
    //记录当前斜线
    diagonal = diagonal_2;
    k = getMod(diagonal - errorCols[1], p);
    //用diagonal2开始求解
    while(diagonal != p-1) {
        codeArray[k][errorCols[1]] = 0;
        //用斜线解
        for(int j = 0; j <= p; j++) {
            krow = getMod(diagonal - j, p);
            if(krow < p - 1 && j != errorCols[1]) {
                //printf("codeArray[%d][%d],\n", krow, j);
                codeArray[k][errorCols[1]] ^= codeArray[krow][j];
            }
        }
        //用行解
        codeArray[k][errorCols[0]] = 0;
        for(int i = 0; i < p; i++) {
            if(i != errorCols[0])
                codeArray[k][errorCols[0]] ^= codeArray[k][i];
        }
        
        //斜线改变
        diagonal = getMod(diagonal - distance, p);
        //改变k
        k = getMod(diagonal - errorCols[1], p);
    }
     
}

/**
 解码：条带上
 */
void decodingOnStripe(uint8_t ***codeArrayStripe, int stripeNum, int p, int *errorCols) {
    for(int i = 0; i < stripeNum; i++) {
        decoding(codeArrayStripe[i], p, errorCols);
    }
}

//判断两个矩阵是否相等
int isEqual(uint8_t **codeArray, uint8_t **orignalArray, int p) {
    for(int i = 0; i < p-1; i++) {
        for(int j = 0; j < p + 1; j++) {
            if(codeArray[i][j] != orignalArray[i][j]) {
                printf("codeArray[%d][%d] = %d\n", i, j, codeArray[i][j]);
                printf("orignalArray[%d][%d] = %d\n", i, j, orignalArray[i][j]);
                return 1;
            }
        }
        //printf("================\n");
    }
    return 0;
}

/**
 判断两矩阵是否相等：条带上
 */
int isEqualOnStripe(uint8_t ***codeArrayStripe, uint8_t ***orignalStripe, int stripeNum, int p) {
    int flag;
    for(int i = 0; i < stripeNum; i++) {
        flag = isEqual(codeArrayStripe[i], orignalStripe[i], p);
        if(flag) {
            return 1;
        }
    }
    return 0;
}

//置两列失效
void setError(uint8_t **codeArray, int p, int *errorCols) {
    for(int i = 0; i < p - 1; i++) {
        codeArray[i][errorCols[0]] = 0;
        codeArray[i][errorCols[1]] = 0;
    }
}

/**
 置两列失效：条带上
 */
void setErrorOnStripe(uint8_t ***codeArrayStripe, int stripeNum, int p, int *errorCols) {
    for(int i = 0; i < stripeNum; i++) {
        setError(codeArrayStripe[i], p, errorCols);
    }
}

//判断是否为素数
int isPrime(int num) {
    for(int i = 2; i * i <= num; i++) {
        if(num % i == 0) {
            return 0;
        }
    }
    return 1;
}

//打印输出
void printArray(uint8_t **codeArray, int p) {
    for(int i = 0; i < p - 1; i++) {
        for(int j = 0; j < p + 1; j++) {
            printf("%d\t", codeArray[i][j]);
        }
        printf("\n");
    }
}

/**
 打印输出：条带上
 */
void printStripe(uint8_t ***codeArrayStripe, int stripeNum, int p) {
    for(int i = 0; i < stripeNum; i++) {
        printArray(codeArrayStripe[i], p);
    }
}

/**
 随机赋值:条带上
 */
void randValueToStripe(uint8_t ***codeArrayStripe, int stripeNum, int p) {
    srand((unsigned)time(NULL));
    for(int i = 0; i < stripeNum; i++) {
        for(int j = 0; j < p - 1; j++) {
            for(int k = 0; k < p - 1; k++) {
                codeArrayStripe[i][j][k] = rand();
            }
            codeArrayStripe[i][j][p - 1] = 0;
            codeArrayStripe[i][j][p] = 0;
        }
    }
}

/**
 保存原始数据：条带上
 */
void saveArrayStripe(uint8_t ***codeArrayStripe, uint8_t ***orignalStripe, int stripeNum, int p) {
    for(int i = 0; i < stripeNum; i++) {
        for(int j = 0; j < p - 1; j++) {
            for(int k = 0; k < p + 1; k++) {
                orignalStripe[i][j][k] = codeArrayStripe[i][j][k];
            }
        }
    }
}

int main(int argc, const char * argv[]) {
    // insert code here...
    //条带规模
    int stripeNum = 300, p = 103;
    //计时
    struct timeval encodeStart, encodeEnd, decodeStart, decodeEnd;
    //编码｜解码 时间
    long encodeTime, decodeTime;
    //错误列
    int errorCols[2] = {33, 74};
    //容量
    long long memory = sizeof(uint8_t) * 8 * (p - 1) * (p - 1) * stripeNum;
    //是否解码成功
    int isSuccess;
    uint8_t ***codeArrayStripe = (uint8_t ***) malloc(stripeNum * sizeof(uint8_t **));
    //记录条带
    uint8_t ***orignalStripe = (uint8_t ***) malloc(stripeNum * sizeof(uint8_t **));
    //开辟空间
    for(int i = 0; i < stripeNum; i++) {
        codeArrayStripe[i] = (uint8_t **) malloc((p - 1) * sizeof(uint8_t *));
        orignalStripe[i] = (uint8_t **) malloc((p - 1) * sizeof(uint8_t *));
        for(int j = 0; j < p - 1; j++) {
            codeArrayStripe[i][j] = (uint8_t *) malloc((p + 1) * sizeof(uint8_t));
            orignalStripe[i][j] = (uint8_t *) malloc((p + 1) * sizeof(uint8_t));
        }
    }
    
    //随机赋值
    randValueToStripe((uint8_t ***) codeArrayStripe, stripeNum, p);
    
    //编码
    gettimeofday(&encodeStart, NULL);
    encodingOnStripe((uint8_t ***)codeArrayStripe, stripeNum, p);
    gettimeofday(&encodeEnd, NULL);
    //编码时间
    encodeTime = (encodeEnd.tv_sec - encodeStart.tv_sec) * 1000000 + encodeEnd.tv_usec - encodeStart.tv_usec;
    

    //保存数据
    saveArrayStripe((uint8_t ***) codeArrayStripe, (uint8_t ***) orignalStripe, stripeNum, p);
    
    //置0
    setErrorOnStripe((uint8_t ***) codeArrayStripe, stripeNum, p, errorCols);
    //解码
    gettimeofday(&decodeStart, NULL);
    decodingOnStripe((uint8_t ***) codeArrayStripe, stripeNum, p, errorCols);
    gettimeofday(&decodeEnd, NULL);
    //解码时间
    decodeTime = (decodeEnd.tv_sec - decodeStart.tv_sec) * 1000000 + decodeEnd.tv_usec - decodeStart.tv_usec;
    
    //是否解码成功
    isSuccess = isEqualOnStripe((uint8_t ***) codeArrayStripe, (uint8_t ***) orignalStripe, stripeNum, p);
    
    printf("p = %d, stripeNum = %d\n", p, stripeNum);
    //printf("memory = %lld \n", memory);
    printf("失效列： %d, %d \n", errorCols[0], errorCols[1]);
    printf("编码速度：%.2f Gb/s \n", (double)memory / encodeTime / 1024.0 / 1024.0 / 1024.0 * 1000000);
    if(!isSuccess) {
        printf("解码成功！\n");
        printf("解码速度：%.2f Gb/s \n", (double)memory / decodeTime / 1024.0 / 1024.0 / 1024.0 * 1000000);
    } else {
        printf("解码失败！\n");
    }
    return 0;
}
