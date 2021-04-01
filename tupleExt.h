//
// Created by rey on 12/21/20.
//

#ifndef CSVPARSER_TUPLEEXT_H
#define CSVPARSER_TUPLEEXT_H

#include <iostream>
#include <tuple>
#include <string>

// рекурсивный принт
template<class Ch, class Tr, class Tuple, std::size_t N>
struct TuplePrinter
{
    static void print(std::ostream& outstream, const Tuple& tuple)
    {
        TuplePrinter<Ch, Tr, Tuple, N-1>::print(outstream, tuple);
        outstream << ", " << std::get<N-1>(tuple);
    }
};

// дно рекурсии
template<class Ch, class Tr, class Tuple>
struct TuplePrinter<Ch, Tr, Tuple, 1>
{
    static void print(std::basic_ostream<Ch, Tr> &outstream, const Tuple& tuple)
    {
        outstream << std::get<0>(tuple);
    }
};

template<typename Ch, typename Tr, typename... Args>
std::ostream& operator<<(std::basic_ostream<Ch, Tr>& outStream, const std::tuple<Args...>& tuple)
{
    std::cout << "(";
    TuplePrinter<Ch, Tr, decltype(tuple), sizeof...(Args)>::print(outStream, tuple);
    std::cout << ")";
}

template<class Ch, class Tr, class Head, class... Tail>
class TupleReader
{
public:
        static std::tuple<Head, Tail...> read(std::basic_istream<Ch, Tr> &inputStream, size_t columnNumber = 1)
        {
            if (inputStream.eof())
            {
                throw std::invalid_argument("Not enough data!");
            }

            Head data;
            inputStream >> data;
            if (inputStream.fail())
            {
                throw std::runtime_error("Failed to read data");
            }
            return std::tuple_cat(std::make_tuple(data), TupleReader<Ch, Tr, Tail...>::read(inputStream, columnNumber + 1));
        }
};

template<class Ch, class Tr, class Head>
class TupleReader<Ch, Tr, Head>
{
public:
    static std::tuple<Head> read(std::basic_istream<Ch, Tr> &inputStream, size_t columnNumber = 1)
    {
        if (inputStream.eof())
        {
            throw std::invalid_argument("Not enough data!");
        }

        Head data;
        inputStream >> data;
        if (inputStream.fail())
        {
            throw std::runtime_error("Wrong argument type!");
        }

        return std::make_tuple(data);
    }
};

template<class Ch, class Tr, class... Args>
std::ifstream &operator>>(std::ifstream &inputStream, std::tuple<Ch, Tr, Args...> &tuple)
{
    size_t columnNumber {1};
    tuple = TupleReader<Ch, Tr, Args...>::read(inputStream, columnNumber);
    return inputStream;
}


#endif //CSVPARSER_TUPLEEXT_H
