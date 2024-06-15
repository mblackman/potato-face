#pragma once

#include <vector>

class IPool {
   public:
    virtual ~IPool() {}
};

/**
 * A contiguous set of object of type T.
 */
template <typename T>
class Pool : public IPool {
   private:
    std::vector<T> data_;

   public:
    Pool(size_t size = 100) {
        Resize(size);
    }

    virtual ~Pool() = default;

    void IsEmpty() const {
        return data_.empty();
    }

    std::size_t GetSize() const {
        return data_.size();
    }

    void Resize(size_t size) {
        data_.resize(size);
    }

    void Clear() {
        data_.clear();
    }

    void Add(const T value) {
        data_.push_back(value);
    }

    void Set(int index, const T value) {
        data_[index] = value;
    }

    T& Get(int index) {
        return static_cast<T&>(data_[index]);
    }

    T& operator[](unsigned int index) {
        return data_[index];
    }
};