#pragma once

class Exception {
public:
    Exception() = default;
    virtual const char* what() const noexcept = 0;
};

class NotComparableIterators : public Exception {
public:
    const char* what() const noexcept override {
        return "Iterators aren't comparable";
    }
};

class UndereferencableIterator : public Exception {
public:
    const char* what() const noexcept override {
        return "Cannot dereference this iterator";
    }
};

class IteratorOutOfBounds : public Exception {
public:
    const char* what() const noexcept override {
        return "Iterator is out of bounds";
    }
};

class AnotherIterator : public Exception {
public:
    const char* what() const noexcept override {
        return "Given iterator points to another collection";
    }
};

class NothingToErase : public Exception {
public:
    const char* what() const noexcept override {
        return "Nothing to erase in the collection";
    }
};

class UndereferencableList : public Exception {
public:
    const char* what() const noexcept override {
        return "Cannot get the element of the list because its empty";
    }
};
