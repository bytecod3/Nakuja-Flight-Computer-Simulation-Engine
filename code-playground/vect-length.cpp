#include <iostream>
#include <vector>

int main(void) {
	std::vector<int> t;
	
	t.push_back(34);
	t.push_back(22);
	t.push_back(67);

	int len = t.size();

	std::cout << len << std::endl;


	return 0;

}


