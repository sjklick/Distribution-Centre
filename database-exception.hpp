#ifndef __DATABASE_EXCEPTION__
#define __DATABASE_EXCEPTION__

#include <string>
#include <exception>

class DatabaseException : public std::exception {
private:
    std::string msg;
public:
    DatabaseException(std::string msg) { this->msg = msg; }
    std::string message() { return msg; }
};

#endif