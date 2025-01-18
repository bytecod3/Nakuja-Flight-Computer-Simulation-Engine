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
        plt.xlabel("x-index")
        plt.ylabel("altitude (meters)")
        plt.title("Simulated altitude graph")
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
        plt.xlabel("x-index")
        plt.ylabel("altitude (meters)")
        plt.title("Simulated altitude graph")
        plt.show()


def open_csv(filename):
    has_multiple_columns = 0
    # check filename for multiple columns
    file_with_multi_columns = "altitude_data_matlab.csv"
    file_single_columns = "altitude_log.csv"

    if filename == "altitude_data_matlab.csv":
        has_multiple_columns = 1
    elif filename == "altitude_log.csv":
        has_multiple_columns = 0

    with open(filename, newline='') as csvfile:
        filereader = csv.reader(csvfile, delimiter=',')

        if has_multiple_columns:
            for row in filereader:
                altitude_x1.append(row[0]) # represents x index
                altitude_val.append(row[1]) # second row represents the altitude data

            generate_altitude_plot(has_header=0)
        else: # has single column, no header
            for row in filereader:
                altitude_val.append(row[0]) # this is just for confirmation using different altitude data

            generate_altitude_plot(has_header=1)


'''
normal operation 
use file with 2 columns, second column representing the altitude values
'''

#============  use this for file with a single column ============
# open_csv("altitude_log.csv")
# generate_altitude_plot(has_header=1)

#============ use this for file with multiple columns ============
# open_csv("altitude_data_matlab.csv")
# generate_altitude_plot(has_header=0)

# menu options
menu_options = {
    1: "Generate altitude plot data",
    2: "Generate flight state log from simulated data",
    3: "Exit"
}

def run_console():

    i = 100
    header = "NAKUJA INTEGRATED TESTING SYSTEM"
    subheader = "TEST DATA VERIFICATION TOOL v1.0"
    a = len(header)
    b = i-a

    print("="*int(b/2)+ " " +header+ " "+ "="*int(b/2))
    print(" "*int(b/2)+ " " +subheader+ " "+ " "*int(b/2))
    print("=" * i)
    print("Choose an item from the menu below:")

    # show menu
    for n, option in menu_options.items():
        print(str(n) + "." + option)

    choice = eval(input(">"))

    if choice == 1:
        # TODO: check if filename is valid, if it is a csv file
        f = input("Enter filename: ")
        open_csv(f)

    elif choice == 2:
        print("Choice 2")
    elif choice == 3:
        print("Choice 3")



if __name__ == "__main__":
    run_console()
