"""
EPL451: Data Mining on the Web
Lab5: Classification

@author: Pavlos Antoniou
"""

# Load libraries
import pandas as pd
import numpy as np
from pandas.tools.plotting import scatter_matrix
from pandas.tools.plotting import andrews_curves
from pandas.tools.plotting import parallel_coordinates
from pandas.tools.plotting import radviz
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score
from sklearn.metrics import jaccard_similarity_score
#from sklearn.linear_model import LogisticRegression
#from sklearn.tree import DecisionTreeClassifier
from sklearn.neighbors import KNeighborsClassifier
#from sklearn.discriminant_analysis import LinearDiscriminantAnalysis
#from sklearn.naive_bayes import GaussianNB
from sklearn.svm import SVC
from statistics import mean,stdev

dataset = pd.read_csv("iris_data.csv")

# shape
print(dataset.shape)

# head
print(dataset.head(10))

#data type for each column
print(dataset.dtypes)

# descriptions
print(dataset.describe())

# class distribution
print(dataset.groupby('class').size())

# Univariate Plots: plots of each individual variable
# box and whisker plots: given that the input variables are numeric
dataset.plot(kind='box', subplots=True, layout=(2,2), sharex=False, sharey=False)
plt.show()

# histograms
dataset.hist()
plt.show()

# Multivariate Plots: look at the interactions between the variables
scatter_matrix(dataset)
plt.show()
andrews_curves(dataset, 'class')
plt.show()
parallel_coordinates(dataset, 'class')
plt.show()
#radviz(dataset, 'class')
#plt.show()

# Split datasets
X = dataset.values[:,0:4]   # features
Y = dataset.values[:,4]     # target
x_train, x_test, y_train, y_test = train_test_split(X, Y, test_size=0.4)        # test = 40%, train = 60%
x_test, x_val, y_test, y_val = train_test_split(x_test, y_test, test_size=0.5)  # test = 20%, validation = 20%

# KNN
# create an instance of Neighbours Classifier and fit the data.
knn = KNeighborsClassifier()
knn.fit(x_train, y_train)

# use validation (features) data to predict target
y_pred_knn = knn.predict(x_val)

# Compare actual class labels with predicted class labels from validation dataset
accuracy = accuracy_score(y_val, y_pred_knn)
#jaccard_similarity = jaccard_similarity_score(y_val, y_pred_knn)
print ("[KNN] Predicted model accuracy: "+ str(accuracy))
#print ("[KNN] Predicted model accuracy (jaccard): "+ str(jaccard_similarity))

# n_neighbors : int, optional (default = 5)
# Number of neighbors to use by default for k_neighbors queries.
results = []
for n in range(1, 40, 1):
    knn = KNeighborsClassifier(n_neighbors=n)
    knn.fit(x_train, y_train)
    y_pred_knn = knn.predict(x_val)
    accuracy = accuracy_score(y_val, y_pred_knn)
    results.append([n, accuracy])

# create a new dataframe with results list as dataframe values
results = pd.DataFrame(results, columns=["n", "accuracy"])

plt.plot(results.n, results.accuracy)
plt.title("Accuracy with Increasing K")
plt.xlabel("K")
plt.ylabel("accuracy")
plt.show()

# SVM
# create an instance of K Neighbours Classifier and fit the data.
svm = SVC()
svm.fit(x_train, y_train)

# use validation (features) data to predict target
y_pred_svm = svm.predict(x_val)

# Compare actual class labels with predicted class labels from validation dataset
accuracy = accuracy_score(y_val, y_pred_svm)
#jaccard_similarity = jaccard_similarity_score(y_val, y_pred_svm)
print ("[SVM] Predicted model accuracy: "+ str(accuracy))
#print ("[SVM] Predicted model accuracy (jaccard): "+ str(jaccard_similarity))

accuracy = [[],[]]
names = ["KNN", "SVM"]
for run in range(51):
    x_train, x_test, y_train, y_test = train_test_split(X, Y, test_size=0.2)        # test = 40%, train = 60%
    x_test, x_val, y_test, y_val = train_test_split(x_test, y_test, test_size=0.5)
    
    knn = KNeighborsClassifier(n_neighbors=5)
    knn.fit(x_train, y_train)
    y_pred_knn = knn.predict(x_val)
    accuracy[0].append(accuracy_score(y_val, y_pred_knn))
    
    # Add code to run SVM
    
    
    

knn_mean = mean(accuracy[0])
knn_stdev = stdev(accuracy[0])
# Add code to evaluate mean and std values for svm


print("[KNN] Mean: " + str(knn_mean) + ", Stdev: " + str(knn_stdev))
# Print SVM mean and std values

fig = plt.figure()
fig.suptitle('Algorithm Comparison')
ax = fig.add_subplot(111)
plt.boxplot(accuracy)
ax.set_xticklabels(names)
plt.show()