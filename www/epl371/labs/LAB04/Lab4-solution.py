# -*- coding: utf-8 -*-
"""
Course: EPL451
Lab 4: Introduction to Python and Similarity/Distance Measures

@author: Pavlos Antoniou
"""

import pandas as pd
import matplotlib.pyplot as plt
from math import sqrt

# define functions
def euclidean_distance(a,b):
    eucl = 0
    sqrlist = list(map(lambda x,y:pow(x-y,2),a,b))
    eucl = sum(sqrlist)
    """
    # alternative solution
    for i in range(len(a)):
        eucl += pow(a[i]-b[i],2)
    """
    return sqrt(eucl)

def manhattan_distance(a,b):
    manh = 0
    difflist = list(map(lambda x,y:abs(x-y),a,b))
    manh = sum(difflist)
    """
    # alternative solution
    for i in range(len(a)):
        manh += abs(a[i]-b[i])
    """
    return manh

def minkowski_distance(a,b,λ):
    mink = 0
    alist = list(map(lambda x,y:pow(abs(x-y),λ),a,b))
    mink = sum(alist)
    """
    # alternative solution
    for i in range(len(a)):
        mink += pow(abs(a[i]-b[i]),λ)
    """
    return pow(mink,1/λ)

def cosine_similarity(a,b):
    prod = 0
    a_sq = 0
    b_sq = 0
    for i in range(len(a)):
        prod += (a[i]*b[i])
        a_sq += (a[i]**2)
        b_sq += (b[i]**2)
    return prod/(sqrt(a_sq)*sqrt(b_sq))

def jaccard_similarity(a,b):
    aset = set(a)
    bset = set(b)
    union = aset.union(bset)
    intersection = aset.intersection(bset)
    return len(intersection)/len(union)

# ******************* NO NEED TO CHANGE *******************
# import data from csv dataset
df = pd.read_csv("Lab4.csv")
# prints a sample of data from the dataframe
print(df.head())

day1 = list(df.loc[df['timestamp'].map(lambda x: x.startswith('02-01-15'))]['energy'])
day2 = list(df.loc[df['timestamp'].map(lambda x: x.startswith('03-01-15'))]['energy'])

plt.plot(day1, label='Day 1')
plt.plot(day2, label='Day 2')
plt.title("Energy consumption")
plt.xlabel("15-minute interval")
plt.ylabel("Energy (wh)")
plt.legend(loc='upper left')
plt.show()
# *********************************************************

# call functions
print(euclidean_distance(day1,day2))
print(manhattan_distance(day1,day2))
print(minkowski_distance(day1,day2,3))
print(cosine_similarity(day1,day2))
print(jaccard_similarity(day1,day2))