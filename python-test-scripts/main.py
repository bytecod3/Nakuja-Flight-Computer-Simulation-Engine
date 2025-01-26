import csv
import matplotlib.pyplot as plt
from pandas.io.stata import stata_epoch

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

max_alt = 0  # the maximum altitude value recognized from the file 
apogee_flag = 0 # show that apogee has been detected 
main_eject_flag = 0
apogee = 0 # determine the altitude at which apogee was detected
state_log_file = "state_log_file.txt"

LAUNCH_DETECTION_THRESHOLD = 10 # below 5 meters we have not launched

states = {
    1:"PREFLIGHT",
    2:"POWERED_FLIGHT",
    3:"APOGEE",
    4:"DROGUE EJECT",
    5:"DROGUE_DESCENT",
    6:"MAIN_EJECT",
    7:"MAIN_DESCENT",
    8:"POST_FLIGHT",
}

def check_flight_states(altitude_values, has_header=0):
    """
    Check flight states based on the test data
    """
    # state machine
    global max_alt
    global apogee_flag
    global main_eject_flag
    altitude_values_float = []

    state_log = open(state_log_file, 'w').close()

    if has_header:
        altitude_values = altitude_values[1:]
        l = len(altitude_values[1:])
        # convert from string to float
        for j in range(0, l):
            altitude_values_float.append(float(altitude_values[j]))

    else: # no header
        l = len(altitude_values)
        # convert from string to float
        for j in range(0, l):
            altitude_values_float.append(float(altitude_values[j]))

    for alt_index in range(0, len(altitude_values_float)):
        state_log = open(state_log_file, 'a')

        if apogee_flag == 0:
            # detect PREFLIGHT 
            if altitude_values_float[alt_index] < LAUNCH_DETECTION_THRESHOLD:
                state_log.write(states[1]) # append flight state to file
                state_log.write("\n")

            # detect POWERED FLIGHT 
            elif  LAUNCH_DETECTION_THRESHOLD < altitude_values_float[alt_index] < LAUNCH_DETECTION_THRESHOLD + 20:
                state_log.write(states[2]) # append flight state to file
                state_log.write("\n")

            # detect APOGEE
            if altitude_values_float[alt_index] > max_alt:
                max_alt = altitude_values_float[alt_index]

            if (max_alt - float(altitude_values_float[alt_index])) > 3:
                apogee = max_alt
                apogee_flag = 1
                print("Apogee detected at " + str(apogee))
                state_log.write(states[3]) # append flight state to file
                state_log.write("\n")  

                # DROGUE EJECT
                state_log.write(states[4]) # append flight state to file
                state_log.write("\n") 

                # DROGUE DESCENT 
                state_log.write(states[5]) # append flight state to file
                state_log.write("\n") 

            
        # STATES AFTER APOGEE
        elif apogee_flag == 1:
            # detect MAIN CHUTE EJECT
            if LAUNCH_DETECTION_THRESHOLD <= altitude_values_float[alt_index] <= max_alt:
                if main_eject_flag == 0:
                    state_log.write(states[6]) # append flight state to file
                    state_log.write("\n")
                    main_eject_flag = 1
                elif main_eject_flag == 1:
                    # MAIN CHUTE DESCENT
                    state_log.write(states[7]) # append flight state to file
                    state_log.write("\n")

            if altitude_values_float[alt_index] < LAUNCH_DETECTION_THRESHOLD:
                # POST FLIGHT
                state_log.write(states[8]) # append flight state to file
                state_log.write("\n")
    
        state_log.close()


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

    else: # for file with multiple columns, no header
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


def open_csv(filename, check_state=0):
    has_multiple_columns = 0
    # check filename for multiple columns
    file_with_multi_columns = "altitude_data_matlab.csv"
    file_single_columns = "altitude_log.csv"

    if (filename == "altitude_data_matlab.csv") or (filename == "wrong_altitude_data_1.csv") or (filename == "wrong_altitude_data_2_slope.csv") :
        has_multiple_columns = 1
    elif filename == "altitude_log.csv":
        has_multiple_columns = 0

    with open(filename, newline='') as csvfile:
        filereader = csv.reader(csvfile, delimiter=',')

        if has_multiple_columns:
            for row in filereader:
                altitude_x1.append(row[0]) # represents x index
                altitude_val.append(row[1]) # second row represents the altitude data

            # TODO: check usage of multithreading to eliminate this check
            if check_state:
                check_flight_states(altitude_val, has_header=0)
            else:
                generate_altitude_plot(has_header=0)

        else: # has single column, no header
            for row in filereader:
                altitude_val.append(row[0]) # this is just for confirmation using different altitude data

            if check_state:
                check_flight_states(altitude_val, has_header=1)
            else:
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
        # perform state detection
        # TODO: check if filename is valid, if it is a csv file
        f = input("Enter filename: ")
        print("Opening test file...")
        open_csv(f, check_state=1)

    elif choice == 3:
        print("Choice 3")



if __name__ == "__main__":
    run_console()
