#include "string.hpp"

        String::String () {
            size_ = 0;
            capacity_ = 0;
            data_ = nullptr;
        }
        String::String (int size, const char symbol) : size_(size), capacity_(size) {
            if (size != 0) {
                data_ = new char[size + 1];
                memset(data_, symbol, size);
                data_[size] = '\0';
            }
        }

        String::String (const char* s, size_t size) {
            if (size != 0){
                data_ = new char[size + 1];     
                memmove(data_, s, size);
                data_[size] = '\0';
                size_ = size;
                capacity_ = size;
            }
            
        }

        String::String (const char* s) : size_(strlen(s)), capacity_(strlen(s))
        {   
            data_ = new char[size_ + 1];     
            memmove(data_, s, size_);
            data_[size_] = '\0';
        }

        String::String (const String&b) :size_(b.size_), capacity_(b.capacity_) {
            if (b.data_ != nullptr){
                data_ = new char[size_ + 1];
                memmove(data_, b.data_, size_);
                data_[size_] = '\0';
            }
            else{
                String();
            }
        }
        String::~String () {
            delete[] data_;
            
        }
        String& String::operator= (const String& b) {
            if (*this == b)
                return *this; 
            Reserve(b.capacity_);
            size_ = 0;
            for (size_t i = 0; i < b.size_; ++i) {
                PushBack(b[i]);
            }
            data_[b.size_] = '\0';
            ShrinkToFit();
            return *this;
        }


        char& String::operator[] (int i) {
            return data_[i];
        }
        const char& String::operator[] (int i) const{
            return const_cast<const char&>(data_[i]);
        }


        char& String::Front() {
            return data_[0];
        }
        const char& String::Front() const{
            return const_cast<const char&>( (const_cast<String*>(this))->Front() );
            //return data_[0];
        }
        char& String::Back() {
            return data_[size_ - 1];
        }
        const char& String::Back() const{
            return data_[size_ - 1];
        }


        const char* String::CStr() const{
            // char* data = new char[size_ + 1];
            // memmove(data, data_, size_ + 1);
            if (size_ == 0){
                return nullptr;
            }
            return const_cast<const char*>(data_);
        }
        const char* String::Data() const {
            if (size_ == 0) {
                return nullptr;
            }
            return const_cast<const char*>(data_);
        }


        bool String::Empty() const {
            if (size_ == 0)
                return true;
            return false;    
        }


        size_t String::Size() const{
            return size_;
        } 
        size_t String::Length() const{
            return size_;
        }

        size_t String::Capacity() const{
            return capacity_;
        } 

        void String::Clear() {
            size_ = 0;
        }

        void String::Swap(String& b) {
            std::swap(data_, b.data_);
            std::swap(size_, b.size_);
            std::swap(capacity_, b.capacity_);
        }

        void String::PopBack() {
            if (size_ > 0) {
                size_--;
                data_[size_] = '\0';
            }
            
        }
        void String::PushBack(const char symbol) {
            if (size_ >= capacity_){
                if (capacity_ == 0)
                    String::Reserve(1);
                else
                    String::Reserve(2 * capacity_);
            }
            data_[size_] = symbol;
            data_[size_ + 1] = '\0';
            size_++;
        }

        String& String::operator+= (const String& b) {
            if (size_ + b.size_ > capacity_) {
                // capacity_ = size_ + b.size_;
                // char* n = new char[capacity_ + 1];
                // memmove(n, data_, size_);
                // delete[] data_;
                // data_ = n;
                String::Reserve(std::max(size_, b.size_) * 2);
            }
            memmove(data_ + size_, b.data_, b.size_);
            size_ += b.size_;
            data_[size_] = '\0';
            return *this;
        }

        void String::Resize(size_t new_size, char symbol) {
            if (new_size > capacity_) {
                // capacity_ = new_size;
                // char* n = new char[capacity_ + 1];
                // memmove(n, data_, size_ + 1);
                // delete[] data_;
                // data_ = n;
                String::Reserve(new_size);
                if (data_ != nullptr){
                    memset(data_ + size_, symbol, new_size - size_);
                    data_[new_size] = '\0';
                }
            }
            else{
                if (new_size > size_) {

                    memset(data_ + size_, symbol, new_size - size_);
                    data_[new_size] = '\0';
                }
            }
            size_ = new_size;
        }

        void String::Reserve(size_t new_capacity) {
            if (new_capacity > capacity_){
                capacity_ = new_capacity;
                char* n = new char[capacity_ + 1];
                
                if (data_ != nullptr){
                    memmove(n, data_, size_ + 1);
                    delete[] data_;
                    data_ = nullptr;
                }
                data_ = n;
            }
        }
        void String::ShrinkToFit() {
            //Reserve(size_);
            capacity_ = size_;
        }
        String String::operator+ (const String& b) const {
            String a = *this;
            a += b;
            return a;
        }

        bool String::operator== (const String& b) const{
            if (size_ == b.size_) {
                for (size_t i = 0; i < size_; ++i) {
                    if (data_[i] != b.data_[i])
                        return false;
                }
                return true;
            }
            return false;
        }

        int String::operator<=> (const String& b) const {
            int a = strcmp(data_, b.data_);
            if (a > 0)
                return 1;
            if (a < 0)
                return -1;
            return 0;
        }
        // friend std::istream& operator>> (std::istream& , String& );
        // friend std::ostream& operator<< (std::ostream& , const String& );
    


std::istream& operator>> (std::istream& is, String& a) {
    is.getline(const_cast<char*>(a.Data()), a.Size());
    return is;
}
std::ostream& operator<< (std::ostream& os, const String& a) {
    if (a.Data() == nullptr)
        return os;
    for (size_t i = 0; i < a.Size(); ++i) {
        os << a.Data()[i];
    }
    return os;
}
