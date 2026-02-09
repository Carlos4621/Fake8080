#ifndef OPCODES_CYCLES_HEADER
#define OPCODES_CYCLES_HEADER

#include <cstdint>

static constexpr uint8_t ADD_ADC_SUB_SBB_CMP_R_Cycles{ 4 };

static constexpr uint8_t ADD_ADC_SUB_SBB_CMP_M_Cycles{ 7 };

static constexpr uint8_t INR_DCR_R_Cycles{ 5 };

static constexpr uint8_t INR_DCR_M_Cycles{ 10 };

static constexpr uint8_t ANA_ORA_XRA_R_Cycles{ 4 };

static constexpr uint8_t ANA_ORA_XRA_M_Cycles{ 7 };

static constexpr uint8_t RLC_RRC_RAL_RAR_Cycles{ 4 };

static constexpr uint8_t STC_DAA_CMA_CMC_Cycles{ 4 };

static constexpr uint8_t MOV_R_R_Cycles{ 5 };

static constexpr uint8_t MVI_R_d8_Cycles{ 7 };

static constexpr uint8_t MOV_M_R_Cycles{ 7 };

static constexpr uint8_t MOV_R_M_Cycles{ 7 };

static constexpr uint8_t MVI_M_d8_Cycles{ 10 };

static constexpr uint8_t INX_DCX_RR_Cycles{ 5 };

static constexpr uint8_t DAD_RR_Cycles{ 10 };

static constexpr uint8_t LXI_Cycles{ 10 };

#endif // !OPCODES_CYCLES_HEADER