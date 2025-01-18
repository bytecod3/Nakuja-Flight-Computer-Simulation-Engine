import csv
import matplotlib.pyplot as plt

# pseudo code
'''
// console menu based
open csv file 
read data 
generate plot 

detect flight states accurately
// use ring buffer to detect apogee 
detect post flight

generate state diagram in a file 

'''
altitude_val = [] # holds altitude values
altitude_x1 = [] # holds x index values for a file with multiple columns
altitude_x = []  # holds x values for file with a single column

def open_csv(filename, has_multiple_columns = 1):
    with open(filename, newline='') as csvfile:
        filereader = csv.reader(csvfile, delimiter=',')

        if has_multiple_columns:
            for row in filereader:
                altitude_x1.append(row[0]) # represents x index
                altitude_val.append(row[1]) # second row represents the altitude data
        else:
            for row in filereader:
                altitude_val.append(row[0]) # this is just for confirmation using different altitude data

def generate_altitude_plot(has_header = 1):
    alt_float = [] # to hold converted values from altitude list
    x_float =[] # to hold converted x ndex values from string to float

    if has_header: # for testing with data that has a single column
        l = altitude_val[1:] # remove column header

        # convert from string to float
        for j in range(0, len(l)):
            alt_float.append(float(l[j]))

        # create the x-axis values, for a file that has only one column
        ln = len(l)
        for i in range(0, ln):
            altitude_x.append(i)

        # plot
        plt.plot(altitude_x, alt_float)
        plt.show()

    else: # for file with multiple columns
        l = altitude_val
        lx = altitude_x1
        # convert from string to float
        for j in range(0, len(l)):
            alt_float.append(float(l[j]))

        # convert x index values from string to integer
        for j in range(0, len(lx)):
            x_float.append(float(lx[j]))


        # plot
        plt.plot(x_float, alt_float)
        plt.show()



'''
normal operation 
use file with 2 columns, second column representing the altitude values
'''

open_csv("altitude_data_matlab.csv")
print(altitude_x1)
generate_altitude_plot(has_header=0)
