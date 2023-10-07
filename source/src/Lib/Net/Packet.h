#pragma once

typedef void(*free_func_ptr)(void* const);

namespace NET
{
	class CPacket
	{
	public:
		struct TAG_DATA { };
		enum
		{
			SIZE_8K = 1024 * 8,
			SIZE_16K = 1024 * 16,
			SIZE_32K = 1024 * 32,
			SIZE_64K = 1024 * 64,
			SIZE_128K = 1024 * 128,
			SIZE_256K = 1024 * 256,
			SIZE_512K = 1024 * 512,
		};

	private:
		void generate_data(INT32 i_size);
		free_func_ptr dealloc_data;

	public:
		CPacket()
			: m_refCnt(1)
			, m_bEncrypted(false)
			, dealloc_data(nullptr)
		{
			//default packet data size, 8K
			generate_data(0);
			memset(m_data, 0, sizeof(HEADER));
		}
		CPacket(INT32 type, INT32 size = 0, INT16 param = 0) : m_refCnt(1), m_bEncrypted(false), m_data(nullptr)
			, dealloc_data(nullptr)
		{
			if (0 >= size)
				return;

			generate_data(size);
			Header()->size = size;
			Header()->checksum = 0;
			Header()->type = type;
			Header()->param = param;
			Header()->flat = 0;
		}
		CPacket(HEADER& header) : m_refCnt(1), m_bEncrypted(false), m_data(nullptr)
			, dealloc_data(nullptr)
		{
			if (0 >= header.size)
				return;

			generate_data(header.size);
			*Header() = header;
		}
		~CPacket()
		{
			if (nullptr != m_data)
			{
				dealloc_data(m_data);
				m_data = nullptr;
				dealloc_data = nullptr;
			}
		}

	public:
		HEADER* Header() { return (nullptr == m_data) ? nullptr : (HEADER*)m_data; }

		void* Body() { return (nullptr == m_data) ? nullptr : (void*)(m_data + HEADER_SIZE); }
		BYTE* Data() { return m_data; }

		INT32	Size() { return (nullptr == m_data) ? 0 : Header()->size; }
		INT32	Type() { return (nullptr == m_data) ? 0 : Header()->type; }
		INT16	Param() { return (nullptr == m_data) ? 0 : Header()->param; }
		UINT8	Flat() { return (nullptr == m_data) ? 0 : Header()->flat; }

		void	AddSize(INT32 size);
		void	Resize();

		INT32	PACKET_SIZE() { return Size() + HEADER_SIZE; }
		INT32	DATA_SIZE() const { return m_datasize; }

		LONG	AddRef() { return InterlockedIncrement(&m_refCnt); }
		LONG	DelRef() { return InterlockedDecrement(&m_refCnt); }

	private:
		volatile LONG	m_refCnt;
		bool			m_bEncrypted;
		BYTE*			m_data;
		INT32			m_datasize;

		enum { POOL_SIZE = 1000 };
	};
}

