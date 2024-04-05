Данные библиотеки являются примером реализации базовых классов и алгоритмов программирования.

# 1. Итераторы

Своё большое путешуствие по реализованным классам начнём с самых элементарных классов, которые в дальнейшем нам понадобятся для реализации всех классов-контейнеров - с итераторов.

**Итератор** - структура, предоставляющая доступ к элементам коллекции и навигацию по ним. В зависимости от типа итератора уровень доступа может различаться. Всего выделяют пять типов:

1) Input iterator (итератор ввода)
2) Output iterator (итератор вывода)
3) Forward iterator (прямой итератор)
4) Bidirectional iterator (двунаправленный итератор)
5) Random access iterator (итератор произвольного доступа)

Рассмотрим и реализуем итераторы всех видов

## 1.1 Итератор ввода (input iterator)

Итератор ввода - самый простейший итератор из всех представленных. Эти итераторы предоставляют наименьший доступ. Операции, которые возможны над итератором ввода:

1) Разыменование (dereferencing) - получение значения по данному итератору. Причём получаемое значение является rvalue, т.е. его можно только присвоить в какую-либо переменную, но нельзя никак изменить. Разыменование возможно двумя способами:
    ```c++
    *it;            // Dereferencing using *
    it->member;     // Accessing a member of the object that the iterator points to
    var = *it;      // Assigning a value to a variable
    ```
2) Сравнение на равенство (equality/inequality comparison) - сравнение двух итераторов на равенство. Два итератора называются равными, если адреса объектов, на которые данные итераторы указывают, совпадают.
    ```c++
    it1 == it2;     // Checking for equality
    it1 != it2;     // Checking for unequality
    ```
3) Инкрементация (increment) - переход к следующему в коллекции элементу.
    ```c++
    it++;           // Post increment
    ++it;           // Pre increment
    ```
Практическое применение: данные итераторы используются в так называемых single-pass алгоритмах, где используется лишь один проход по коллекции, например, поиск значения в коллекции.

### Реализация интерфейса

```c++
template <typename ValueType>
class InputIterator {
public:
    using ValueType         = ValueType;
    using Pointer           = ValueType*;
    using ConstPointer      = const ValueType*;
    using Reference         = ValueType&;
    using ConstReference    = const Reference&;
    using SizeType          = size_t;
    using RightReference    = ValueType&&;

    InputIterator() = delete;

    // dereferencing

    virtual ValueType operator*() const = 0;
    virtual ConstPointer operator->() const = 0;

    // checking for equality/unequality

    virtual bool operator==(const InputIterator& other) const noexcept = 0; 

    virtual bool operator!=(const InputIterator& other) const noexcept = 0;

    // increment

    virtual void operator++() = 0;
    virtual OutputIterator& operator++(int) = 0;
};
```

## 1.2 Итератор вывода (output iterator)

Итератор вывода, в некотором роде, является противоположностью итератора ввода. Эта противоположность заключается в их единственном отличии: переменные, лежащие под итератором, нельзя присваивать переменным, но можно изменять. Остальное остаётся тем же:
1) Разыменование
    ```c++
    *it;
    *it = var;  // assignment to variable that iterator points to
    ```
2) Сравнение на равенство
    ```c++
    it1 == it2;
    it1 != it2;
    ```
3) Инкрементация
    ```c++
    it++;
    ++it;
    ```
Практическое применение: как и input итераторы, output итераторы применяются в single-pass алгоритмах, но только в тех, где используется присваивание. Например, копирование коллекций

## Реализация интерфейса


```c++
template <typename ValueType>
class OutputIterator {
public:
    using ValueType         = ValueType;
    using Pointer           = ValueType*;
    using ConstPointer      = const ValueType*;
    using Reference         = ValueType&;
    using ConstReference    = const Reference&;
    using SizeType          = size_t;
    using RightReference    = ValueType&&;

    Iterator() = delete;

    // dereferencing

    virtual Iterator operator*() const = 0;

    // checking for equality/unequality

    virtual bool operator==(const OutputIterator& other) const noexcept = 0; 

    virtual bool operator!=(const OutputIterator& other) const noexcept = 0;

    // increment

    virtual void operator++() = 0;
    virtual OutputIterator& operator++(int) = 0;
};
```

Здесь не будет приведено реализаций input и output итераторов, поскольку они находят довольно малое применение, в отличии от типа итератора, который представлен дальше.

## 1.3 Прямой итератор (forward iterator)

Прямой итератор - это итератор, который включает в себя свойства как итератора ввода, так и итератора вывода, т.е. его операциями являются:

1) Разыменование, причём как rvalue, так и lvalue
    ```c++
    *it;
    it->member;
    *it = var;  // assignment to variable that iterator points to
    var = *it;      // Assigning a value to a variable
    ```
2) Сравнение на равенство
    ```c++
    it1 == it2;
    it1 != it2;
    ```
3) Инкрементация
    ```c++
    it++;
    ++it;
    ```

### Реализация интерфейса

В качестве реализации можно взять ту же реализацию, что представлена в 1.2, по причине, описанной там же.

## 1.4 Двунаправленный итератор (Bidirectional iterator)

Из названия этого итератора, скорее всего, понятно, чем он отличается от предыдущих. Двунаправленный итератор - прямой итератор с возможностью итерироваться назад. Т.е. помимо всех перечисленныхх ранее операций, доступен также декремент:
```c++
it--;       // post decrement
--it        // pre decrement
```

### Реализация интерфейса

Двунаправленный итератор можно определить как наследника прямого итератора, поскольку он полностью перенимает его операции:

```c++
template <typename ValueType>
class BidirectionalIterator : public ForwardIterator<ValueType> {
public:
    using Base              = ForwardIterator<ValueType>;
    using Pointer           = typename Base::Pointer;
    using Reference         = typename Base::Reference;
    using ConstPointer      = typename Base::ConstPointer;
    using ConstReference    = typename Base::ConstReference;
    using SizeType          = typename Base::SizeType;
    using RightReference    = typename Base::RightReference;

    virtual void operator--() = 0;
};
```

## 1.5 Итератор произвольного доступа (Random access iterator)

Итератор произвольного доступа - итератор, предоставляющий наибольший доступ к коллекции. Он включает в себя все возможные операции двунаправленного итератора и дополнительно предоставляет возможность с любого элемента коллекции попасть в любой другой. Т.е. доступны следующие операции:
1) Арифметические операции
    ```c++
    it + 3;     // moving by 3 elements forward
    it - 2;     // moving by 2 elements backward
    it += 3;    // it = it + 3;
    it -= 2;    // it = it - 2;
    ```
2) Операция разности
    ```c++
    int n = it1 - it2;  // it means that it1 = it2 + n 
    ```
3) Операции сравнения
    ```c++
    it1 > it2;      // it means that it1 - it2 > 0
    it1 <= it2;     // it means that it2 - it2 <= 0
    ```

## Реализация интерфейса

Итератор произвольного доступа можно реализовать наследованием от двунаправленного итератора:

```c++
template <typename ValueType>
class RandomAccessIterator : public BidirectionalIterator<ValueType> {
public:
    using Base              = BidirectionalIterator<ValueType>;
    using Pointer           = typename Base::Pointer;
    using Reference         = typename Base::Reference;
    using ConstPointer      = typename Base::ConstPointer;
    using ConstReference    = typename Base::ConstReference;
    using SizeType          = typename Base::SizeType;
    using ItDiff            = int;
    using RightReference    = typename Base::RightReference;

    virtual void operator+= (int) = 0;
    virtual void operator-= (int) = 0;
    virtual ItDiff operator- (const RandomAccessIterator&) const = 0;
    virtual operator> (const RandomAccessIterator&) const = 0;
    virtual operator< (const RandomAccessIterator&) const = 0;
    virtual operator>= (const RandomAccessIterator&) const = 0;
    virtual operator<= (const RandomAccessIterator&) const = 0;
};
```
Из реализации мы оставим только Forward, Bidirectional и RandomAccess итераторы, т.к. далее не возникнет потребности в ограничениях, которые накладывает Input и Output итераторы.
