// SPDX-License-Identifier: BSD-Source-Code

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <libbase/memtest.h>

#include <generated/soc.h>
#include <generated/csr.h>
#include <generated/mem.h>
#include <libbase/i2c.h>

#include <liblitedram/sdram.h>
#include <liblitedram/sdram_spd.h>
#include <liblitedram/bist.h>
#include <liblitedram/accessors.h>
#include <liblitedram/rh_test.h>

#include "../command.h"
#include "../helpers.h"

/**
 * Command "sdram_bist"
 *
 * Run SDRAM Build-In Self-Test
 *
 */
#if defined(CSR_SDRAM_GENERATOR_BASE) && defined(CSR_SDRAM_CHECKER_BASE)
static void sdram_bist_handler(int nb_params, char **params)
{
	char *c;
	int burst_length;
	int random;
	if (nb_params < 2) {
		printf("sdram_bist <burst_length> <random>");
		return;
	}
	burst_length = strtoul(params[0], &c, 0);
	if (*c != 0) {
		printf("Incorrect burst_length");
		return;
	}
	random = strtoul(params[1], &c, 0);
	if (*c != 0) {
		printf("Incorrect random");
		return;
	}
	sdram_bist(burst_length, random);
}
define_command(sdram_bist, sdram_bist_handler, "Run SDRAM Build-In Self-Test", LITEDRAM_CMDS);
#endif

/**
 * Command "sdram_hw_test"
 *
 * Run SDRAM HW-accelerated memtest
 *
 */
#if defined(CSR_SDRAM_GENERATOR_BASE) && defined(CSR_SDRAM_CHECKER_BASE)
static void sdram_hw_test_handler(int nb_params, char **params)
{
	char *c;
	uint64_t origin;
	uint64_t size;
	uint64_t burst_length = 1;
	if (nb_params < 2) {
		printf("sdram_hw_test <origin> <size> [<burst_length>]");
		return;
	}
	origin = strtoull(params[0], &c, 0);
	if (*c != 0) {
		printf("Incorrect origin");
		return;
	}
	size = strtoull(params[1], &c, 0);
	if (*c != 0) {
		printf("Incorrect size");
		return;
	}
	if (nb_params > 2) {
		burst_length = strtoull(params[2], &c, 0);
		if (*c != 0) {
			printf("Incorrect burst length");
			return;
		}
	}
	int errors = sdram_hw_test(origin, size, burst_length);
	printf("%d errors found\n", errors);
}
define_command(sdram_hw_test, sdram_hw_test_handler, "Run SDRAM HW-accelerated memtest", LITEDRAM_CMDS);
#endif

#ifdef CSR_DDRPHY_RDPHASE_ADDR
/**
 * Command "sdram_force_rdphase"
 *
 * Force read phase
 *
 */
static void sdram_force_rdphase_handler(int nb_params, char **params)
{
	char *c;
	int phase;
	if (nb_params < 1) {
		printf("sdram_force_rdphase <phase>");
		return;
	}
	phase = strtoul(params[0], &c, 0);
	if (*c != 0) {
		printf("Incorrect phase");
		return;
	}
	printf("Forcing read phase to %d\n", phase);
	ddrphy_rdphase_write(phase);
}
define_command(sdram_force_rdphase, sdram_force_rdphase_handler, "Force read phase", LITEDRAM_CMDS);
#endif

#ifdef CSR_DDRPHY_WRPHASE_ADDR
/**
 * Command "sdram_force_wrphase"
 *
 * Force write phase
 *
 */
static void sdram_force_wrphase_handler(int nb_params, char **params)
{
	char *c;
	int phase;
	if (nb_params < 1) {
		printf("sdram_force_wrphase <phase>");
		return;
	}
	phase = strtoul(params[0], &c, 0);
	if (*c != 0) {
		printf("Incorrect phase");
		return;
	}
	printf("Forcing write phase to %d\n", phase);
	ddrphy_wrphase_write(phase);
}
define_command(sdram_force_wrphase, sdram_force_wrphase_handler, "Force write phase", LITEDRAM_CMDS);
#endif

#ifdef CSR_DDRPHY_CDLY_RST_ADDR

/**
 * Command "sdram_rst_cmd_delay"
 *
 * Reset write leveling Cmd delay
 *
 */
#if defined(CSR_SDRAM_BASE) && defined(CSR_DDRPHY_BASE)
static void sdram_rst_cmd_delay_handler(int nb_params, char **params)
{
	sdram_software_control_on();
	sdram_write_leveling_rst_cmd_delay(1);
	sdram_software_control_off();
}
define_command(sdram_rst_cmd_delay, sdram_rst_cmd_delay_handler, "Reset write leveling Cmd delay", LITEDRAM_CMDS);
#endif

/**
 * Command "sdram_force_cmd_delay"
 *
 * Force write leveling Cmd delay
 *
 */
#if defined(CSR_SDRAM_BASE) && defined(CSR_DDRPHY_BASE)
static void sdram_force_cmd_delay_handler(int nb_params, char **params)
{
	char *c;
	int taps;
	if (nb_params < 1) {
		printf("sdram_force_cmd_delay <taps>");
		return;
	}
	taps = strtoul(params[0], &c, 0);
	if (*c != 0) {
		printf("Incorrect taps");
		return;
	}
	sdram_software_control_on();
	sdram_write_leveling_force_cmd_delay(taps, 1);
	sdram_software_control_off();
}
define_command(sdram_force_cmd_delay, sdram_force_cmd_delay_handler, "Force write leveling Cmd delay", LITEDRAM_CMDS);
#endif

#endif

#if defined(CSR_SDRAM_BASE)
/**
 * Command "sdram_init"
 *
 * Initialize SDRAM (Init + Calibration)
 *
 */
define_command(sdram_init, sdram_init, "Initialize SDRAM (Init + Calibration)", LITEDRAM_CMDS);

/**
 * Command "sdram_test"
 *
 * Test SDRAM
 *
 */
static void sdram_test_handler(int nb_params, char **params)
{
	memtest((unsigned int *)MAIN_RAM_BASE, MAIN_RAM_SIZE/32);
}
define_command(sdram_test, sdram_test_handler, "Test SDRAM", LITEDRAM_CMDS);

/**
 * Command "sdram_cal"
 *
 * Calibrate SDRAM
 *
 */
#if defined(CSR_DDRPHY_BASE)
static void sdram_cal_handler(int nb_params, char **params)
{
	sdram_software_control_on();
	sdram_leveling();
	sdram_software_control_off();
}
define_command(sdram_cal, sdram_cal_handler, "Calibrate SDRAM", LITEDRAM_CMDS);
#endif

#ifdef SDRAM_PHY_WRITE_LEVELING_CAPABLE

/**
 * Command "sdram_rst_dat_delay"
 *
 * Reset write leveling Dat delay
 *
 */
#if defined(CSR_DDRPHY_BASE)
static void sdram_rst_dat_delay_handler(int nb_params, char **params)
{
	char *c;
	int module;
	if (nb_params < 1) {
		printf("sdram_rst_dat_delay <module>");
		return;
	}
	module = strtoul(params[0], &c, 0);
	if (*c != 0) {
		printf("Incorrect module");
		return;
	}
	sdram_software_control_on();
	sdram_write_leveling_rst_dat_delay(module, 1);
	sdram_software_control_off();
}
define_command(sdram_rst_dat_delay, sdram_rst_dat_delay_handler, "Reset write leveling Dat delay", LITEDRAM_CMDS);
#endif

/**
 * Command "sdram_force_dat_delay"
 *
 * Force write leveling Dat delay
 *
 */
#if defined(CSR_DDRPHY_BASE)
static void sdram_force_dat_delay_handler(int nb_params, char **params)
{
	char *c;
	int module;
	int taps;
	if (nb_params < 2) {
		printf("sdram_force_dat_delay <module> <taps>");
		return;
	}
	module = strtoul(params[0], &c, 0);
	if (*c != 0) {
		printf("Incorrect module");
		return;
	}
	taps = strtoul(params[1], &c, 0);
	if (*c != 0) {
		printf("Incorrect taps");
		return;
	}
	sdram_software_control_on();
	sdram_write_leveling_force_dat_delay(module, taps, 1);
	sdram_software_control_off();
}
define_command(sdram_force_dat_delay, sdram_force_dat_delay_handler, "Force write leveling Dat delay", LITEDRAM_CMDS);
#endif /* defined(CSR_SDRAM_BASE) && defined(CSR_DDRPHY_BASE) */

#endif /* SDRAM_PHY_WRITE_LEVELING_CAPABLE */

#if defined(SDRAM_PHY_BITSLIPS) && defined(SDRAM_PHY_WRITE_LEVELING_CAPABLE)
/**
 * Command "sdram_rst_bitslip"
 *
 * Reset write leveling Bitslip
 *
 */
#if defined(CSR_DDRPHY_BASE)
static void sdram_rst_bitslip_handler(int nb_params, char **params)
{
	char *c;
	int module;
	if (nb_params < 1) {
		printf("sdram_rst_bitslip <module>");
		return;
	}
	module = strtoul(params[0], &c, 0);
	if (*c != 0) {
		printf("Incorrect module");
		return;
	}
	sdram_software_control_on();
	sdram_write_leveling_rst_bitslip(module, 1);
	sdram_software_control_off();
}
define_command(sdram_rst_bitslip, sdram_rst_bitslip_handler, "Reset write leveling Bitslip", LITEDRAM_CMDS);
#endif

/**
 * Command "sdram_force_bitslip"
 *
 * Force write leveling Bitslip
 *
 */
#if defined(CSR_DDRPHY_BASE)
static void sdram_force_bitslip_handler(int nb_params, char **params)
{
	char *c;
	int module;
	int bitslip;
	if (nb_params < 2) {
		printf("sdram_force_bitslip <module> <bitslip>");
		return;
	}
	module = strtoul(params[0], &c, 0);
	if (*c != 0) {
		printf("Incorrect module");
		return;
	}
	bitslip = strtoul(params[1], &c, 0);
	if (*c != 0) {
		printf("Incorrect bitslip");
		return;
	}
	sdram_software_control_on();
	sdram_write_leveling_force_bitslip(module, bitslip, 1);
	sdram_software_control_off();
}
define_command(sdram_force_bitslip, sdram_force_bitslip_handler, "Force write leveling Bitslip", LITEDRAM_CMDS);
#endif

#endif /* defined(SDRAM_PHY_BITSLIPS) && defined(SDRAM_PHY_WRITE_LEVELING_CAPABLE) */

/**
 * Command "sdram_mr_write"
 *
 * Write SDRAM Mode Register
 *
 */
static void sdram_mr_write_handler(int nb_params, char **params)
{
	char *c;
	uint8_t reg;
	uint16_t value;

	if (nb_params < 2) {
		printf("sdram_mr_write <reg> <value>");
		return;
	}
	reg = strtoul(params[0], &c, 0);
	if (*c != 0) {
		printf("Incorrect reg");
		return;
	}
	value = strtoul(params[1], &c, 0);
	if (*c != 0) {
		printf("Incorrect value");
		return;
	}
	sdram_software_control_on();
	printf("Writing 0x%04x to MR%d\n", value, reg);
	sdram_mode_register_write(reg, value);
	sdram_software_control_off();
}
define_command(sdram_mr_write, sdram_mr_write_handler, "Write SDRAM Mode Register", LITEDRAM_CMDS);

#endif /* CSR_SDRAM_BASE */

/**
 * Command "sdram_spd"
 *
 * Read contents of SPD EEPROM memory.
 * SPD address is a 3-bit address defined by the pins A0, A1, A2.
 *
 */
#if defined(CSR_SDRAM_BASE) && defined(CONFIG_HAS_I2C)

static void sdram_spd_handler(int nb_params, char **params)
{
	char *c;
	unsigned char spdaddr;
	unsigned char buf[SDRAM_SPD_SIZE];
	int len = sizeof(buf);

	if (nb_params < 1) {
		printf("sdram_spd <spdaddr>");
		return;
	}

	spdaddr = strtoul(params[0], &c, 0);
	if (*c != 0) {
		printf("Incorrect address");
		return;
	}
	if (spdaddr > 0b111) {
		printf("SPD EEPROM max address is 0b111 (defined by A0, A1, A2 pins)");
		return;
	}

	if (!sdram_read_spd(spdaddr, 0, buf, (uint16_t)len)) {
		printf("Error when reading SPD EEPROM");
		return;
	}

	dump_bytes((unsigned int *) buf, len, 0);

#ifdef SPD_BASE
	{
		int cmp_result;
		cmp_result = memcmp(buf, (void *) SPD_BASE, SPD_SIZE);
		if (cmp_result == 0) {
			printf("Memory contents matches the data used for gateware generation\n");
		} else {
			printf("\nWARNING: memory differs from the data used during gateware generation:\n");
			dump_bytes((void *) SPD_BASE, SPD_SIZE, 0);
		}
	}
#endif
}
define_command(sdram_spd, sdram_spd_handler, "Read SDRAM SPD EEPROM", LITEDRAM_CMDS);
#endif

#ifdef SDRAM_DEBUG
define_command(sdram_debug, sdram_debug, "Run SDRAM debug tests", LITEDRAM_CMDS);
#endif

#ifdef CSR_RH_TEST_BASE
/**
 * Command "sdram_rhtest_ref_rate_set"
 * 
 * Set the refresh rate for the rohammer test
 * 
*/
static void sdram_rhtest_ref_rate_handler(int nb_params, char **params)
{
	char *c;
	uint32_t sdram_refresh_rate;

	if (nb_params < 1) {
		printf("sdram_set_ref_rate_rhtest <refresh_rate>");
		return;
	}
	sdram_refresh_rate = strtoul(params[0], &c, 0);
	if (*c != 0) {
		printf("Incorrect ref rate");
		return;
	}

	// Disable refresh if set to zero, else enable it and set refresh rate
	if (sdram_refresh_rate == 0) {
		rh_test_refresh_enable_csr_write(0);
		printf(OUTPUT_STR_DISABLED);
	} else {
		rh_test_refresh_enable_csr_write(1);
		rh_test_refresh_rate_csr_write(sdram_refresh_rate);
		printf(OUTPUT_STR_ENABLED, sdram_refresh_rate);
	}
}
define_command(sdram_set_ref_rate_rhtest, sdram_rhtest_ref_rate_handler, "Set refresh rate for rh test", LITEDRAM_CMDS);

/**
 * Command "sdram_set_addr_freq"
 * 
 * Set the addresses we want to attack and the number of times we want to attack
 * 
*/
static void sdram_set_addr_freq_handler(int nb_params, char **params)
{
	char *c;
	uint32_t order_val;
	uint32_t addr_val;
	uint32_t freq_val;

	// Value to set from functions
	uint32_t num_addrs_attack_sig_val;

	// Obtain value
	num_addrs_attack_sig_val = sdram_get_num_addrs_attack_sig();

	if (nb_params < 3) {
		

		printf("sdram_set_addr_freq_rhtest <order_val> <addr_val> <freq_val>\n");
		printf("order_val: Number between 0 - 19, choose the order value to set\n");
		printf("Available values are:\n");
		printf("Modify order values: ");
		for (int i = 0; ((i < num_addrs_attack_sig_val) && (i < NUM_SETS_ATTACK_ADDR)); ++i) {
			printf("%d ", i);
		}
		printf("\n");
		if (num_addrs_attack_sig_val < NUM_SETS_ATTACK_ADDR) {
			printf("Add new order value: %ld\n", num_addrs_attack_sig_val);
		}
		printf("addr_val: Address to attack (ex. 0x1f)\n");
		printf("freq_val: Number of times to attack address before moving to next one\n");
		return;
	}
	order_val = strtoul(params[0], &c, 0);
	if (*c != 0) {
		printf("Incorrect order_val");
		return;
	}
	addr_val = strtoul(params[1], &c, 0);
	if (*c != 0) {
		printf("Incorrect addr_val");
		return;
	}
	freq_val = strtoul(params[2], &c, 0);
	if (*c != 0) {
		printf("Incorrect freq_val");
		return;
	} 
	sdram_set_addr_freq(order_val, addr_val, freq_val, num_addrs_attack_sig_val);
}
define_command(sdram_set_addr_freq_rhtest, sdram_set_addr_freq_handler, "Set addresses and freqs for hammering", LITEDRAM_CMDS);

/**
 * Command "sdram_set_cycles"
 * 
 * Set the number of cycles to go through addressess in row hammer test
 * 
*/
static void sdram_set_cycles_handler(int nb_params, char **params)
{
	char *c;
	uint32_t cycle_val;
	uint32_t timer_num = 0;

	if (nb_params < 1) {
		printf("sdram_set_cycles_rhtest <cycles> <timer_num>\n");
		printf("cycles: Number of times to go through all the addresses (32-bit max)\n");
		printf("timer_num (default=0): The timer selected:\n");
		printf(" 0: - Cycles for all 20 states, 1 - 5: Cycles for pairs of states 1-2, 3-4, 5-6, 7-8, 9-10\n\n");
		return;
	}

	cycle_val = strtoul(params[0], &c, 0);
	if (*c != 0) {
		printf("Incorrect cycles");
		return;
	}
	if (nb_params > 1) {
		timer_num = strtoul(params[1], &c, 0);
		if (*c != 0) {
			printf("Incorrect burst length");
			return;
		}
	}

	// Check bounds of timer number
	if (timer_num == 0) {
		timer_num = TIMER_CYCLES_ADDR;
	} else if (timer_num < TIMER_5_ADDR) {
		timer_num += 1; // To match the address corresponding to variables TIMER_1_ADDR to TIMER_5_ADDR
	} else {
		printf("Error: Timer %ld not in bounds\n\n", timer_num);
		return;
	}

	// Set the timer
	printf("Input args: cycle val: %ld, timer_num: %ld\n", cycle_val, timer_num);
	sdram_set_timer_sigs(cycle_val, timer_num);
	printf("Timer %ld set to %ld\n", timer_num, sdram_get_timer_sigs(timer_num));
}
define_command(sdram_set_cycles_rhtest, sdram_set_cycles_handler, "Set the number of times to run through row hammer sequence", LITEDRAM_CMDS);

/**
 * Command "set_auto_precharge"
 * 
 * Set auto precharge setting for row hammer test
 * 
*/
static void sdram_set_auto_precharge_handler(int nb_params, char **params)
{
	char *c;
	uint32_t auto_precharge_val;

	if (nb_params < 1) {
		printf("sdram_set_auto_precharge_rhtest <value>\n");
		printf("value: 1 - enable, 0 - disable (for single row hammer)");
		return;
	}

	auto_precharge_val = strtoul(params[0], &c, 0);
	if (*c != 0) {
		printf("Incorrect value");
		return;
	}
	rh_test_auto_precharge_csr_write(auto_precharge_val);
}
define_command(sdram_set_auto_precharge_rhtest, sdram_set_auto_precharge_handler, "Enable/Disable auto precharge (for single row hammer)", LITEDRAM_CMDS);


/**
 * Command "sdram_rhtest_summary"
 * 
 * Display settings for rowhammer test
 * 
*/
static void sdram_rhtest_summary(int nb_params, char **params)
{
	sdram_rhtest_summarize_test_params();
}
define_command(sdram_summary_rhtest, sdram_rhtest_summary, "Display a summary of settings for the Row Hammer test", LITEDRAM_CMDS);

/**
 * Command "sdram_pop_addr_freq_rhtest"
 * 
 * Run Rowhammer Test
 * 
*/
static void sdram_pop_addr_freq_helper(int nb_params, char **params) 
{
	sdram_pop_addr_freq();
}
define_command(sdram_pop_addr_freq_rhtest, sdram_pop_addr_freq_helper, "Pop the last address to attack", LITEDRAM_CMDS);

/**
 * Command "sdram_info_rhtest"
 * 
 * Gives information about the address organization in rhtest
 * 
*/
static void sdram_info_rhtest_handler(int nb_params, char **params)
{
	rh_test_addr_info();
}
define_command(sdram_info_rhtest, sdram_info_rhtest_handler, "Address info for row hammer test", LITEDRAM_CMDS);

/**
 * Command 'sdram_enable_double_pattern_rhtest
 * 
 * Allow for double data patterns
 * 
*/
static void sdram_enable_double_pattern_handler(int nb_params, char **params)
{
	char *c;
	uint32_t enable_val;
	
	if (nb_params < 1) {
		printf("sdram_enable_double_pattern_rhtest <enable_val>");
		printf("enable_val: 1 to enable two-pattern setting, 0 to only use one-pattern");
		return;
	}

	enable_val = strtoul(params[0], &c, 0);
	if (*c != 0) {
		printf("Incorrect enable_val");
		return;
	}

	rh_test_input_data_double_pattern_setting_csr_write(enable_val);
	if (rh_test_input_data_double_pattern_setting_csr_read()) {
		printf("\nTwo-pattern enabled, val: %ld\n\n", rh_test_input_data_double_pattern_setting_csr_read());
	} else {
		printf("\nOne-pattern enabled, val: %ld\n\n", rh_test_input_data_double_pattern_setting_csr_read());
	}
}
define_command(sdram_enable_doublepat_rhtest, sdram_enable_double_pattern_handler, "Set one- or two-pattern setting", LITEDRAM_CMDS);

/**
 * Command 'sdram_set_pattern_rhtest
 * 
 * Set the pattern for the row hammer test
*/
static void sdram_set_pattern_helper(int nb_params, char **params)
{
	char *c;
	uint32_t pattern_val;
	uint32_t data_sel = 0;

	if (nb_params < 1) {
		printf("sdram_set_pattern_rhtest <pattern_val> <data_sel>\n");
		printf("pattern_val: 32-bit value, replicated to fill data width (data width is %ld)\n", rh_test_data_width_csr_read());
		printf("data_sel [default=0]: 0 for one-pattern or two-pattern even rows, 1 for two-pattern odd rows\n\n");
		return;
	}

	pattern_val = strtoul(params[0], &c, 0);
	if (*c != 0) {
		printf("Incorrect pattern_val");
		return;
	}
	if (nb_params > 1) {
		data_sel = strtoul(params[1], &c, 0);
		if (*c != 0) {
			printf("Incorrect burst length");
			return;
		}
	}

	sdram_set_data_pattern(pattern_val, data_sel);
	show_data_pattern();
}
define_command(sdram_set_pattern_rhtest, sdram_set_pattern_helper, "Set the pattern to use in rowhammer test", LITEDRAM_CMDS);


/**
 * Command 'sdram_run_rhtest'
 * 
 * Run the row hammer test
 * 
*/
static void sdram_run_rhtest_handler(int nb_params, char **params)
{
	char c;

	// Show the current row hammer test settings
	sdram_rhtest_summarize_test_params();

	// Give user an option to quit
	printf("\nProceed? Y/n :");

	c = getchar();

	while ((c != 'y') && (c != 'Y') && (c != 'n') && (c != 'N') && (c != '\n')) {
		printf("\nProceed? Y/n :");
		c = getchar();
	}

	// Skip to the end of the function if user decides, else run the test
	if ((c == 'n') || (c == 'N')) {
		printf("\nExiting\n");
	} else {
		// Run the test
		run_rowhammer_test();
	}
	
}
define_command(sdram_run_rhtest, sdram_run_rhtest_handler, "Start row hammer test", LITEDRAM_CMDS);

#endif //CSR_RH_TEST_BASE

