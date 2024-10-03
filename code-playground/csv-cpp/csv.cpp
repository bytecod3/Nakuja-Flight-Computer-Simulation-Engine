#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

std::vector<std::string> getNextLineAndSplitIntoTokens(std::istream& str) {
    std::vector<std::string> result;
    std::string line;
    std::getline(str, line);

    std::stringstream linestream(line);
    std::string cell;

    while (std::getline(linestream, cell, ',')) {
        result.push_back(cell);
    }

    // check fro trailing comma with no data after it
    if(!linestream && cell.empty()) {
        // if there is a trailing comma then add an empty element
        result.push_back("");
    }

    return result;
}

/**
 * A class representing a row 
 */
class CSVRow
{
private:
    /* data */
    std::string m_line;
    std::vector<int> m_data;
public:

    std::string operator[](std::size_t index) const {
        return std::string(&m_line[m_data[index] + 1], m_data[index + 1] -  (m_data[index] + 1));
    }

    std::size_t size() const {
        return m_data.size() - 1;
    }

    void readNextRow(std::istream& str) {
        std::getline(str, m_line);
        
        m_data.clear();
        m_data.emplace_back(-1);
        std::string::size_type pos = 0;

        while((pos = m_line.find(',', pos)) != std::string::npos) {
            m_data.emplace_back(pos);
            ++pos;
        }

        // check for trailing comma with no data after it
        pos = m_line.size();
        m_data.emplace_back(pos);
    }
};

std::istream& operator>>(std::istream& str, CSVRow& data) {
    data.readNextRow(str);
    return str;
}


int main() {
    std::ifstream file("sample_100.csv");

    std::vector<float> decimal;
    std::vector<int> points;
    std::vector<std::string> vals; 

    CSVRow row;

    while (file >> row) {
        vals.push_back( row[2] );
    }

    for(const auto& element : vals) {
        std::cout << element << "\n";
    }

    return 0;   
    
}
