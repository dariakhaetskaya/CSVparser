
#ifndef CSVPARSER_CSVPARSER_H
#define CSVPARSER_CSVPARSER_H

#include <sstream>
#include <fstream>
#include <algorithm>
#include <vector>
#include "tupleExt.h"

template<typename... Args>
class CSVParser {
private:
    size_t skip;
    std::ifstream& input;
    char columnDelim = ',';
    char lineDelim = '\n';
    std::streampos fileStart;

    void skipLines()
    {
        for (int i = 0; i < skip; ++i)
        {
            std::string skippedLine{ 0 };
            std::getline(input, skippedLine, lineDelim);
        }
    }

public:
    CSVParser(std::ifstream& inputFile, size_t skip = 0)
                        : input{inputFile}, skip{skip}
    {
        skipLines();
        fileStart = input.tellg();
    }

    class TupleIterator {
    private:
        CSVParser& CSVparser;
        std::tuple<Args...> tupleIterator;
        bool lastIterator;
        size_t lineNumber;
        std::streampos currentPos;

        class NewCType : public std::ctype<char> {
        private:
            mask chart[table_size];

        public:
            explicit NewCType(char delimiter, size_t refs = 0)
                        : std::ctype<char>{&chart[0], false, refs}
            {
                std::copy_n(classic_table(), table_size, chart);
                chart[' '] = lower;
                chart[delimiter] = space;
            }
        };

        void readTuple()
        {
            if (CSVparser.input.eof())
            {
                lastIterator = true;
                return;
            }

            CSVparser.input.clear();
            CSVparser.input.seekg(currentPos);

            std::string currentLine;
            std::getline(CSVparser.input, currentLine, CSVparser.lineDelim);

            currentPos = CSVparser.input.tellg();
            std::istringstream lineParser{currentLine};
            lineParser.imbue(std::locale{ std::locale::classic(), new NewCType{CSVparser.columnDelim} } );
            tupleIterator = TupleReader<char, std::char_traits<char>, Args...>::read(lineParser, lineNumber);
            ++lineNumber;
        }

// помойка
//        void readTupleWithVector()
//        {
//            if (CSVparser.input.eof())
//            {
//                lastIterator = true;
//                return;
//            }
//
//            CSVparser.input.clear();
//            CSVparser.input.seekg(currentPos);
//
//            std::string currentLine;
//            std::getline(CSVparser.input, currentLine, CSVparser.lineDelim);
//
//            currentPos = CSVparser.input.tellg();
//
//            std::vector<std::string> fields{""};
//
//            bool quoteClosed = true;
//            bool deleteNextComma = false;
//            size_t index = 0;
//            std::string buffer{0};
//
//            for(char c : currentLine){
//                if (deleteNextComma){
//                    deleteNextComma = false;
//                    break;
//                }
//                if(quoteClosed){
//                    if(c == CSVparser.fieldDelim){
//                        quoteClosed = false;
//                    } else {
//                        if (c == CSVparser.columnDelim){
//                            fields.push_back(buffer);
//                            buffer.clear();
//                        } else {
//                            buffer.push_back(c);
//                        }
//                    }
//                } else {
//                    if(c == CSVparser.fieldDelim){
//                        quoteClosed = true;
//                        fields.push_back(buffer);
//                        buffer.clear();
//                        deleteNextComma = true;
//                    } else {
//                        buffer.push_back(c);
//                    }
//                }
//            }
//
//            if (!quoteClosed){
//                throw std::logic_error("field delimeter not closed");
//            }
//            tupleIterator = TupleReader<char, std::char_traits<char>, Args...>::read(fields, lineNumber);
//            ++lineNumber;
//        }

    public:
        TupleIterator(CSVParser& parser, bool isLastIterator)
        : CSVparser{parser}, lastIterator{isLastIterator}, lineNumber{parser.skip + 1}, currentPos{parser.fileStart}
        {
            if (isLastIterator)
            {
                return;
            }
            readTuple();
        }

        bool operator!=(const TupleIterator& other) const
        {
            return lastIterator != other.lastIterator;
        }

        TupleIterator& operator++()
        {
            readTuple();
            return *this;
        }

        std::tuple<Args...> operator*() const
        {
            return tupleIterator;
        }

        ~TupleIterator() = default;
    };

    TupleIterator begin()
    {
        input.clear();
        input.seekg(fileStart);
        return TupleIterator{ *this, false };
    }

    TupleIterator end()
    {
        return TupleIterator{ *this, true };
    }

    void changeDelims(char column, char line)
    {
        columnDelim = column;
        lineDelim = line;
    }

};

#endif //CSVPARSER_CSVPARSER_H
