#ifndef _DEFINED_TYPES_HPP
#define _DEFINED_TYPES_HPP

#include <string>
#include <cstring>
#include <iostream>

struct Defined{
    std::string name{};

    Defined(){}

    void initValue(const std::string& given_name){
        name = given_name;
    }

    std::size_t encode_into_buffer(char* byte_buffer, std::size_t size_of_buffer){

        std::size_t total_size = 
            (sizeof(size_t) + name.size());

        if (total_size > size_of_buffer){
            std::cout << "size is not enough\n";
            return 0;
        }

        std::size_t current_offset = 0;

        /* void* memcpy(void* destination, const void* source, std::size_t num); */

        std::size_t size_of_string = name.size();
        std::memcpy(byte_buffer + current_offset, &size_of_string, sizeof(std::size_t));
        current_offset += sizeof(std::size_t);

        std::memcpy(byte_buffer + current_offset, name.c_str(), name.size());
        current_offset += name.size();

        if(current_offset != total_size){

            std::cout << "size is unmatched\n";
            return 0;
        }

        return total_size;
    }

    std::size_t decode_from_buffer(char* byte_buffer){

        std::size_t current_offset = 0;
        
        /* void* memcpy(void* destination, const void* source, std::size_t num); */

        std::size_t size_of_string = 0;
        std::memcpy(&size_of_string, byte_buffer + current_offset, sizeof(std::size_t));
        current_offset += sizeof(std::size_t);

        /*
        char temp_char_name1[size_of_string + 1]; // +1 for null character
        std::memcpy(temp_char_name1, byte_buffer + current_offset, size_of_string);
        temp_char_name1[size_of_string] = '\0';
        */

        char temp_char_name[size_of_string + 1]; /* +1 for null character */
        std::memcpy(temp_char_name, byte_buffer + current_offset, size_of_string);
        current_offset += size_of_string;
        temp_char_name[size_of_string] = '\0'; /* IMP */
        std::string temp_string_name(temp_char_name);
        name = temp_string_name;

        return current_offset;
    }

    void print_data(){
        std::cout << "name: " << name << "\n";
    }
};


#endif