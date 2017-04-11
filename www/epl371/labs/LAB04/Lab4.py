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
#print(euclidean_distance(day1,day2))
#print(manhattan_distance(day1,day2))
#print(minkowski_distance(day1,day2,3))
#print(cosine_similarity(day1,day2))
#print(jaccard_similarity(day1,day2))