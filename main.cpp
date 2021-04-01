#include "csvparser.h"
#include "tupleExt.h"

int main() {
    std::ifstream file{ "test.csv" };
    if (file.fail()) {
        std::cout << "The file could not be opened." << std::endl;
        return 0;
    }

    CSVParser<size_t, std::string, int, std::string, double> CSVparser{ file, 0 };

    CSVparser.changeDelims(',','\n');

    try {
        for (auto tuple : CSVparser) {
            std::cout << tuple << std::endl;
        }
    } catch (& e) {
        std::cout << e.what() << "Line: " << e.getLineNumber() << "." << std::endl;
    } catch (FailedToReadData& e) {
        std::cout << e.what() << "Line: " << e.getLineNumber() << ". Column: " << e.getColumnNumber() << "." << std::endl;
    }

    file.close();
    return 0;
}
