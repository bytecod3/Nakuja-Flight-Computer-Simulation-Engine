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
altitude_val = []
altitude_x = []

def open_csv(filename):
    with open(filename, newline='') as csvfile:
        filereader = csv.reader(csvfile, delimiter=',')
        for row in filereader:
            altitude_val.append(row[0])


def generate_altitude_plot():
    l = altitude_val[1:] # remove column header
    l_float = []

    # convert altitude value from string to float
    for j in range(0, len(l)):
        l_float.append(float(l[j]))

    # create the x axis values
    ln = len(l)
    for i in range(0, ln):
        altitude_x.append(i)

    # plot
    plt.plot(altitude_x, l_float)
    plt.show()

open_csv("altitude_log.csv")
generate_altitude_plot()
