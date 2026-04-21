#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"
#include "algorithm.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
/**
 * a data container like std::list
 * allocate random memory addresses for data and they are doubly-linked in a list.
 */
template<typename T>
class list {
protected:
    class node {
    public:
        T *value;   // stored pointer because T may not have default constructor
        node *prev;
        node *next;

        node() : value(nullptr), prev(nullptr), next(nullptr) {}
        node(const T &v, node *p = nullptr, node *n = nullptr)
            : value(new T(v)), prev(p), next(n) {}
        // Sentinel constructor (no value)
        explicit node(std::nullptr_t) : value(nullptr), prev(nullptr), next(nullptr) {}

        ~node() {
            if (value) {
                delete value;
                value = nullptr;
            }
        }
    };

protected:
    node *head;     // sentinel node: head->next == first element, head->prev == last element
    size_t list_size;

    /**
     * insert node cur before node pos
     * return the inserted node cur
     */
    node *insert(node *pos, node *cur) {
        cur->prev = pos->prev;
        cur->next = pos;
        pos->prev->next = cur;
        pos->prev = cur;
        ++list_size;
        return cur;
    }
    /**
     * remove node pos from list (no need to delete the node)
     * return the removed node pos
     */
    node *erase(node *pos) {
        pos->prev->next = pos->next;
        pos->next->prev = pos->prev;
        pos->prev = nullptr;
        pos->next = nullptr;
        --list_size;
        return pos;
    }

    void init_sentinel() {
        head = new node();
        head->prev = head;
        head->next = head;
        list_size = 0;
    }

public:
    class const_iterator;
    class iterator {
        friend class list<T>;
        friend class const_iterator;
    private:
        node *ptr;
        const list<T> *container;
    public:
        iterator() : ptr(nullptr), container(nullptr) {}
        iterator(node *p, const list<T> *c) : ptr(p), container(c) {}
        iterator(const iterator &other) : ptr(other.ptr), container(other.container) {}

        /**
         * iter++
         */
        iterator operator++(int) {
            if (ptr == nullptr || container == nullptr || ptr == container->head)
                throw invalid_iterator();
            iterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }
        /**
         * ++iter
         */
        iterator & operator++() {
            if (ptr == nullptr || container == nullptr || ptr == container->head)
                throw invalid_iterator();
            ptr = ptr->next;
            return *this;
        }
        /**
         * iter--
         */
        iterator operator--(int) {
            if (ptr == nullptr || container == nullptr || ptr->prev == container->head)
                throw invalid_iterator();
            iterator tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }
        /**
         * --iter
         */
        iterator & operator--() {
            if (ptr == nullptr || container == nullptr || ptr->prev == container->head)
                throw invalid_iterator();
            ptr = ptr->prev;
            return *this;
        }
        T & operator *() const {
            if (ptr == nullptr || container == nullptr || ptr == container->head || ptr->value == nullptr)
                throw invalid_iterator();
            return *(ptr->value);
        }
        T * operator ->() const {
            if (ptr == nullptr || container == nullptr || ptr == container->head || ptr->value == nullptr)
                throw invalid_iterator();
            return ptr->value;
        }
        bool operator==(const iterator &rhs) const {
            return ptr == rhs.ptr && container == rhs.container;
        }
        bool operator==(const const_iterator &rhs) const {
            return ptr == rhs.ptr && container == rhs.container;
        }
        bool operator!=(const iterator &rhs) const {
            return !(*this == rhs);
        }
        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }
    };

    class const_iterator {
        friend class list<T>;
        friend class iterator;
    private:
        node *ptr;
        const list<T> *container;
    public:
        const_iterator() : ptr(nullptr), container(nullptr) {}
        const_iterator(node *p, const list<T> *c) : ptr(p), container(c) {}
        const_iterator(const const_iterator &other) : ptr(other.ptr), container(other.container) {}
        const_iterator(const iterator &other) : ptr(other.ptr), container(other.container) {}

        const_iterator operator++(int) {
            if (ptr == nullptr || container == nullptr || ptr == container->head)
                throw invalid_iterator();
            const_iterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }
        const_iterator & operator++() {
            if (ptr == nullptr || container == nullptr || ptr == container->head)
                throw invalid_iterator();
            ptr = ptr->next;
            return *this;
        }
        const_iterator operator--(int) {
            if (ptr == nullptr || container == nullptr || ptr->prev == container->head)
                throw invalid_iterator();
            const_iterator tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }
        const_iterator & operator--() {
            if (ptr == nullptr || container == nullptr || ptr->prev == container->head)
                throw invalid_iterator();
            ptr = ptr->prev;
            return *this;
        }
        const T & operator *() const {
            if (ptr == nullptr || container == nullptr || ptr == container->head || ptr->value == nullptr)
                throw invalid_iterator();
            return *(ptr->value);
        }
        const T * operator ->() const {
            if (ptr == nullptr || container == nullptr || ptr == container->head || ptr->value == nullptr)
                throw invalid_iterator();
            return ptr->value;
        }
        bool operator==(const iterator &rhs) const {
            return ptr == rhs.ptr && container == rhs.container;
        }
        bool operator==(const const_iterator &rhs) const {
            return ptr == rhs.ptr && container == rhs.container;
        }
        bool operator!=(const iterator &rhs) const {
            return !(*this == rhs);
        }
        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }
    };

    list() {
        init_sentinel();
    }
    list(const list &other) {
        init_sentinel();
        for (node *p = other.head->next; p != other.head; p = p->next) {
            node *cur = new node(*(p->value));
            insert(head, cur);
        }
    }
    virtual ~list() {
        clear();
        delete head;
        head = nullptr;
    }
    list &operator=(const list &other) {
        if (this == &other) return *this;
        clear();
        for (node *p = other.head->next; p != other.head; p = p->next) {
            node *cur = new node(*(p->value));
            insert(head, cur);
        }
        return *this;
    }

    const T & front() const {
        if (list_size == 0) throw container_is_empty();
        return *(head->next->value);
    }
    const T & back() const {
        if (list_size == 0) throw container_is_empty();
        return *(head->prev->value);
    }

    iterator begin() { return iterator(head->next, this); }
    const_iterator cbegin() const { return const_iterator(head->next, this); }
    iterator end() { return iterator(head, this); }
    const_iterator cend() const { return const_iterator(head, this); }

    virtual bool empty() const { return list_size == 0; }
    virtual size_t size() const { return list_size; }

    virtual void clear() {
        node *p = head->next;
        while (p != head) {
            node *next = p->next;
            delete p;
            p = next;
        }
        head->prev = head;
        head->next = head;
        list_size = 0;
    }

    virtual iterator insert(iterator pos, const T &value) {
        if (pos.container != this || pos.ptr == nullptr)
            throw invalid_iterator();
        node *cur = new node(value);
        insert(pos.ptr, cur);
        return iterator(cur, this);
    }

    virtual iterator erase(iterator pos) {
        if (pos.container != this || pos.ptr == nullptr || pos.ptr == head || list_size == 0)
            throw invalid_iterator();
        node *next = pos.ptr->next;
        erase(pos.ptr);
        delete pos.ptr;
        return iterator(next, this);
    }

    void push_back(const T &value) {
        node *cur = new node(value);
        insert(head, cur);
    }
    void pop_back() {
        if (list_size == 0) throw container_is_empty();
        node *last = head->prev;
        erase(last);
        delete last;
    }
    void push_front(const T &value) {
        node *cur = new node(value);
        insert(head->next, cur);
    }
    void pop_front() {
        if (list_size == 0) throw container_is_empty();
        node *first = head->next;
        erase(first);
        delete first;
    }

    /**
     * sort the values in ascending order with operator< of T
     * Uses a merge sort on the linked list - O(n log n) with no element copying (only pointer manipulation for merging).
     * However, the spec explicitly allows copying/moving for sort, so we use stable merge sort on nodes.
     */
    void sort() {
        if (list_size <= 1) return;
        // detach cycle: make it a singly-null-terminated list temporarily
        node *first = head->next;
        node *last = head->prev;
        last->next = nullptr;
        first->prev = nullptr;

        node *sorted = merge_sort(first, list_size);

        // reattach
        head->next = sorted;
        sorted->prev = head;
        node *p = sorted;
        while (p->next != nullptr) p = p->next;
        p->next = head;
        head->prev = p;
    }

    void merge(list &other) {
        if (this == &other) return;
        if (other.list_size == 0) return;
        if (list_size == 0) {
            // steal all nodes
            head->next = other.head->next;
            head->prev = other.head->prev;
            head->next->prev = head;
            head->prev->next = head;
            list_size = other.list_size;
            other.head->next = other.head;
            other.head->prev = other.head;
            other.list_size = 0;
            return;
        }
        node *p1 = head->next;
        node *p2 = other.head->next;
        node *end1 = head;
        node *end2 = other.head;

        while (p1 != end1 && p2 != end2) {
            if (*(p2->value) < *(p1->value)) {
                // take p2 and insert before p1
                node *take = p2;
                p2 = p2->next;
                // splice
                take->prev = p1->prev;
                take->next = p1;
                p1->prev->next = take;
                p1->prev = take;
            } else {
                p1 = p1->next;
            }
        }
        if (p2 != end2) {
            // splice remaining p2..end2->prev to end of *this
            node *first2 = p2;
            node *last2 = end2->prev;
            first2->prev = end1->prev;
            end1->prev->next = first2;
            last2->next = end1;
            end1->prev = last2;
        }
        list_size += other.list_size;
        other.head->next = other.head;
        other.head->prev = other.head;
        other.list_size = 0;
    }

    void reverse() {
        if (list_size <= 1) return;
        node *p = head;
        do {
            node *tmp = p->next;
            p->next = p->prev;
            p->prev = tmp;
            p = tmp;
        } while (p != head);
    }

    void unique() {
        if (list_size <= 1) return;
        node *p = head->next;
        while (p->next != head) {
            if (*(p->value) == *(p->next->value)) {
                node *dup = p->next;
                erase(dup);
                delete dup;
            } else {
                p = p->next;
            }
        }
    }

private:
    // Merge sort on nodes connected via prev/next (prev/next forms a plain non-circular doubly-linked list here).
    // Returns head of sorted list. Sorted list's prev links are maintained.
    static node *merge_sort(node *head_ptr, size_t n) {
        if (n <= 1) return head_ptr;
        size_t mid = n / 2;
        node *p = head_ptr;
        for (size_t i = 1; i < mid; ++i) p = p->next;
        node *second = p->next;
        p->next = nullptr;
        second->prev = nullptr;

        node *a = merge_sort(head_ptr, mid);
        node *b = merge_sort(second, n - mid);
        return merge_nodes(a, b);
    }

    static node *merge_nodes(node *a, node *b) {
        node dummy;
        node *tail = &dummy;
        dummy.next = nullptr;
        while (a != nullptr && b != nullptr) {
            if (*(b->value) < *(a->value)) {
                tail->next = b;
                b->prev = tail;
                tail = b;
                b = b->next;
            } else {
                tail->next = a;
                a->prev = tail;
                tail = a;
                a = a->next;
            }
        }
        if (a != nullptr) {
            tail->next = a;
            a->prev = tail;
        } else if (b != nullptr) {
            tail->next = b;
            b->prev = tail;
        } else {
            tail->next = nullptr;
        }
        node *result = dummy.next;
        if (result) result->prev = nullptr;
        // dummy goes out of scope, but dummy has no dynamic allocations; safe.
        return result;
    }
};

}

#endif //SJTU_LIST_HPP
