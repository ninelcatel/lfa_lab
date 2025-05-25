#include <iostream>
#include <vector>
#include <algorithm>

// Function to flip (reverse) the sub-array arr[0...i]
void flip(std::vector<int>& arr, int i) {
    std::reverse(arr.begin(), arr.begin() + i + 1);
}

// Function to find the index of the maximum element in arr[0...n-1]
int findMaxIndex(const std::vector<int>& arr, int n) {
    auto it = std::max_element(arr.begin(), arr.begin() + n);
    return std::distance(arr.begin(), it);
}
// Helper function to print the array
void printArray(const std::vector<int>& arr) {
    for (size_t i = 0; i < arr.size(); ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
}
void pancakeSort(std::vector<int>& arr) {
    int n = arr.size();

    // Start from the complete array size and gradually reduce it by one
    for (int current_size = n; current_size > 1; --current_size) {
        // Find the index of the maximum element in the unsorted sub-array arr[0...current_size-1]
        int max_index = findMaxIndex(arr, current_size);

        if (max_index != current_size - 1) {
            // 1. Bring the maximum element to the front 
            if (max_index != 0) {
                flip(arr, max_index);
            }

            // 2. Move the maximum element (now at the front) to its correct position.
            flip(arr, current_size - 1);
        }
    }
    ::printArray(arr);

}



// The main function to test the code
int main() {
    int n;
    std::vector<int> array;
    std::cout<<"Input the amount of elements in the array: ";
    std::cin>>n;
    std::cout<<std::endl;
    std::cout<<"Input the (integer) elements of the vector: ";
    for(int i = 0; i < n; i++){
        int aux;
        std::cin>>aux;
        array.push_back(aux);
    }
    std::cout<<std::endl;
    std::cout << "------------------------------------"<<std::endl;
    std::cout << "Initial array: ";
    printArray(array);
    std::cout << "------------------------------------"<<std::endl;
    std::cout << "Sorted array: ";
    pancakeSort(array);
    

    return 0;
}
