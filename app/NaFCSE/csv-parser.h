#ifndef CSVPARSER_H
#define CSVPARSER_H

#include <vector>
#include <sstream>
#include <iostream>
#include <string>

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

    std::string operator[](std::size_t index) const;

    std::size_t size() const;
    void readNextRow(std::istream& str);
};

std::vector<std::string> getNextLineAndSplitIntoTokens(std::istream& str);
std::istream& operator>>(std::istream& str, CSVRow& data);



#endif // CSVPARSER_H
