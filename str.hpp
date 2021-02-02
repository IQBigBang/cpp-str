#ifndef STR_H
#define STR_H

#include <string>
#include <iostream>
#include <cstdlib>
#include <cstring>

/**
 * \brief A thin wrapper around an immutable string
 *
 * An alternative to std::string which (to me) has complicated
 * semantics regarding memory management and copy/move semantics.
 *
 * A `Str` object contains a constant character array pointer (C-like string),
 * which it owns and will therefore free once not used.
 * It is reference-counted and therefore cheaply copyable.
 */
class Str
{
    public:
        /**
         * @brief Creates an uninitialized Str object.
         * Most methods will probably fail if called on this object.
         */
        inline Str() : ptr(nullptr), counter(nullptr) {}

        /**
         * @brief Initializes the object from a std::string object.
         */
        inline explicit Str(const std::string& str) : Str(str.c_str(), str.size()) {}

        /**
         * @brief Initializes the object with a c-style string.
         * The string is copied into a newly-allocated memory area based on its length.
         * The pointer must not be a null pointer.
         */
        explicit Str(const char* str, size_t length) : counter(new int)
        {
            char* strin = new char[length + 1];
            memcpy(strin, str, length);
            strin[length] = 0;

            ptr = strin;
            *counter = 1;
        }

        /**
         * Copy constructor
         */
        Str(const Str& other) : ptr(other.ptr), counter(other.counter)
        {
            if (counter != nullptr) (*counter)++; // the other object may be uninitialized
        }

        /**
         * Copy assignment operator
         */
        Str& operator=(const Str& other)
        {
            if (counter != nullptr) (*counter)--; // the original reference is lost
            ptr = other.ptr;
            counter = other.counter;
            if (counter != nullptr) (*counter)++;
            return *this;
        }

        /**
         * Destructor
         */
        ~Str()
        {
            if (counter == nullptr) return;
            (*counter)--;
            if (*counter == 0) {
                delete[] ptr;
                delete counter;
            }
        }

        /**
         * Returns the reference count
         */
        inline int ref_count() const
        {
            if (counter == nullptr) return 0;
            return *counter;
        }

        /**
         * @brief Returns the underlying string pointer.
         * This string must not be manually deallocated/reallocated and
         * should not be modified for the `Str` objects to remain immutable
         *
         * Returns a null pointer if the object isn't initialized.
         */
        inline const char* c_str() const { return ptr; }

        /**
         * Returns true if the object is valid, e.g. if it was NOT created
         * with the empty constructor
         */
        inline bool valid() const { return counter == nullptr; }

        /**
         * @brief Prints the string to the output stream.
         */
        friend std::ostream& operator<<(std::ostream& os, const Str& s)
        {
            if (s.ptr != nullptr) os << s.ptr;
            return os;
        }

        friend bool operator==(const Str& s1, const Str& s2)
        {
            if (s1.ptr == s2.ptr) return true; // this also solves equality of null pointers
            if (s1.ptr == nullptr || s2.ptr == nullptr) return false;
            return strcmp(s1.ptr, s2.ptr) == 0;
        }

        friend bool operator !=(const Str& s1, const Str& s2)
        {
            if (s1.ptr == s2.ptr) return false; // this also solves equality of null pointers
            if (s1.ptr == nullptr || s2.ptr == nullptr) return true;
            return strcmp(s1.ptr, s2.ptr) != 0;
        }

        friend bool operator <(const Str& s1, const Str& s2)
        {
            if (s1.ptr == nullptr && s2.ptr == nullptr) return false;
            if (s1.ptr == nullptr) return true; // nullptr < any-string
            if (s2.ptr == nullptr) return false;
            return strcmp(s1.ptr, s2.ptr) < 0;
        }

        friend bool operator >(const Str& s1, const Str& s2)
        {
            if (s1.ptr == nullptr && s2.ptr == nullptr) return false;
            if (s1.ptr == nullptr) return false; // nullptr < any-string
            if (s2.ptr == nullptr) return true;
            return strcmp(s1.ptr, s2.ptr) > 0;
        }

        friend bool operator <=(const Str& s1, const Str& s2)
        {
            if (s1.ptr == nullptr && s2.ptr == nullptr) return true;
            if (s1.ptr == nullptr) return true; // nullptr < any-string
            if (s2.ptr == nullptr) return false;
            return strcmp(s1.ptr, s2.ptr) <= 0;
        }

        friend bool operator >=(const Str& s1, const Str& s2)
        {
            if (s1.ptr == nullptr && s2.ptr == nullptr) return true;
            if (s1.ptr == nullptr) return false; // nullptr < any-string
            if (s2.ptr == nullptr) return true;
            return strcmp(s1.ptr, s2.ptr) >= 0;
        }

        /**
         * @brief Parses this string as an integer in decimal and returns it
         *
         * Uses the `std::atoi` function
         */
        inline int parse_to_int() const { return std::atoi(ptr); }

    private:
        const char* ptr;
        int* counter;
};

#endif // STR_H
