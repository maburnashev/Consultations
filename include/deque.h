#pragma once

#include <initializer_list>
#include <algorithm>

class Deque {
public:
    Deque() {
        pptr_ = new int*[blocks_capacity_];
    }

    Deque(size_t size) : size_(size) {
        blocks_count_ = (size_ + kBlockSize - 1) / kBlockSize;
        blocks_capacity_ = (blocks_count_ - blocks_count_ / 2) * 2;
        pptr_ = new int*[blocks_capacity_];
        for (size_t i = 0; i < blocks_count_; ++i) {
            if (i == size_ / kBlockSize) {
                pptr_[i] = new int[kBlockSize];
                for (size_t j = 0; j < size_ % kBlockSize; ++j) {
                    pptr_[i][j] = 0;
                }
            } else {
                pptr_[i] = new int[kBlockSize]{0};
            }
        }
    }

    Deque(std::initializer_list<int> list) : size_(list.size()) {
        blocks_count_ = (size_ + kBlockSize - 1) / kBlockSize;
        blocks_capacity_ = kBlockSize;
        pptr_ = new int*[blocks_capacity_];
        for (size_t i = 0; i < blocks_count_; ++i) {
            pptr_[i] = new int[kBlockSize];
        }
        for (size_t i = 0; i < size_; ++i) {
            pptr_[i / kBlockSize][i % kBlockSize] = *(list.begin() + i);
        }
    }

    Deque(const Deque& other) : size_(other.size_) {
        blocks_count_ = (size_ + kBlockSize - 1) / kBlockSize;
        blocks_capacity_ = kBlockSize;
        pptr_ = new int*[blocks_capacity_];
        for (size_t i = 0; i < blocks_count_; ++i) {
            pptr_[i] = new int[kBlockSize];
        }
        for (size_t i = 0; i < size_; ++i) {
            (*this)[i] = other[i];
        }
    }

    Deque(Deque&& other)
        : size_(other.size_),
          first_block_(other.first_block_),
          first_element_(other.first_element_),
          blocks_count_(other.blocks_count_),
          blocks_capacity_(other.blocks_capacity_) {
        pptr_ = other.pptr_;
        other.pptr_ = nullptr;
        other.size_ = 0;
        other.first_block_ = 0;
        other.first_element_ = 0;
        other.blocks_capacity_ = 0;
        other.blocks_count_ = 0;
    }

    ~Deque() {
        for (size_t i = 0; i < blocks_count_; ++i) {
            delete[] pptr_[(first_block_ + i) % blocks_capacity_];
        }
        delete[] pptr_;
    }

    Deque& operator=(const Deque& other) {
        Deque copy(other);
        Swap(copy);
        return *this;
    }

    Deque& operator=(Deque&& other) {
        Deque tmp(std::move(other));
        Swap(tmp);
        return *this;
    }

    void PushBack(int value) {
        size_t absolute = first_element_ + size_;
        size_t end_block = absolute / kBlockSize;
        size_t end_index = absolute % kBlockSize;
        if (end_block == blocks_count_) {
            if (blocks_count_ == blocks_capacity_) {
                SummonBlocks();
            }
            size_t bucket = (first_block_ + end_block) % blocks_capacity_;
            pptr_[bucket] = new int[kBlockSize];
            pptr_[bucket][0] = value;
            ++blocks_count_;
            ++size_;
        } else {
            size_t bucket = (first_block_ + end_block) % blocks_capacity_;
            pptr_[bucket][end_index] = value;
            ++size_;
        }
    }

    void PopBack() {
        --size_;
        size_t absolute = first_element_ + size_;
        size_t end_index = absolute % kBlockSize;
        size_t end_block = absolute / kBlockSize;
        // логика с последним элементом всего массива
        if (end_index == 0) {
            size_t bucket = (first_block_ + end_block) % blocks_capacity_;
            delete[] pptr_[bucket];
            pptr_[bucket] = nullptr;
            --blocks_count_;
        }
    }

    void PushFront(int value) {
        if (first_element_ == 0) {
            if (blocks_count_ == blocks_capacity_) {
                SummonBlocks();
            }
            first_block_ = (first_block_ - 1 + blocks_capacity_) % blocks_capacity_;
            first_element_ = (first_element_ - 1 + kBlockSize) % kBlockSize;
            pptr_[first_block_] = new int[kBlockSize];
            pptr_[first_block_][first_element_] = value;
            ++size_;
            ++blocks_count_;
        } else {
            --first_element_;
            pptr_[first_block_][first_element_] = value;
            ++size_;
        }
    }

    void PopFront() {
        first_element_ = (first_element_ + 1 + kBlockSize) % kBlockSize;
        if (first_element_ == 0) {
            delete[] pptr_[first_block_];
            pptr_[first_block_] = nullptr;
            first_block_ = (first_block_ + 1 + blocks_capacity_) % blocks_capacity_;
            --blocks_count_;
        }
        --size_;
    }

    void SummonBlocks() {
        size_t new_capacity = (blocks_capacity_ == 0) ? 1 : blocks_capacity_ * 2;
        int** pptr_new = new int*[new_capacity];
        for (size_t i = 0; i < blocks_count_; ++i) {
            size_t old_block_index = (first_block_ + i) % blocks_capacity_;
            pptr_new[i] = pptr_[old_block_index];
        }
        delete[] pptr_;
        pptr_ = pptr_new;
        blocks_capacity_ = new_capacity;
        pptr_new = nullptr;
        first_block_ = 0;
    }

    int operator[](size_t index) const {
        size_t bucket_number =
            (first_block_ + (first_element_ + index) / kBlockSize) % blocks_capacity_;
        size_t element_number = (first_element_ + index) % kBlockSize;
        return pptr_[bucket_number][element_number];
    }

    int& operator[](size_t index) {
        size_t bucket_number =
            (first_block_ + (first_element_ + index) / kBlockSize) % blocks_capacity_;
        size_t element_number = (first_element_ + index) % kBlockSize;
        return pptr_[bucket_number][element_number];
    }

    void Swap(Deque& other) {
        std::swap(size_, other.size_);
        std::swap(first_block_, other.first_block_);
        std::swap(first_element_, other.first_element_);
        std::swap(blocks_count_, other.blocks_count_);
        std::swap(blocks_capacity_, other.blocks_capacity_);
        std::swap(pptr_, other.pptr_);
    }

    size_t Size() const {
        return size_;
    }

    void Clear() {
        for (size_t i = 0; i < blocks_count_; ++i) {
            size_t index = (first_block_ + i) % blocks_capacity_;
            delete[] pptr_[index];
            pptr_[index] = nullptr;
        }
        size_ = 0;
        blocks_count_ = 0;
        first_element_ = 0;
        first_block_ = 0;
    }

private:
    static const int kBlockSize = 128;  // в 512 байт влезает 128 интов
    int** pptr_ = nullptr;  // указатель на массив указателей на массивы
    size_t size_ = 0;
    size_t first_block_ = 0;
    size_t first_element_ = 0;
    size_t blocks_count_ = 0;
    size_t blocks_capacity_ = 0;  // capacity считаем в блоках для удобства
};
