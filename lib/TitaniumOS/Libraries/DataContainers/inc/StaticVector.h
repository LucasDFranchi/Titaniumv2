#ifndef STATIC_VECTOR_H
#define STATIC_VECTOR_H

#include <iostream>
#include <memory>
#include <vector>

/**
 * @brief A fixed-size vector implementation using std::vector.
 *
 * This class provides a fixed-size vector using std::vector with additional size tracking.
 *
 * @tparam T The type of elements stored in the vector.
 */
template <typename T>
class StaticVector {
   public:
    /**
     * @brief Constructor to initialize the StaticVector with a given size.
     *
     * @param size The maximum size of the vector.
     */
    StaticVector(size_t size) {
        this->_internal_vector = std::make_unique<std::vector<T>>(size);
        this->_maximum_size    = size;
    }

    /**
     * @brief Adds an element to the vector.
     *
     * @param[in] data The element to be added.
     * @return 0 if successful, -1 if the vector is full.
     */
    int push_back(const T &data) {
        int result = -1;

        if (_size < _maximum_size) {
            _internal_vector->push_back(data);
            this->_size++;
            result = 0;
        }
        return result;
    }

    /**
     * @brief Returns the last element of the vector.
     *
     * @return The last element of the vector.
     */
    T back(void) {
        return this->_internal_vector->back();
    }

    T get(uint32_t index) {
        return this->_internal_vector->get(index);
    }

    T erase(uint32_t index) {
        return this->_internal_vector->get(index);
    }

    /**
     * @brief Removes the last element of the vector.
     */
    void pop_back(void) {
        this->_size--;
        this->_internal_vector->pop_back();
    }

    size_t size(void) const {
        return this->_size;
    }

   private:
    std::unique_ptr<std::vector<T>> _internal_vector = nullptr;  ///< Internal vector storage.
    size_t _size                                     = 0;        ///< Current size of the vector.
    size_t _maximum_size                             = 0;        ///< Maximum size of the vector.
};

#endif /* STATIC_VECTOR_H */
