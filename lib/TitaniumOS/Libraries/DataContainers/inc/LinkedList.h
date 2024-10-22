/**
 * @file LinkedList.h
 * @brief A simple singly linked list implementation in C++ with FreeRTOS compatibility.
 */

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "SystemProcess/Template/TaskIncludeHelper.h"

/**
 * @brief A simple singly linked list template.
 *
 * @tparam T The type of data stored in the list.
 */
template <typename T>
class LinkedList {
   private:
    /**
     * @brief Node structure for the linked list.
     */
    struct Node {
        T data;      ///< Data stored in the node.
        Node* next;  ///< Pointer to the next node in the list.
    };

    Node* head;               ///< Pointer to the head (first node) of the list.
    Node* tail;               ///< Pointer to the tail (last node) of the list.
    size_t listSize;          ///< Number of elements in the list.
    SemaphoreHandle_t mutex;  ///< Mutex to ensure thread safety for list operations.

   public:
    /**
     * @brief Constructor.
     */
    LinkedList()
        : head(nullptr), tail(nullptr), listSize(0) {
        mutex = xSemaphoreCreateMutex();
        configASSERT(mutex != NULL);  // Check if mutex creation fails
    }

    /**
     * @brief Destructor.
     */
    ~LinkedList() {
        clear();
        vSemaphoreDelete(mutex);
    }

    /**
     * @brief Adds a new element to the end of the list.
     *
     * @param[in] value The value to be added.
     */
    void push_back(const T& value) {
        xSemaphoreTake(mutex, portMAX_DELAY);

        Node* newNode = new Node{value, nullptr};

        if (tail == nullptr) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail       = newNode;
        }

        listSize++;

        xSemaphoreGive(mutex);
    }

    /**
     * @brief Removes the first element from the list.
     */
    void pop_front() {
        xSemaphoreTake(mutex, portMAX_DELAY);

        if (head == nullptr) {
            xSemaphoreGive(mutex);
            return;  // List is empty
        }

        Node* temp = head;
        head       = head->next;
        delete temp;

        if (head == nullptr) {
            tail = nullptr;  // List is now empty
        }

        listSize--;

        xSemaphoreGive(mutex);
    }

    /**
     * @brief Returns the number of elements in the list.
     *
     * @return The number of elements in the list.
     */
    size_t size() const {
        return listSize;
    }

    /**
     * @brief Checks if the list is empty.
     *
     * @return True if the list is empty, false otherwise.
     */
    bool empty() const {
        return (listSize == 0);
    }

    /**
     * @brief Removes all elements from the list.
     */
    void clear() {
        xSemaphoreTake(mutex, portMAX_DELAY);

        while (head != nullptr) {
            Node* temp = head;
            head       = head->next;
            delete temp;
        }

        tail     = nullptr;
        listSize = 0;

        xSemaphoreGive(mutex);
    }
};

#endif /* LINKED_LIST_H */
