/*
 *  des.h
 *  ctech
 *
 *  Created by Steven Xu on 1/8/09.
 *  Copyright 2009 dreamwork.cn. All rights reserved.
 *
 */

#ifndef CTECH_UTILITY_DES_H_
#define CTECH_UTILITY_DES_H_

#include <cstddef>
#include <boost/cstdint.hpp>

namespace ctech
{
	namespace utility
	{
		using boost::uint32_t;
		using boost::uint8_t;
		
		namespace detail
		{
			extern uint32_t enc_key[];
			extern uint32_t dec_key[];
			
			void des(uint32_t* buffer, const uint32_t* key);
		}
		
		inline void des_encrypt(uint8_t* buffer)
		{
			detail::des(reinterpret_cast<uint32_t*>(buffer), detail::enc_key);
		}
		
		inline void des_decrypt(uint8_t* buffer)
		{
			detail::des(reinterpret_cast<uint32_t*>(buffer), detail::dec_key);
		}
	}
}

#endif