#pragma once

namespace ecg {

struct ECGSample {

};

static_assert(sizeof(ECGSample) <= sizeof(uint16_t), "ECGSample is too big");

} // namespace ecg