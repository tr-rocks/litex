
#include <generated/csr.h>

#ifndef __SDRAM_RH_TEST_H
#define __SDRAM_RH_TEST_H

// Output strings
#define OUTPUT_STR_DISABLED "\nRefresh disabled for Rowhammmer Test\n"
#define OUTPUT_STR_ENABLED "\nRefresh rate enabled, set to %ld for Rowhammmer Test\n"
#define OUTPUT_STR_SUMMARY "\n\nPrinting a summary of settings for the Rowhammer Test\n"
#define OUTPUT_SEPARATER_TITLE_STR "\n########################\n# %s\n########################\n"
#define OUTPUT_ROW_BANK_COL_INFO "\n\nRow (bits: %ld), Bank (bits: %ld), Column (bits: %ld)"
#define OUTPUT_ADDR_DATA_WIDTH_INFO "\n\nAddress width: %ld, Data width: %ld\n\n"

// Constants
#define NUM_SETS_ATTACK_ADDR 20
#define DATA_WIDTH_32_BIT 32
#define MAX_ADDR_TO_SET_FREQ 10
#define MIN_TIMER_ADDRESS 2
#define MAX_TIMER_ADDRESS 7
#define TRUE_CONST 1
#define FALSE_CONST 0
#define BYTE_INTERVALS 32
#define TIMER_1_ADDR 2 // Rowhammer cycle counter addr for hammer states 1-2
#define TIMER_2_ADDR 3 // Rowhammer cycle counter addr for hammer states 3-4
#define TIMER_3_ADDR 4 // Rowhammer cycle counter addr for hammer states 5-6
#define TIMER_4_ADDR 5 // Rowhammer cycle counter addr for hammer states 7-8
#define TIMER_5_ADDR 6 // Rowhammer cycle counter addr for hammer states 9-10
#define TIMER_CYCLES_ADDR 7 // All 20 states cycle counter addr
#define ERROR_VAL_RETURN 0xffffffff


// Feedback State Sections
#define RH_IDLE_STATE 0x100
#define RH_WRITE_FILL_INIT_STATE 0x200
#define RH_READ_CHECK_STATE 0x400
#define RH_READ_SEND_ERRORS_STATE 0x800
#define RH_INIT_SETTINGS_STATE 0x1000
#define RH_ROWHAMMER_STATE 0x2000
#define RH_RESET_SETTNGS_STATE 0x4000
#define RH_FINAL_CHECK 0x8000

// Feedback State Values
#define RH_FIRST_STATE 0x0
#define RH_SECOND_STATE 0x1
#define RH_THIRD_STATE 0x2
#define RH_FOURTH_STATE 0x3
#define RH_FIFTH_STATE 0x4
#define RH_SIXTH_STATE 0x5
#define RH_SEVENTH_STATE 0x6
#define RH_EIGHTH_STATE 0x7
#define RH_NINTH_STATE 0x8
#define RH_TENTH_STATE 0x9
#define RH_ELEVENTH_STATE 0xa
#define RH_TWELFTH_STATE 0xb
#define RH_THIRTEENTH_STATE 0xc
#define RH_FOURTEENTH_STATE 0xd
#define RH_FIFTEENTH_STATE 0xe
#define RH_SIXTEENTH_STATE 0xf
#define RH_SEVENTEENTH_STATE 0x10
#define RH_EIGHTEENTH_STATE 0x11
#define RH_NINETEENTH_STATE 0x12
#define RH_TWENTIETH_STATE 0x13

/*
Display the status of the refresh rates. 
Used to summarize status of the refresh registers
*/
void sdram_rhtest_ref_rate_display(void);

/*
Print all the settings necessary for the rowhammer test
*/
void sdram_rhtest_summarize_test_params(void);

/*
Set the corresponding address and freq count
*/
void sdram_set_addr_freq(uint32_t order_count, uint32_t addr_to_set, uint32_t freq_count, uint32_t num_addrs_attack_sig_val);

/*
Get the number of addresses assigned to attack
*/
uint32_t sdram_get_num_addrs_attack_sig(void);

/*
Set the new number of attacked addresses
*/
void sdram_set_num_addrs_attack_sig(uint32_t new_val);

/*
Get values in CSR registers from a certain rowhammer address
*/
uint32_t sdram_get_value_addr_sig(uint32_t input_addr);

/*
Set of commands to set address and frequency for rowhammer test
*/
void sdram_set_order_addr_sig(uint32_t order_count, uint32_t addr_to_set, uint32_t freq_count);

/*
Method to show address and frequency of all sets being used
*/
void sdram_show_addr_freq(void);

/*
Pop an address at the end of the row hammer attack sequence
*/
void sdram_pop_addr_freq(void);

/*
Set data pattern
*/
void sdram_set_data_pattern(uint32_t input_pattern, uint32_t data_sel);

/*
Return timer info (timers are 2-6: pair timers 1-5 respectively, 7: entire cycle counter)
*/
uint32_t sdram_get_timer_sigs(uint32_t addr_to_set);

/*
Set timer info (timers are 2-6: pair timers 1-5 respectively, 7: entire cycle counter)
*/
void sdram_set_timer_sigs(uint32_t input_val, uint32_t addr_to_set);

/*
Display the data pattern
*/
void show_data_pattern(void);

/*
Display the auto precharge setting
*/
void show_auto_precharge(void);

/*
Run the row hammer test
*/
void run_rowhammer_test(void);

/*
See row bits, bank bits, col bits
*/
void rh_test_addr_info(void);

/*
Extract row, bank, col from address helper functions
*/
uint32_t extract_bank_from_addr(uint32_t addr, uint32_t bankbits, uint32_t colbits);
uint32_t extract_col_from_addr(uint32_t addr, uint32_t colbits);
uint32_t extract_row_from_addr(uint32_t addr, uint32_t bankbits, uint32_t colbits);

#endif // __SDRAM_RH_TEST_H