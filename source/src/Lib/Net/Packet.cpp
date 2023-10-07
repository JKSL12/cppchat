#include "pch.h"
#include "Packet.h"

#define MALLOC(size)			(BYTE*)POOL_##size::malloc(); dealloc_data = POOL_##size::free; m_datasize = size;

namespace NET
{

	void CPacket::generate_data(INT32 i_size)
	{

#define TYPEDEF_SIZED_POOL_TYPE(size)	\
	typedef boost::singleton_pool<NET::CPacket::TAG_DATA,	\
		NET::CPacket::size,	\
		boost::default_user_allocator_new_delete,	\
		boost::details::pool::default_mutex,	\
		NET::CPacket::POOL_SIZE, NET::CPacket::POOL_SIZE> POOL_##size;

		TYPEDEF_SIZED_POOL_TYPE(SIZE_8K);
		TYPEDEF_SIZED_POOL_TYPE(SIZE_16K);
		TYPEDEF_SIZED_POOL_TYPE(SIZE_32K);
		TYPEDEF_SIZED_POOL_TYPE(SIZE_64K);
		TYPEDEF_SIZED_POOL_TYPE(SIZE_128K);
		TYPEDEF_SIZED_POOL_TYPE(SIZE_256K);
		TYPEDEF_SIZED_POOL_TYPE(SIZE_512K);
#undef TYPEDEF_SIZED_POOL_TYPE

		if (nullptr != m_data)
			return;

		if (SIZE_8K >= i_size + HEADER_SIZE)
		{
			m_data = MALLOC(SIZE_8K);
		}
		else if (SIZE_16K >= i_size + HEADER_SIZE)
		{
			m_data = MALLOC(SIZE_16K);
		}
		else if (SIZE_32K >= i_size + HEADER_SIZE)
		{
			m_data = MALLOC(SIZE_32K);
		}
		else if (SIZE_64K >= i_size + HEADER_SIZE)
		{
			m_data = MALLOC(SIZE_64K);
		}
		else if (SIZE_128K >= i_size + HEADER_SIZE)
		{
			m_data = MALLOC(SIZE_128K);
		}
		else if (SIZE_256K >= i_size + HEADER_SIZE)
		{
			m_data = MALLOC(SIZE_256K);
		}
		else if (SIZE_512K >= i_size + HEADER_SIZE)
		{
			m_data = MALLOC(SIZE_512K);
		}
		else
		{
			m_datasize = i_size + HEADER_SIZE;
			m_data = new BYTE[m_datasize];
			dealloc_data = ::operator delete[];
		}
	}

	void CPacket::AddSize(INT32 size)
	{
		if (nullptr == m_data)
			return;

		Header()->size += size;

		if (PACKET_BODY_SIZE <= Header()->size)
		{
			*((int*)0) = 0;
		}
	}

	void CPacket::Resize()
	{
		BYTE* old_data = m_data;
		auto old_dealloc_func = dealloc_data;
		auto old_packet_size = PACKET_SIZE();

		// generate_data는 (m_data == nullptr)를 요구한다
		m_data = nullptr;
		generate_data(old_packet_size);
		memcpy(m_data, old_data, HEADER_SIZE);

		if (old_dealloc_func != nullptr)
			old_dealloc_func(old_data);
	}
}