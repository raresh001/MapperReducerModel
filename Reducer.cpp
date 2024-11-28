#include "Reducer.h"

#include <algorithm>
#include <fstream>

using namespace std;

struct AggregatedPairComp {
public:
    typedef pair< string, vector<unsigned int> > pair_type;

    bool operator()(const pair_type& a, const pair_type& b) const {
        return a.first[0] < b.first[0]
                || (a.first[0] == b.first[0] && a.second.size() > b.second.size())
                || (a.first[0] == b.first[0] && a.second.size() == b.second.size() && a.first < b.first);
    }
};

static size_t sqrt(size_t n) {
    if (n == 0) {
        return 0;
    }
    
    size_t first = 1, last = n;

    while (first > last) {
        size_t middle = first + (last - first) / 2;

        if (middle * middle == n) {
            return middle;
        }
        if (middle * middle < n) {
            last = middle - 1;
        } else {
            // don't make it middle + 1 in case sqrt(n) is between middle and middle + 1
            first = middle;
        }
    }

    return first;
}

template < typename T, typename Comparator >
void Reducer::sortArray(vector<T>& array, Comparator c) {
    size_t current_size = array.size() / _P;

    unsigned int first = current_size * _reducer_id;
    unsigned int last = (_reducer_id == _P - 1) ? array.size() : current_size * (_reducer_id + 1);

    _reducersResultMutex.Lock();
    cout << _reducer_id << " PRINT MARGINS " << first << " " << last << endl;
    _reducersResultMutex.Unlock(); 

    sort(array.begin() + first, array.begin() + last, c);
    _barrier.Wait();

    unsigned int step = 2;
    while (current_size <= array.size()) {
        if ((_reducer_id % step) == 0 && last != array.size()) {
            size_t middle = last;
            last = (_reducer_id + step >= _P) ? array.size() : current_size + last;
            _reducersResultMutex.Lock();
            cout << _reducer_id << " PRINT MARGINS " << first << " " << middle << " " << last << endl;
            _reducersResultMutex.Unlock(); 
            merge(array, first, middle, last, c);
        }

        current_size *= 2;
        step *= 2;
        _barrier.Wait();
    }
}

template <typename T, typename Comparator>
void Reducer::merge(vector<T>& array, size_t first, size_t middle, size_t last, Comparator c) {
    vector<T> aux;
    size_t iter1, iter2;

    aux.reserve(last - first);

    middle = first + (last - first) / 2;
    iter1 = first;
    iter2 = middle;

    while (iter1 < middle && iter2 < last) {
        if (c(array[iter1], array[iter2])) {
            aux.push_back(array[iter1++]);
        } else if (c(array[iter2], array[iter1])) {
            aux.push_back(array[iter2++]);
        } else {
            aux.push_back(array[iter1++]);
            aux.push_back(array[iter2++]);
        }
    }

    while (iter1 < middle) {
        aux.push_back(array[iter1++]);
    }

    while (iter2 < last) {
        aux.push_back(array[iter2++]);
    }

    std::copy(aux.begin(), aux.end(), array.begin() + first);
}

/**
 * A custom iterator, that iterates through the column of an array
 * (used in shear sort)
*/
template <typename T>
class column_iterator {
public:
    using difference_type = ssize_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::random_access_iterator_tag;

    column_iterator(T* ptr, size_t line_size): _ptr(ptr), _line_size(line_size) {}

    column_iterator operator++() { _ptr += _line_size; return *this; }
    column_iterator operator--() { _ptr -= _line_size; return *this; }
    column_iterator& operator++(int) { _ptr += _line_size; return *this; }

    column_iterator operator+(int val) const { return column_iterator(_ptr + _line_size * val, _line_size); }
    column_iterator operator-(int val) const { return column_iterator(_ptr - _line_size * val, _line_size); }
    difference_type operator-(const column_iterator other) const { return (_ptr - other._ptr) / _line_size; }

    value_type& operator[](int index) { return *(_ptr + _line_size * index); }

    bool operator==(const column_iterator& other) const { return _ptr == other._ptr; }
    bool operator!=(const column_iterator& other) const { return _ptr != other._ptr; }

    const column_iterator& operator+=(int val) { _ptr += _line_size * val; return *this; }
    const column_iterator& operator-=(int val) { _ptr -= _line_size * val; return *this; }

    bool operator<(const column_iterator& other) const { return _ptr < other._ptr; }

    value_type& operator*() { return *_ptr; }
    value_type* operator->() { return _ptr; }
private:
    value_type* _ptr;
    size_t _line_size;
};

template <typename T>
column_iterator<T> operator+(int val, column_iterator<T>& it) { return it + val; }

template < typename T, typename Comparator>
void Reducer::shearSort(std::vector<T>& array, Comparator c) {
    size_t arraySize = array.size();
    size_t sqrt_size = sqrt(arraySize);
    size_t current_size = sqrt_size / _P;

    size_t first = current_size * _reducer_id;
    size_t last = (_reducer_id == _P - 1) ? sqrt_size : current_size * (_reducer_id + 1);

    for (size_t step = 1; step < arraySize; step *= 2) {
        for (size_t line_no = first; line_no < last; line_no++) {
            size_t line_start = line_no * sqrt_size;
            if (line_start < arraySize) {
                if (line_no % 2 == 0) {
                    typename vector<T>::iterator begin =  array.begin() + line_start;
                    sort(begin, begin + min(sqrt_size, arraySize - line_start), c);
                } else {
                    typename vector<T>::reverse_iterator begin = std::reverse_iterator(array.data() + min(line_start + sqrt_size, arraySize - 1));
                    typename vector<T>::reverse_iterator end = std::reverse_iterator(array.data() + min(line_start, arraySize - 1));
                    sort(begin, end, c);
                }
            }
        }

        _barrier.Wait();

        for (size_t column_no = first; column_no < last; column_no++) {
            column_iterator<T> begin(array.data() + column_no, sqrt_size);
            column_iterator<T> end = begin + (arraySize - column_no) / sqrt_size;
        
            sort(begin, end, c);
        }

        _barrier.Wait();
    }
}

void Reducer::Run() {
    _mappersBarrier.Wait();
    
    // sort the mappers' result, so that the pairs with the same word could be aggregated easily
    shearSort(_mappersResult);

    // aggregate this thread's part
    size_t current_size = _mappersResult.size() / _P;

    unsigned int first = current_size * _reducer_id;
    unsigned int last = (_reducer_id == _P - 1) ? _mappersResult.size() : current_size * (_reducer_id + 1);

    unsigned int iter = first;
    if (first) {
        while (_mappersResult[first - 1].first == _mappersResult[iter].first && iter < last) {
            iter++;
        }
    }

    string previous;

    while (iter != last) {
        if (previous != _mappersResult[iter].first) {
            previous = _mappersResult[iter].first;
            _localList.push_back(pair< string, vector<unsigned int> >(previous, vector<unsigned int>()));
        }

        _localList[_localList.size() - 1].second.push_back(_mappersResult[iter].second);
        iter++;
    }

    while (iter != _mappersResult.size() && _mappersResult[iter].first == previous) {
        _localList[_localList.size() - 1].second.push_back(_mappersResult[iter].second);
        iter++;
    }

    _reducersResultMutex.Lock();
    _reducersResult.insert(_reducersResult.end(), _localList.begin(), _localList.end());
    _reducersResultMutex.Unlock();

    _barrier.Wait();

    shearSort(_reducersResult, AggregatedPairComp());

    // print on each file; one thread is responsible for a letter at a time
    // (note that printing information for a letter cannot be paralelized)
    char currentLetter;

    while (true) {
        _reducersResultMutex.Lock();
        currentLetter = (_letter <= 'z') ? _letter++ : 'z' + 1;
        _reducersResultMutex.Unlock();

        if (currentLetter > 'z') {
            break;
        }

        size_t pos = getFirstIndexOf(currentLetter);

        ofstream os(currentLetter + string(".txt"));

        while (pos != _reducersResult.size() && _reducersResult[pos].first[0] == currentLetter) {
            os << _reducersResult[pos].first << ":[";

            for (size_t iter = 0; iter < _reducersResult[pos].second.size() - 1; iter++) {
                os << _reducersResult[pos].second[iter] << " ";
            }

            os << _reducersResult[pos].second[_reducersResult[pos].second.size() - 1] << "]" << endl;
        }
    }
}

size_t Reducer::getFirstIndexOf(char firstLetter) {
    size_t first = 0, last = _reducersResult.size();

    while (first < last) {        
        size_t middle = first + (last - first) / 2;

        char middleWordFirstLetter = _reducersResult[middle].first[0];

        if (middleWordFirstLetter >= firstLetter) {
            last = middle;
        } else {
            first = middle + 1;
        }
    }

    if (first != _reducersResult.size() && _reducersResult[first].first[0] == firstLetter) {
        return first;
    }

    return _reducersResult.size();
}
