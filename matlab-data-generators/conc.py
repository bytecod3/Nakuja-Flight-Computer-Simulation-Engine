### join 2 rows from differenct csv files into a single file as csv

import pandas as pd

# read csv files
data1 = pd.read_csv('time_log.csv')
data2 = pd.read_csv('altitude_log.csv')

# merge the columns horizontally -> column-wise
df_merged = pd.concat([data1, data2], axis=1)

# save the seleted columns to a new csv file 
df_merged.to_csv('output.csv', index=False)
