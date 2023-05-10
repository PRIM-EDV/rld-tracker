#ifndef RA_02_HPP
#define RA_02_HPP

#include "sx127x.hpp"

namespace modm
{

template <typename SpiMaster, typename Cs>
class Ra02 : public SX127x<SpiMaster, Cs>
{
public:
	Ra02();

	ResumableResult<void>
	initialize(frequency_t freq, sx127x::SpreadingFactor sf, sx127x::SignalBandwidth bw, sx127x::ErrorCodingRate cr);
};

}

#include "ra_02_impl.hpp"

#endif
