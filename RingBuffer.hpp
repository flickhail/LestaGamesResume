#pragma once

// Ring-buffer implementation, version 1.
// Pros:	easy-to-use templated class for working with any underlaying type;
//			can read and write the whole regions of data via pointers;
// Cons:	can be hard to maintain because of fast, but unsafe pointers
//			there is can be memory leak because of pointers, so there is need in careful coding of this class
template<typename T>
class RingBufferV1
{
public:
	//				[CONSTRUCTORS & DESTRUCTOR]

	explicit RingBufferV1(size_t bufferSize)
		: m_Buffer{ new T[bufferSize] }
		, m_BufferSize{ bufferSize }
		, m_WriteOffset{ 0 }
		, m_ReadOffset{ 0 }
		, m_ElementsAvailable{ bufferSize }
	{}

	RingBufferV1(const RingBufferV1& buffer)
		: m_Buffer{ new T[buffer.m_BufferSize] }
		, m_BufferSize{ buffer.m_BufferSize }
		, m_WriteOffset{ buffer.m_WriteOffset }
		, m_ReadOffset{ buffer.m_ReadOffset }
		, m_ElementsAvailable{ buffer.m_ElementsAvailable }
	{
		for (size_t i{ 0 }; i < m_BufferSize; ++i)
			m_Buffer[i] = buffer.m_Buffer[i];
	}

	RingBufferV1(RingBufferV1&& buffer) noexcept
		: m_Buffer{ buffer.m_Buffer }
		, m_BufferSize{ buffer.m_BufferSize }
		, m_WriteOffset{ buffer.m_WriteOffset }
		, m_ReadOffset{ buffer.m_ReadOffset }
		, m_ElementsAvailable{ buffer.m_ElementsAvailable }
	{
		buffer.m_Buffer = nullptr;
		buffer.m_BufferSize = 0;
		buffer.m_WriteOffset = 0;
		buffer.m_ReadOffset = 0;
		buffer.m_ElementsAvailable = 0;
	}

	~RingBufferV1() { delete[] m_Buffer; }


	//				[OPERATORS}

	RingBufferV1& operator=(const RingBufferV1& buffer)
	{
		if (&buffer == this)
			return *this;

		m_Buffer = new T[buffer.m_BufferSize];
		m_BufferSize = buffer.m_BufferSize;
		for (size_t i{ 0 }; i < m_BufferSize; ++i)
			m_Buffer[i] = buffer.m_Buffer[i];

		m_WriteOffset = buffer.m_WriteOffset;
		m_ReadOffset = buffer.m_ReadOffset;
		m_ElementsAvailable = buffer.m_ElementsAvailable;
		return *this;
	}

	RingBufferV1& operator=(RingBufferV1&& buffer) noexcept
	{
		if (&buffer == this)
			return *this;

		m_Buffer = buffer.m_Buffer;
		m_BufferSize = buffer.m_BufferSize;
		m_WriteOffset = buffer.m_WriteOffset;
		m_ReadOffset = buffer.m_ReadOffset;
		m_ElementsAvailable = buffer.m_ElementsAvailable;

		buffer.m_Buffer = nullptr;
		buffer.m_BufferSize = 0;
		buffer.m_WriteOffset = 0;
		buffer.m_ReadOffset = 0;
		buffer.m_ElementsAvailable = 0;

		return *this;
	}


	//				[GETTERS]

	size_t Size() const noexcept { return m_BufferSize; }
	size_t ElementsStored() const noexcept { return m_BufferSize - m_ElementsAvailable; }
	size_t ElementsAvailable() const noexcept { return m_ElementsAvailable; }


	//				[UTILITY METHODS]

	bool Put(const T* data, size_t dataSize)
	{
		if (m_ElementsAvailable < dataSize || data == nullptr)
			return false;

		if (dataSize == 0)
			return true;

		for (int i{ 0 }; i < dataSize; ++i)
			m_Buffer[(m_WriteOffset + i) % m_BufferSize] = data[i];

		m_WriteOffset = (m_WriteOffset + dataSize) % m_BufferSize;
		m_ElementsAvailable -= dataSize;
		return true;
	}

	bool Get(T* outsideStorage, size_t readSize)
	{
		if (m_BufferSize - m_ElementsAvailable < readSize || outsideStorage == nullptr)
			return false;

		if (readSize == 0)
			return true;

		for (int i{ 0 }; i < readSize; ++i)
			outsideStorage[i] = m_Buffer[(m_ReadOffset + i) % m_BufferSize];

		m_ReadOffset = (m_ReadOffset + readSize) % m_BufferSize;
		m_ElementsAvailable += readSize;
		return true;
	}


private:
	//				[PRIVATE MEMBERS]

	T* m_Buffer;
	size_t m_BufferSize;
	size_t m_WriteOffset;
	size_t m_ReadOffset;
	size_t m_ElementsAvailable;
};


// Ring-buffer implementation, version 2.
// Pros:	easy-to-use templated class for working with any underlaying type;
//			can read and write data only per element;
//			slightly easy to maintain because of safe underlaying std::vector<T> type that handles memory automatically
//			references and std::vector<T> makes it less error-prone
// Cons:	can work slightly slower than RingBufferV1
//			per element reading and writing to the buffer can be a little inconvenient
template<typename T>
class RingBufferV2
{
public:
	//				[CONSTRUCTORS]

	explicit RingBufferV2(size_t bufferSize)
		: m_Buffer(bufferSize)
		, m_ElementsStored{ 0 }
		, m_ReadOffset{ 0 }
		, m_WriteOffset{ 0 }
	{}

	RingBufferV2(const RingBufferV2& buffer) = default;
	RingBufferV2(RingBufferV2&& buffer) noexcept
		: m_WriteOffset{ buffer.m_WriteOffset }
		, m_ReadOffset{ buffer.m_ReadOffset }
		, m_ElementsStored{ buffer.m_ElementsStored }
	{
		m_Buffer = std::move_if_noexcept(buffer.m_Buffer);
		buffer.m_WriteOffset = 0;
		buffer.m_ReadOffset = 0;
		buffer.m_ElementsStored = 0;
	}


	//				[OPERATORS}

	RingBufferV2& operator=(const RingBufferV2& buffer) = default;
	RingBufferV2& operator=(RingBufferV2&& buffer) noexcept
	{
		m_Buffer = std::move_if_noexcept(buffer.m_Buffer);
		m_WriteOffset = buffer.m_WriteOffset;
		m_ReadOffset = buffer.m_ReadOffset;
		m_ElementsStored = buffer.m_ElementsStored;
		buffer.m_WriteOffset = 0;
		buffer.m_ReadOffset = 0;
		buffer.m_ElementsStored = 0;
	}


	//				[GETTERS]

	size_t Size() const noexcept { return m_Buffer.size(); }
	size_t ElementsStored() const noexcept { return m_ElementsStored; }
	size_t ElementsAvailable() const noexcept { return m_Buffer.size() - m_ElementsStored; }


	//				[UTILITY METHODS]

	bool Put(const T& data) noexcept
	{
		if (m_ElementsStored == m_Buffer.size())
			return false;

		m_Buffer[m_WriteOffset] = data;

		m_WriteOffset = (m_WriteOffset + 1) % m_Buffer.size();
		m_ElementsStored++;
	}

	bool Get(T& outsideStorage) noexcept
	{
		if (m_ElementsStored == 0)
			return false;

		outsideStorage = m_Buffer[m_ReadOffset];

		m_ReadOffset = (m_ReadOffset + 1) % m_Buffer.size();
		m_ElementsStored--;
	}


private:
	//				[PRIVATE MEMBERS]

	std::vector<T> m_Buffer;
	size_t m_WriteOffset;
	size_t m_ReadOffset;
	size_t m_ElementsStored;
};