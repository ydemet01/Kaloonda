from pyspark import SparkContext, SparkConf
from numpy import array
from pyspark.mllib.clustering import KMeans, KMeansModel
# for plotting purposes
import pandas as pd
import matplotlib.pyplot as plt

sconf = SparkConf().setAppName("MovieLensApp").setMaster("local[4]")
sc = SparkContext(conf=sconf)

# Load data
data = None

# Parse data (remove header)
data_header = data.take(1)[0]
parsedData = data.filter(lambda line: line!=data_header).map(lambda line: None)

# Build the model (cluster the data)
# train(rdd, k, maxIterations=100, runs=1, initializationMode='k-means||', seed=None, initializationSteps=2, epsilon=0.0001, initialModel=None)
k = 2
kmeans = KMeans.train(None)
# Get the cluster centers, represented as a list of NumPy arrays (see https://spark.apache.org/docs/latest/api/python/pyspark.mllib.html#pyspark.mllib.clustering.KMeans)
centroids = None
# predict returns (a) Predicted cluster label or (b) an RDD of predicted cluster labels if the input is an RDD.
labelsRDD = kmeans.predict(parsedData)
labels = labelsRDD.collect()

#print(centroids)
#print(labels)

######## do not change ########
dataset = pd.read_csv("fleet_data.csv")
X = dataset.values[:,1:]   # features

# figure size
plt.figure(figsize=(5,5))
# customize gridness
plt.rc('grid', linestyle="--", color='gray')
colors = ['r', 'g', 'b', 'm', 'c']
for i in range(k):
    ind = [j for j in range(len(labels)) if labels[j] == i]
    # plot the data observations
    # Colormaps reference: http://matplotlib.org/examples/color/colormaps_reference.html
    plt.scatter(X[ind,0], X[ind,1], c=colors[i], s=10, label='Cluster %d'%i)
    # plot the centroids
    lines = plt.plot(centroids[i][0],centroids[i][1],'kx')
    # make the centroid x's bigger
    plt.setp(lines,ms=15.0)
    plt.setp(lines,mew=2.0)
pltlabels = data_header.split(",")
plt.xlabel(pltlabels[1])
plt.ylabel(pltlabels[2])
# print the grid
plt.grid(True)
# print the lagend
plt.legend()
plt.show()
######## end of non editable area ########