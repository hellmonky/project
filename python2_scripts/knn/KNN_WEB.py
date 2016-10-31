# -*- coding: utf-8 -*-

import urllib
import re
from numpy import *
import operator

def get_data():
    url_data = urllib.urlopen('http://archive.ics.uci.edu/ml/machine-learning-databases/iris/bezdekIris.data').read()
    return url_data
def init_data():
    # 对获取的数据进行预处理 ，字符串转换成数字，特征和类别标签分类
    url_data = get_data()
    url_data = re.sub('Iris-setosa','1',url_data)
    url_data = re.sub('Iris-versicolor','2',url_data)
    url_data = re.sub('Iris-virginica','3',url_data)
    lines = re.split('\n',url_data)
    m =len(lines)-2
    dataSet = zeros((m,5))
    dataLabel = zeros((m,1))
    for row in range(m):
        line = re.split(',',lines[row])
        col = len(line)
        for i in range(col):
            dataSet[row,i] = (line[i])
    dataLabel = dataSet[:,-1]
    dataSet = dataSet[:,(0,1,2,3)]
    return dataSet,dataLabel

def classify0(intX,dataSet,labels,k):
    # 产生与dataSet一样大小的矩阵，矩阵的每行数据都是intX
    # 根据矩阵的减法 求聚类最近的k个类
    dataSetSize=dataSet.shape[0]
    diffMat=tile(intX,(dataSetSize,1))-dataSet
    sqDiffMat=diffMat**2
    sqDistances=sqDiffMat.sum(axis=1)
    distances=sqDistances**0.5
    #  distances.argsort() 的结果是保存 升序后的原数据的id下标
    sortedDistIndicies=distances.argsort()
    classCount={}
    # 取出前k个最近的类别标签，以最多的类别作为该样本的类别
    for i in range(k):
        voteIlabel=labels[sortedDistIndicies[i]]
        classCount[voteIlabel]=classCount.get(voteIlabel,0)+1
    sortedClassCount=sorted(classCount.iteritems(),
                            key=operator.itemgetter(1),reverse=True)
    return sortedClassCount[0][0]

def norm_max_min(dataSet):
    # max - min 归一化
    # 求出每一类的最大值 最小值
    maxVals = dataSet.max(axis=0)
    minVals = dataSet.min(axis=0)
    ranges = maxVals - minVals
    nromDataSet = zeros(shape(dataSet))
    m = dataSet.shape[0]
    nromDataSet = dataSet-tile(minVals,(m,1))
    nromDataSet = nromDataSet/(tile(ranges,(m,1)))
    return minVals,ranges,nromDataSet

def norm_zScore(dataSet):
    dataSet = array(dataSet)
    # 计算每类的均值 和标准差
    meanVals = dataSet.mean(0)
    stdVals = dataSet.std(0)
    m = dataSet.shape[0]
    normDataSet = dataSet-tile(meanVals,(m,1))
    normDataSet = normDataSet/tile(stdVals,(m,1))
    return meanVals,stdVals,normDataSet
def irisClassTest():
    dataSet,dataLabel = init_data()
    trainSet = []
    trainLabel = []
    testSet = []
    testLabel = []
    predictClass = []
    # k 的取值 0 -20 的奇数
    k = 5
    #计算预测正确率
    acc = 0.0
    error = 0.0
    # 随机抽取40%作为测试集
    testLen = int(len(dataLabel)*0.4)
    #trainIndex 这里是下标，在150个下表中随机删除测试集
    trainIndex = range(len(dataLabel))
    testIndex = []

    for index in range(testLen):
        randIndex = int(random.uniform(0,len(trainIndex)))
        testIndex.append(trainIndex[randIndex])
        del(trainIndex[randIndex])
    # 训练集
    for index in trainIndex:
        trainSet.append(dataSet[index])
        trainLabel.append(dataLabel[index])
    #　测试集　
    for index in testIndex:
        testSet.append(dataSet[index])
        testLabel.append(dataLabel[index])
    # 对测试集中的每条数据判断其做在的类别，并计算准确率
    # 对数据进行归一化
    # minVals,ranges,normTrainSet = norm_max_min(array(trainSet))

    meanVals,stdVals,normTrainSet = norm_zScore(array(trainSet))

    for i in range(testLen):
        intX = testSet[i]
        intX = array(intX)
        # normX = (intX-minVals)/ranges
        normX = (intX-meanVals)/stdVals
        classifierResult = classify0(normX,array(normTrainSet),trainLabel,k)
        predictClass .append(classifierResult)
        if(classifierResult != testLabel[i]):
            error= error + 1.0
    acc=1.0-error/float(testLen)
    print error,acc

# 执行主函数测试
if __name__ == "__main__":
    irisClassTest();
    irisClassTest();
    irisClassTest();
    irisClassTest();
    irisClassTest();
