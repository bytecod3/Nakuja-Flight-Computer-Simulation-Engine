#include <fstream>
#include <string>
#include <vector>

// void write_csv(std::string filename, std::string colname, std::vector<int> vals) {
//     std::ofstream myFile(filename);
//     myFile << colname << "\n";
    
//     // send data to the stream 
//     for(int i=0; i < vals.size(); i++) {
//         myFile << vals.at(i) << "\n";
//     }

//     myFile.close();

// }

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
    
    std::vector<int> vec1(100, 1);
    std::vector<int> vec2(100, 2);
    std::vector<int> vec3(100, 3);

    // wrap into a vector
    std::vector<std::pair<std::string, std::vector<int>>> vals = {{"one", vec1}, {"Two", vec2}, {"Three", vec3}};

    write_csv("threee_cols.csv", vals);

    return 0; 
}
