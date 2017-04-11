# -*- coding: utf-8 -*-
"""
EPL451: Data Mining on the Web
Lab5: Clustering

@author: Pavlos Antoniou
"""

# Load libraries
import pandas as pd
import numpy as np
from sklearn.cluster import KMeans
from sklearn.metrics import silhouette_score
from scipy.spatial.distance import cdist, pdist
import matplotlib.pyplot as plt

dataset = pd.read_csv("fleet_data.csv")
    
# shape
print(dataset.shape)
    
# head
print(dataset.head(10))
    
#data type for each column
print(dataset.dtypes)
    
# descriptions
print(dataset.describe())

k = 2
X = dataset.values[:,1:]   # features
kmeans = KMeans(n_clusters=k).fit(X)

labels = kmeans.labels_
centroids = kmeans.cluster_centers_
print(centroids)

# figure size
plt.figure(figsize=(5,5))
# customize gridness
plt.rc('grid', linestyle="--", color='gray')
colors = ['r', 'g', 'b', 'm', 'c']
for i in range(k):
    ind = [j for j in range(labels.size) if labels[j] == i]
    # plot the data observations
    # Colormaps reference: http://matplotlib.org/examples/color/colormaps_reference.html
    plt.scatter(X[ind,0], X[ind,1], c=colors[i], s=10, label='Cluster %d'%i)
    # plot the centroids
    lines = plt.plot(centroids[i,0],centroids[i,1],'kx')
    # make the centroid x's bigger
    plt.setp(lines,ms=15.0)
    plt.setp(lines,mew=2.0)
plt.xlabel(dataset.columns[1])
plt.ylabel(dataset.columns[2])
# print the grid
plt.grid(True)
# print the lagend
plt.legend()
plt.show()

# Evaluation
# elbow & silhouette curve
# cluster data into K=1..10 clusters
K = range(2,10)
KM = [KMeans(n_clusters=k).fit(X) for k in K]
centroids = [k.cluster_centers_ for k in KM]
labels = [k.labels_ for k in KM]

# find distances of each driver from all centroids
D_k = [cdist(X, cent, 'euclidean') for cent in centroids]
# find the index of the closest cluster (each row of cIdx contains all indexes for a given k)
cIdx = [np.argmin(D,axis=1) for D in D_k]
# find the distance to the closest cluster
# (each row of dist contains the distances to the closest centroid for each point in X)
dist = [np.min(D,axis=1) for D in D_k]
# X.shape[0] is the len of rows
avgWithinSS = [np.sum(d)/X.shape[0] for d in dist]

# Total with-in sum of square
wcss = [np.sum(d**2) for d in dist]
tss = np.sum(pdist(X)**2)/X.shape[0]
bss = tss-wcss

kIdx = 2 # index of k you want to provide a circle marker on

# elbow curve
fig = plt.figure()
ax = fig.add_subplot(111)
ax.plot(K, avgWithinSS, 'b*-')
ax.plot(K[kIdx], avgWithinSS[kIdx], marker='o', markersize=12, 
markeredgewidth=2, markeredgecolor='r', markerfacecolor='None')
plt.grid(True)
plt.xlabel('Number of clusters (k)')
plt.ylabel('Average within-cluster sum of squares')
plt.title('Elbow for KMeans clustering')

"""
fig = plt.figure()
ax = fig.add_subplot(111)
ax.plot(K, bss/tss*100, 'b*-')
ax.plot(K[kIdx], bss[kIdx]/tss*100, marker='o', markersize=12, 
markeredgewidth=2, markeredgecolor='r', markerfacecolor='None')
plt.grid(True)
plt.xlabel('Number of clusters (k)')
plt.ylabel('Percentage of variance explained')
plt.title('Elbow for KMeans clustering')
plt.show()
"""

fig = plt.figure()
ax = fig.add_subplot(111)
s = [silhouette_score(X, l, metric='euclidean') for l in labels]
plt.plot(K,s)
plt.grid(True)
plt.xlabel('Number of clusters (k)')
plt.ylabel("Silouette")
plt.title("Silouette for Kmeans cell's behaviour")
plt.show()