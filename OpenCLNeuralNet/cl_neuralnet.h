#ifndef CL_NEURALNET_H
#define CL_NEURALNET_H

#define MAXSIZE 1000
#define MAXFILTERDIM 15
#define MAXFILTERS 15
#define MAXPOOLDIM 2
#define N 0.02
#define twoD_access(matrix, row, column, width) (matrix[(row)*(width) + (column)])
#define twoD_index(matrix, row, column, width) ((row)*(width) + (column))

typedef struct Node
{
    int numberOfWeights;
    float weights[MAXSIZE];
    float output;
    float input;
    float errorGradient;
} Node;

typedef struct Layer
{
    int numberOfNodes;
    Node nodes[MAXSIZE];
} Layer;

typedef struct Filter
{
    int filterDim;
    int filterNumber;
    float weights[MAXFILTERDIM]; 
    float costs[MAXFILTERDIM];
    float errorGradient;//sum of the error gradients
} Filter;

typedef struct ConvolutionalLayer
{
    int numberOfFilters;
    Filter filters[MAXFILTERS];
} ConvolutionalLayer;

float inline sigmoid(float n)
{
    //To deal with overflow rounding errors and the such
    if (n < -100)
        return 0;
    if (n > 100)
        return 1;
    return 1/(1 + exp(-n));
}

//Returns the result of passing n through the deriviative of the sigmoid function
float sigmoidDerivative(float n)
{
    float k = sigmoid(n);
    return k*(1-k);
}
#endif