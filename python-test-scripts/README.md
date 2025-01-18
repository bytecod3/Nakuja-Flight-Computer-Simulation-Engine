## NAKUJA INTEGRATED FLIGHT COMPUTER TESTING SYSTEM 
### TEST DATA VERIFICATION TOOL v1.0

### Purpose 
This is a python console tool used as a bridge between simulation data from MATLAB and 
the C++ simulation GUI interface. 
The purpose of this tool is to verify the test data by generating graphs and generating 
flight state machine log file. 
The tool allows the user to detect various states of flight and verify that 
indeed the data from MATLAB generates the needed states.  
Following this, this tool can generate the flight state log that can then be analyzed
by the tester. 

The main test data used here is the altitude data, which we heavily rely on to 
detect the flight states.

### Requirements 
Make sure you have installed the following in your PC
``` python
1. Python >3.10
2. Matplotlib
3. Numpy
```

### Test data preparation 
This tool requires test data to be used as intended. To prepare the data, follow the steps 
below: 
1. Copy the ```altitude_log.csv``` and ```altitude_data_matlab.csv``` files from the ```matlab-data-generators``` folder and 
paste into ```python-test-scripts``` folder. These two files are essential for 
carrying out the test. 

|File | Description                                                                                                                                                  |
|---|--------------------------------------------------------------------------------------------------------------------------------------------------------------| 
|```altitude_log.csv```| Contains a single column that represents altitude data                                                                                                       |
|```altitude_data_matlab.csv```| Might contain multiple columns, with the ***First column*** representing the row index, the second column ***MUST*** represent the altitude values in meters |

NOTE: Please make sure the files are named exactly as described above

### Usage 
Follow the steps below to run the tool:
1. Open a console inside the ```python-test-scripts``` folder
2. Run the following command in the console  
``` python main.py```. The following console will appear.

3. Select the required option and follow the displayed prompts. 

### Data analysis
The data produced by the tool is two-fold:
- Graphical data
- State log file 

You can save the graphs as images for later usage.  
For the flight state file, open the ```flight_test_file.txt``` and verify that the 
flight states are in sequence as intended.

### How apogee is detected

We employ a ring buffer data structure to detect the apogee. It works as defined below:
- Store 5 successive values in a ring buffer
- While consuming the data, keep checking if the oldest value and the current value have a 
positive difference. 
- If difference is positive, we are still ascending 
- If difference is negative, then we are descending. Register this at the first as apogee the first time it 
is detected

### References and additional resources 


