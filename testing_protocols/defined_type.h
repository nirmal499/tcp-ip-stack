// #include <array>
#include <vector>
#include <string>
#include <cstring>

struct Defined{
    int x{};
    double y{};
    std::string name{};
    std::vector<int> grade_vec{};

    Defined(){}

    void initValue1(){
        x = 34;
        y = 56.78;
        name = "Nirmal";

        grade_vec.push_back(54);
        grade_vec.push_back(67);
        grade_vec.push_back(8);
        grade_vec.push_back(19);
        grade_vec.push_back(52);
    }

    std::size_t encode_into_buffer(char* byte_buffer, std::size_t size_of_buffer){

        std::size_t total_size = 
            sizeof(int) + 
            sizeof(double) + 
            (sizeof(size_t) + name.size()) + 
            (sizeof(size_t) + grade_vec.size() * sizeof(int));

        if (total_size > size_of_buffer){
            std::cout << "size is not enough\n";
            return 0;
        }

        std::size_t current_offset = 0;

        /* void* memcpy(void* destination, const void* source, std::size_t num); */
        std::memcpy(byte_buffer + current_offset, &x, sizeof(int));
        current_offset += sizeof(int);

        std::memcpy(byte_buffer + current_offset, &y, sizeof(double));
        current_offset += sizeof(double);

        std::size_t size_of_string = name.size();
        std::memcpy(byte_buffer + current_offset, &size_of_string, sizeof(std::size_t));
        current_offset += sizeof(std::size_t);

        std::memcpy(byte_buffer + current_offset, name.c_str(), name.size());
        current_offset += name.size();

        std::size_t whole_size_of_vec = grade_vec.size() * sizeof(int); /* vec<T>.size() * sizeof(T) */
        std::memcpy(byte_buffer + current_offset, &whole_size_of_vec, sizeof(std::size_t));
        current_offset += sizeof(std::size_t);

        std::memcpy(byte_buffer + current_offset, grade_vec.data(), whole_size_of_vec);
        current_offset += whole_size_of_vec;

        if(current_offset != total_size){

            std::cout << "size is unmatched\n";
            return 0;
        }

        return total_size;
    }

    std::size_t decode_from_buffer(char* byte_buffer){

        std::size_t current_offset = 0;
        
        /* void* memcpy(void* destination, const void* source, std::size_t num); */
        std::memcpy(&x, byte_buffer + current_offset, sizeof(int));
        current_offset += sizeof(int);

        std::memcpy(&y, byte_buffer + current_offset, sizeof(double));
        current_offset += sizeof(double);

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
        std::string temp_string_name(temp_char_name);
        name = temp_string_name;

        std::size_t whole_size_of_vec = 0;
        std::memcpy(&whole_size_of_vec, byte_buffer + current_offset, sizeof(std::size_t));
        current_offset += sizeof(std::size_t);

        // if(size_of_arr != grade_vec.size()){
        //     std::cout << "array size not matched\n";
        //     return 0;
        // }

        std::size_t no_of_elements_in_vec = whole_size_of_vec / sizeof(int); 
        grade_vec.reserve(no_of_elements_in_vec);

        std::memcpy(grade_vec.data(), byte_buffer + current_offset, whole_size_of_vec);
        current_offset += whole_size_of_vec;

        std::cout << "grade_vec[0] : " << grade_vec[0] << "\n";
        std::cout << "grade_vec[1] : " << grade_vec[1] << "\n";
        std::cout << "grade_vec[2] : " << grade_vec[2] << "\n";
        std::cout << "grade_vec[3] : " << grade_vec[3] << "\n";
        std::cout << "grade_vec[4] : " << grade_vec[4] << "\n";

        return current_offset;
    }
};

void print_data(Defined* defined_data){
    std::cout << "x: " << defined_data->x << "\n";
    std::cout << "y: " << defined_data->y << "\n";
    std::cout << "name: " << defined_data->name << "\n";

    std::cout << "grade_vec[0] : " << defined_data->grade_vec[0] << "\n";
    std::cout << "grade_vec[1] : " << defined_data->grade_vec[1] << "\n";
    std::cout << "grade_vec[2] : " << defined_data->grade_vec[2] << "\n";
    std::cout << "grade_vec[3] : " << defined_data->grade_vec[3] << "\n";
    std::cout << "grade_vec[4] : " << defined_data->grade_vec[4] << "\n";
    
    std::cout << "\n";
}