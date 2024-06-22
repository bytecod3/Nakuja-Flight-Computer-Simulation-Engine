#include <string>
#include <fstream>
#include <vector>
#include <utility> // std::pair
#include <stdexcept>  // std::runtime_error
#include <sstream> // std::stringstream

std::vector<std::pair<std::string, std::vector<int>>> read_csv(std::string filename) {
    // Reads a CSV file into a vector of <string, vector<int>> pairs where each pair 
    // represents a  <column name, column values>

    // create a vector of <string, int vector> pairs to store the result 
    std::vector<std::pair<std::string, std::vector<int>>> result;

    // create an input file stream 
    std::ifstream myFile(filename);

    // make sure the file is open
    if(!myFile.is_open()) {
        throw std::runtime_error("COULD NOT OPEN FILE");

    }

    // helper vars 
    std::string line, colname;
    int val;

    // read the column names 
    if(myFile.good()) {
        // extract the first line in the file
        std::getline(myFile, line);

        // create a stringstream from line 
        std::stringstream ss(line);

        // extract each column name 
        while (std::getline(ss, colname, ',')){
            // initialize and add <colname, int vector> pairs to result  
            result.push_back({colname, std::vector<int> {}} );
        }

    }

    // read data, line by line 
    while (std::getline(myFile, line))
    {
        // create a stringstream of the current line 
        std::stringstream ss(line);

        // keep track of the current column index
        int colIdx = 0;

        // extract each integer 
        while(ss >> val)  {
            result.at(colIdx).second.push_back(val);

            // if the next token is a comma, ignore it and move on
            if(ss.peek() == ',') ss.ignore();

            // increment the column index 
            colIdx++;
        }

    }

     // close the file 
    myFile.close();

    return result;
    
}


void write_csv( std::string filename, std::vector<std::pair<std::string, std::vector<int>>> dataset) {
    std::ofstream myFile(filename);

    // send column names to the stream 
    for (int j = 0; j < dataset.size(); j++) {
        myFile << dataset.at(j).first;
        if(j != dataset.size() - 1) {
            myFile << ","; // no comma at the end of line 
        }
    }

    myFile << "\n";

    // send data to the stream 
    for(int i=0; i < dataset.at(0).second.size(); i++ ) {
        for (int j = 0; j < dataset.size(); j++) {
            myFile << dataset.at(j).second.at(i);
            if(j != dataset.size() - 1) {
                myFile << ","; // no comma at end of line
            }
        }

        myFile << "\n";
    }

    

    // close the file 
    myFile.close();   

}



int main() {
    // Read three_cols.csv and ones.csv
    std::vector<std::pair<std::string, std::vector<int>>> three_cols = read_csv("threee_cols.csv");
    std::vector<std::pair<std::string, std::vector<int>>> ones = read_csv("ones.csv");

    // Write to another file to check that this was successful
    write_csv("threee_cols_copy.csv", three_cols);
    write_csv("ones_copy.csv", ones);
    
    return 0;
}
    

