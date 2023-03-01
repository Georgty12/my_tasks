#pragma once
#include <cstring>
#include <iostream>
class String {
    public:


        String ();
        String (int size, const char symbol);

        String (const char* s, size_t size);

        String (const char* s);

        String (const String&b);
        ~String ();
        String& operator= (const String& b);
        

        char& operator[] (int i);
        const char& operator[] (int i) const;


        char& Front();
        const char& Front() const;
        char& Back() ;
        const char& Back() const;

        const char* CStr() const;
        const char* Data() const;


        bool Empty() const;


        size_t Size() const;
        size_t Length() const;
        size_t Capacity() const;

        void Clear() ;

        void Swap(String& b);

        void PopBack() ;
        void PushBack(const char symbol);

        String& operator+= (const String& b);

        void Resize(size_t new_size, char symbol);

        void Reserve(size_t new_capacity);
        
        void ShrinkToFit();
        String operator+ (const String& b) const;

        bool operator== (const String& b) const;


        int operator<=> (const String& b) const;
        
        // friend std::istream& operator>> (std::istream& , String& );
        // friend std::ostream& operator<< (std::ostream& , const String& );
    private:
        char* data_ = nullptr;
        size_t size_ = 0;
        size_t capacity_ = 0;
};

std::istream& operator>> (std::istream& is, String& a);
std::ostream& operator<< (std::ostream& os, const String& a);