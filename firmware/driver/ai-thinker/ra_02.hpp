/*
 * Copyright (c) 2023, Lucas Mösch
 * All Rights Reserved.
 */
// ----------------------------------------------------------------------------
#ifndef RA_02_HPP
#define RA_02_HPP

#include "../lora/semtec/sx127x/sx127x.hpp"

namespace modm
{

template <typename SpiMaster, typename Cs>
class Ra02 : public SX127x<SpiMaster, Cs>
{
public:
	Ra02();
};

}

#include "ra_02_impl.hpp"

#endif
