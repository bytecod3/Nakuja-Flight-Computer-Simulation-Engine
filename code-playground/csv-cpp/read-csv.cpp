#include <string>
#include <fstream>
#include <vector>
#include <utility> // std::pair
#include <stdexcept>  // std::runtime_error
#include <sstream> // std::stringstream
#include <iostream>

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

// function to read the names 
std::vector<std::pair<std::string, std::vector<std::string>>> read_csv_names(std::string filename) {
    // Reads a CSV file into a vector of <string, vector<string>> pairs where each pair 
    // represents a  <column name, column values>

    // create a vector of <string, int vector> pairs to store the result 
    std::vector< std::pair<std::string, std::vector<std::string> > > result;

    // create an input file stream 
    std::ifstream myFile(filename);

    // make sure the file is open
    if(!myFile.is_open()) {
        throw std::runtime_error("COULD NOT OPEN FILE");

    }

    // helper vars 
    std::string line, colname;
    std::string val;

    // read the column names 
    if(myFile.good()) {
        // extract the first line in the file
        std::getline(myFile, line);

        // create a stringstream from line 
        std::stringstream ss(line);

        // extract each column name 
        while (std::getline(ss, colname, ',')){
            // initialize and add <colname, int vector> pairs to result  
            result.push_back({colname, std::vector<std::string> {} } );
        }

    }

    // read data, line by line 
    while (std::getline(myFile, line))
    {
        // create a stringstream of the current line 
        std::stringstream ss(line);

        // keep track of the current column index
        int colIdx = 0;

        // extract each data value 
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
    using column = std::pair<std::string, std::vector<int> >;
    using names_col = std::pair<std::string, std::vector<std::string> >;

    std::vector<column> first_column = read_csv("sample_100.csv");    // first column
    std::vector<names_col> second_column = read_csv_names("sample_100.csv"); // second column
    std::vector<column> third_column = read_csv("sample_100.csv");  // third column

    // std::vector<column> ones = read_csv("ones.csv");

    // std::cout<< "Vector elements (ones)" << "\n";

    // std::cout << "Rows: " << first_column.at(0).second.size() << ", columns: " << first_column.size() << std::endl;

    // READ THE FIRST COLUMN
    // std::cout << "ITEM NUMBER\n";

    // std::cout << first_column.at(0).first << "\n"; // column name

    // std::cout << "Values\n";

    // for (const auto& element : first_column.at(0).second) { // column items 
    //     std::cout << element << std::endl;
    // }

    // READ THE SECOND COLUMN
    std::cout << "\nITEM NUMBER\n";

    std::cout << second_column.at(1).first << "\n"; // column name

    std::cout << "\nVALUES\n";

    for (const auto& element : second_column.at(1).second) { // column items 
        std::cout << element << std::endl;
    }

    std::cout << second_column.at(1).second[0] << std::endl;
    

    // std::string line;
    // std::ifstream fin;
    // fin.open("threee_cols.csv");

    // if(!fin.is_open()) {
    //     std::cout << "Error opening file" << std::endl;
    // }

    // while (fin.is_open()) {
    //     for(int i=0; i <10; i++) {
    //         getline(fin, line);

    //         std::string col1, col2, col3;

    //         getline(fin, col1, ',');
    //         getline(fin, col2, ',');
    //         getline(fin, col3, ',');

    //         std::cout<< col1 << std::endl;
    //         std::cout<< col2 << std::endl;
    //         std::cout<< col3 << std::endl;

    //     }

    //     fin.close();
    // }
    
    return 0;
}
    

