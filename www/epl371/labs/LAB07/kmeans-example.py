from pyspark import SparkContext, SparkConf
from numpy import array
from math import sqrt

from pyspark.mllib.clustering import KMeans, KMeansModel

sconf = SparkConf().setAppName("MovieLensApp").setMaster("local[4]")
sc = SparkContext(conf=sconf)

# Load and parse the data
data = sc.textFile("../data/mllib/kmeans_data.txt")
parsedData = data.map(lambda line: array([float(x) for x in line.split(' ')]))

# Build the model (cluster the data)
# train(rdd, k, maxIterations=100, runs=1, initializationMode='k-means||', seed=None, initializationSteps=2, epsilon=0.0001, initialModel=None)
clusters = KMeans.train(parsedData, 2, maxIterations=10, initializationMode="random")

# Evaluate clustering by computing Within Set Sum of Squared Errors
def error(point):
    center = clusters.centers[clusters.predict(point)]
    return sqrt(sum([x**2 for x in (point - center)]))

WSSSE = parsedData.map(lambda point: error(point)).reduce(lambda x, y: x + y)
print("Within Set Sum of Squared Error = " + str(WSSSE))

# Save and load model
#clusters.save(sc, "KMeansModel")
#sameModel = KMeansModel.load(sc, "KMeansModel")