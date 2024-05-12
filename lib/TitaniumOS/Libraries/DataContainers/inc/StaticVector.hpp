#include <iostream>
#include <vector>
#include <memory>

// This class need to be task safe

template <typename T>
class StaticVector
{
public:
	StaticVector(size_t size)
	{
		this->_internal_vector = std::make_unique<std::vector<T>>(size);
		this->_maximum_size = size;
	}

	int push_back(const T &data)
	{
		int result = -1;

		if (_size < _maximum_size)
		{
			_internal_vector->push_back(data);
			this->_size++;
			result = 0;
		}
		return result;
	}

	T back(void)
	{
		return this->_internal_vector.back();
	}

	T pop_back(void)
	{
		this->_size--;
		return this->_internal_vector.pop_back();
	}

private:
	std::unique_ptr<std::vector<T>> _internal_vector = nullptr;
	size_t _size = 0;
	size_t _maximum_size = 0;
};