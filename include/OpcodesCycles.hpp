#ifndef OPCODES_CYCLES_HEADER
#define OPCODES_CYCLES_HEADER

#include <cstdint>

static constexpr uint8_t ADD_ADC_SUB_SBB_CMP_Cycles{ 4 };

static constexpr uint8_t INR_DCR_Cycles{ 5 };

static constexpr uint8_t ANA_ORA_XRA_Cycles{ 4 };

static constexpr uint8_t RLC_RRC_RAL_RAR_Cycles{ 4 };

static constexpr uint8_t STC_DAA_CMA_CMC_Cycles{ 4 };

static constexpr uint8_t MOV_R_R_Cycles{ 5 };

static constexpr uint8_t MVI_R_d8_Cycles{ 7 };

static constexpr uint8_t MOV_M_R_Cycles{ 7 };

#endif // !OPCODES_CYCLES_HEADER