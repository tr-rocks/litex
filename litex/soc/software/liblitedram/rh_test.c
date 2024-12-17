

#include <stdio.h>
#include <generated/csr.h>
#include <liblitedram/rh_test.h>

uint32_t extract_bank_from_addr(uint32_t addr, uint32_t bankbits, uint32_t colbits) {

    // Extract bank and return it
    uint32_t new_addr = addr >> colbits;
    uint32_t bank_mask = (~0) << bankbits;
    return (new_addr & (~bank_mask));
}

uint32_t extract_col_from_addr(uint32_t addr, uint32_t colbits) {
    uint32_t col_mask = (~0) << colbits;
    return (addr & (~col_mask));
}

uint32_t extract_row_from_addr(uint32_t addr, uint32_t bankbits, uint32_t colbits) {
    return addr >> (bankbits + colbits);
}

uint32_t sdram_get_num_addrs_attack_sig(void) {

	// We want to get data, not send it
	rh_test_addr_to_set_set_not_get_csr_write(FALSE_CONST);

	// Access the signal holding the number of sets of addresses/freqs already set
	rh_test_addr_to_set_sel_val_csr_write(NUM_SETS_ATTACK_ADDR);

	// Start the process of getting data into CSR register
	rh_test_addr_to_set_start_fsm_csr_write(TRUE_CONST);

	// Wait for fsm to start
	while (rh_test_addr_to_set_start_prev_csr_read() == FALSE_CONST) {};

	// Finish it
	rh_test_addr_to_set_start_fsm_csr_write(FALSE_CONST);

	// Wait for fsm to finish. Data should now be in addr_to_set_freq_out_csr
	while (rh_test_addr_to_set_start_prev_csr_read() == TRUE_CONST) {};

	// Obtain value
	return rh_test_addr_to_set_freq_out_csr_read();

}

void sdram_set_num_addrs_attack_sig(uint32_t new_val) {

    // Change new val
    rh_test_addr_to_set_freq_csr_write(new_val);

	// We want to send address and freq count data to set number.
	rh_test_addr_to_set_set_not_get_csr_write(TRUE_CONST);

	// Access the signal holding the number of sets of addresses/freqs already set
	rh_test_addr_to_set_sel_val_csr_write(NUM_SETS_ATTACK_ADDR);

	// Start the process of getting data into CSR register
	rh_test_addr_to_set_start_fsm_csr_write(TRUE_CONST);

	// Wait for fsm to start
	while (rh_test_addr_to_set_start_prev_csr_read() == FALSE_CONST) {};

	// Finish it
	rh_test_addr_to_set_start_fsm_csr_write(FALSE_CONST);

	// Wait for fsm to finish. Data should now be in addr_to_set_freq_out_csr
	while (rh_test_addr_to_set_start_prev_csr_read() == TRUE_CONST) {};

}

uint32_t sdram_get_value_addr_sig(uint32_t input_addr) {

	// We want to get data, not send it
	rh_test_addr_to_set_set_not_get_csr_write(FALSE_CONST);

	// Access the signal holding the number of sets of addresses/freqs already set
	rh_test_addr_to_set_sel_val_csr_write(input_addr);

	// Start the process of getting data into CSR register
	rh_test_addr_to_set_start_fsm_csr_write(TRUE_CONST);

	// Wait for fsm to start
	while (rh_test_addr_to_set_start_prev_csr_read() == FALSE_CONST) {};

	// Finish it
	rh_test_addr_to_set_start_fsm_csr_write(FALSE_CONST);

	// Wait for fsm to finish. Data should now be in addr_to_set_freq_out_csr
	while (rh_test_addr_to_set_start_prev_csr_read() == TRUE_CONST) {};

	// Obtain value
	return rh_test_addr_to_set_freq_out_csr_read();

}

void sdram_set_order_addr_sig(uint32_t order_count, uint32_t addr_to_set, uint32_t freq_count) {

    rh_test_addr_to_set_val_csr_write(addr_to_set);

    // Whatever value the freq csr will be written to even if 
    // the addr_to_set was 10 or above will be ignored anyways,
    // as the freq is hard coded to 1. Below 'if' statement is 
    // possibly redundant.
    if (order_count < MAX_ADDR_TO_SET_FREQ) {
        rh_test_addr_to_set_freq_csr_write(freq_count);
    }

	// We want to send address and freq count data to set number.
	rh_test_addr_to_set_set_not_get_csr_write(TRUE_CONST);

	// Access the signal holding the number of sets of addrs/freqs already set
	rh_test_addr_to_set_sel_val_csr_write(order_count);

	// Start the process of getting data into CSR register
	rh_test_addr_to_set_start_fsm_csr_write(TRUE_CONST);

	// Wait for fsm to start
	while (rh_test_addr_to_set_start_prev_csr_read() == FALSE_CONST) {};

	// Finish it
	rh_test_addr_to_set_start_fsm_csr_write(FALSE_CONST);

	// Wait for fsm to finish. Data should now be in addr_to_set_freq_out_csr
	while (rh_test_addr_to_set_start_prev_csr_read() == TRUE_CONST) {};

}

// Set data or timers
void sdram_set_timer_sigs(uint32_t input_val, uint32_t addr_to_set) {

    // Assert only timers 2-7 are to be set.
    if ((addr_to_set < MIN_TIMER_ADDRESS) || (addr_to_set > MAX_TIMER_ADDRESS)) {
        printf("\nValue of timer should be in range %d - %d\n\n", MIN_TIMER_ADDRESS, MAX_ADDR_TO_SET_FREQ);
        return;
    }

    // Set the timer or data address we want to set
    rh_test_input_data_sel_val_csr_write(addr_to_set);

    // Set the address of the timer
    rh_test_rowhammer_state_cycle_counter_csr_write(input_val);

    // We want to set the timer
    rh_test_input_data_set_not_get_csr_write(TRUE_CONST);

    // Start the fsm
    rh_test_input_data_to_set_start_fsm_csr_write(TRUE_CONST);

    // Wait for the fsm to start
    while (rh_test_input_data_set_start_prev_csr_read() == FALSE_CONST) {};

    // Finish it
    rh_test_input_data_to_set_start_fsm_csr_write(FALSE_CONST);

    // Wait for fsm to finish
    while (rh_test_input_data_set_start_prev_csr_read() == TRUE_CONST) {};
}


// Set data or timers
uint32_t sdram_get_timer_sigs(uint32_t addr_to_set) {

    // Assert only timers 2-7 are to be set.
    if ((addr_to_set < MIN_TIMER_ADDRESS) || (addr_to_set > MAX_TIMER_ADDRESS)) {
        printf("\nValue of timer should be in range %d - %d\n\n", MIN_TIMER_ADDRESS, MAX_TIMER_ADDRESS);
        return ERROR_VAL_RETURN;
    }

    // Set the timer or data address we want to get
    rh_test_input_data_sel_val_csr_write(addr_to_set);

    // We want to get the timer
    rh_test_input_data_set_not_get_csr_write(FALSE_CONST);

    // Start the fsm
    rh_test_input_data_to_set_start_fsm_csr_write(TRUE_CONST);

    // Wait for the fsm to start
    while (rh_test_input_data_set_start_prev_csr_read() == FALSE_CONST) {};

    // Finish it
    rh_test_input_data_to_set_start_fsm_csr_write(FALSE_CONST);

    // Wait for fsm to finish
    while (rh_test_input_data_set_start_prev_csr_read() == TRUE_CONST) {};

    // Return the value
    return rh_test_rowhammer_state_cycle_counter_val_output_csr_read();
}


// Get output for refresh rate settings
void sdram_rhtest_ref_rate_display(void) {

    // Print the status of the registers
    if (rh_test_refresh_enable_csr_read() == 0) {
        printf(OUTPUT_STR_DISABLED);
    } else {
        printf(OUTPUT_STR_ENABLED, rh_test_refresh_rate_csr_read());
    }
}

// Set the address and frequency as desired by User
void sdram_set_addr_freq(uint32_t order_count, uint32_t addr_to_set, uint32_t freq_count, uint32_t num_addrs_attack_sig_val) {

    if ((order_count > num_addrs_attack_sig_val) || (order_count >= NUM_SETS_ATTACK_ADDR)) {
        printf("Option for order value not available. Available options are:\n");
        printf("Modify order values: ");
		for (int i = 0; ((i < num_addrs_attack_sig_val) && (i < NUM_SETS_ATTACK_ADDR)); ++i) {
			printf("%d ", i);
		}
		printf("\n");
		if (num_addrs_attack_sig_val < NUM_SETS_ATTACK_ADDR) {
			printf("Add new order value: %ld\n", num_addrs_attack_sig_val);
		}
        
        return;
    } 

    // If adding new address, increment number of attacked addresses for test
    if (order_count == num_addrs_attack_sig_val) {
        sdram_set_num_addrs_attack_sig(num_addrs_attack_sig_val + 1);
    }
    
    sdram_set_order_addr_sig(order_count, addr_to_set, freq_count);

}

// Delete the last ordered set of addr and freq to be used
void sdram_pop_addr_freq(void) {

    uint32_t num_addrs_attack_sig_val;

	// Obtain value
	num_addrs_attack_sig_val = sdram_get_num_addrs_attack_sig();

    if (num_addrs_attack_sig_val > 0) {
        sdram_set_num_addrs_attack_sig(num_addrs_attack_sig_val - 1);
        printf("Number of addresses: %ld\n\n", num_addrs_attack_sig_val - 1);
    } else {
        printf("No addresses to pop\n\n");
    }

    return;
}

// Show which sets of addresses/freqs will be attacked
void sdram_show_addr_freq(void) {

    printf("Addresses to be attacked: \n");

    uint32_t num_addrs_attack_sig_val;
    uint32_t timer_addr;

	// Obtain value
	num_addrs_attack_sig_val = sdram_get_num_addrs_attack_sig();

    // Should not occur for num_addrs_attack_sig_val to be above 20
    if (num_addrs_attack_sig_val > NUM_SETS_ATTACK_ADDR) {
        printf("Not sure what happened here, \nnum_addrs_attack_sig_val is greater than 20, val: %ld\n", num_addrs_attack_sig_val);
        return;
    }

    timer_addr = TIMER_1_ADDR;
    int helper_index = 0;
	
    // Display all the sets of addresses with frequencies being used
    printf("\nNumber of addresses, frequencies being used: %ld\n\n", num_addrs_attack_sig_val);
    for (int i = 0; i < num_addrs_attack_sig_val; ++i) {

        // Run a sequence to get data in output CSRs first
        sdram_get_value_addr_sig(i);

        // Now we have csrs, display them
        if (i < MAX_ADDR_TO_SET_FREQ) {
            printf("Address set: 0x%07lx ROW: %ld BANK: %ld COL: %ld, freq: %ld\n", 
                rh_test_addr_to_set_val_out_csr_read(), 
                extract_row_from_addr(rh_test_addr_to_set_val_out_csr_read(), rh_test_bank_width_csr_read(), rh_test_col_width_csr_read()),
                extract_bank_from_addr(rh_test_addr_to_set_val_out_csr_read(), rh_test_bank_width_csr_read(), rh_test_col_width_csr_read()),
                extract_col_from_addr(rh_test_addr_to_set_val_out_csr_read(), rh_test_col_width_csr_read()),
                rh_test_addr_to_set_freq_out_csr_read()
            );
        }

        if (((i + 1) % 2 == 0) && (i < MAX_ADDR_TO_SET_FREQ)) {
            printf("\nTimer %d: Cycles for above two addresses: %ld\n\n", ((i / 2) + 1), sdram_get_timer_sigs(timer_addr));
            timer_addr += 1;
        }

        helper_index = i + 1;
    }

    // Display data for the rest of the timers
    for (int i = helper_index; i < MAX_ADDR_TO_SET_FREQ; ++i) {
        
        if ((i + 1) % 2 == 0) {
            printf("\nTimer %d: Cycles for above two addresses: %ld\n\n", ((i / 2) + 1), sdram_get_timer_sigs(timer_addr));
            timer_addr += 1;
        }

    }

    // Display all the sets of addresses/freqs with a frequency of one being used
    for (int i = MAX_ADDR_TO_SET_FREQ; i < num_addrs_attack_sig_val; ++i) {

        // Run a sequence to get data in output CSRs first
        sdram_get_value_addr_sig(i);

        // Now we have csrs, display them
        if (i < NUM_SETS_ATTACK_ADDR) {
            printf("Address set: 0x%07lx ROW: %ld BANK: %ld COL: %ld, freq of one\n", 
                rh_test_addr_to_set_val_out_csr_read(),
                extract_row_from_addr(rh_test_addr_to_set_val_out_csr_read(), rh_test_bank_width_csr_read(), rh_test_col_width_csr_read()),
                extract_bank_from_addr(rh_test_addr_to_set_val_out_csr_read(), rh_test_bank_width_csr_read(), rh_test_col_width_csr_read()),
                extract_col_from_addr(rh_test_addr_to_set_val_out_csr_read(), rh_test_col_width_csr_read())
            );
        }
    }


    // Anything else that contributes to cycles of sets of addresses/freqs being accessed

    printf("\nNumber of cycles to repeat sequence of addr and freq accesses: %ld\n\n", sdram_get_timer_sigs(MAX_TIMER_ADDRESS));
}

void sdram_set_data_pattern(uint32_t input_pattern, uint32_t data_sel) {

    // Assert only timers 2-7 are to be set.
    if (data_sel >= MIN_TIMER_ADDRESS) {
        printf("\nData sel value should be less than %d\n\n", MIN_TIMER_ADDRESS);
        return;
    }

    // Choose which data val to set
    rh_test_input_data_sel_val_csr_write(data_sel);

    // Set the input data
    rh_test_input_data_pattern_csr_write(input_pattern);

    // We want to set the data, not get it
    rh_test_input_data_set_not_get_csr_write(TRUE_CONST);

    // Start the fsm to get data
    rh_test_input_data_to_set_start_fsm_csr_write(TRUE_CONST);

    // Wait for FSM to start
    while (rh_test_input_data_set_start_prev_csr_read() == FALSE_CONST) {};

    // Finish it
    rh_test_input_data_to_set_start_fsm_csr_write(FALSE_CONST);

    // Wait for FSM to finish
    while (rh_test_input_data_set_start_prev_csr_read() == TRUE_CONST) {};
}

// Show data pattern 
void show_data_pattern(void) {

    // Get first data val
    rh_test_input_data_sel_val_csr_write(0);

    // We want to get the data to show, not set it
    rh_test_input_data_set_not_get_csr_write(FALSE_CONST);

    // Start the fsm to get data
    rh_test_input_data_to_set_start_fsm_csr_write(TRUE_CONST);

    // Wait for FSM to start
    while (rh_test_input_data_set_start_prev_csr_read() == FALSE_CONST) {};

    // Finish it
    rh_test_input_data_to_set_start_fsm_csr_write(FALSE_CONST);

    // Wait for FSM to finish
    while (rh_test_input_data_set_start_prev_csr_read() == TRUE_CONST) {};

    // Output data
    printf("Pattern set to:\n\n");
    printf(" %08lx x%ld for all rows, or even rows with double setting\n", rh_test_input_data_pattern_output_csr_read(), (rh_test_data_width_csr_read()/DATA_WIDTH_32_BIT));

    // Get second data val
    rh_test_input_data_sel_val_csr_write(1);

    // We want to get the data to show, not set it
    rh_test_input_data_set_not_get_csr_write(FALSE_CONST);

    // Start the fsm to get data
    rh_test_input_data_to_set_start_fsm_csr_write(TRUE_CONST);

    // Wait for FSM to start
    while (rh_test_input_data_set_start_prev_csr_read() == FALSE_CONST) {};

    // Finish it
    rh_test_input_data_to_set_start_fsm_csr_write(FALSE_CONST);

    // Wait for FSM to finish
    while (rh_test_input_data_set_start_prev_csr_read() == TRUE_CONST) {};

    printf(" %08lx x%ld for odd rows with double setting\n\n", rh_test_input_data_pattern_output_csr_read(), (rh_test_data_width_csr_read()/DATA_WIDTH_32_BIT));

    if (rh_test_input_data_double_pattern_setting_csr_read()) {
		printf("Two-pattern enabled, val: %ld\n\n", rh_test_input_data_double_pattern_setting_csr_read());
	} else {
		printf("One-pattern enabled, val: %ld\n\n", rh_test_input_data_double_pattern_setting_csr_read());
	}

    return;
}

// Show auto precharge setting
void show_auto_precharge(void) {

    printf("Auto precharge set to %ld: ", rh_test_auto_precharge_csr_read());
    if (rh_test_auto_precharge_csr_read()) {
        printf("Enabled for RH test\n\n");
    } else {
        printf("Normal operation\n\n");
    }

}

// // Show read or write setting
// void write_not_read_decide(void) {

// }

// Summarize what will happen in row hammer test 
void sdram_rhtest_summarize_test_params(void) {
    
    printf(OUTPUT_STR_SUMMARY);

    printf(OUTPUT_SEPARATER_TITLE_STR, "Refresh Rate");
    sdram_rhtest_ref_rate_display();

    printf(OUTPUT_SEPARATER_TITLE_STR, "Address List");
    sdram_show_addr_freq();

    printf(OUTPUT_SEPARATER_TITLE_STR, "Data Pattern");
    show_data_pattern();

    printf(OUTPUT_SEPARATER_TITLE_STR, "Auto Precharge");
    show_auto_precharge();
}

// Print out info about addresses
void rh_test_addr_info(void) {

    printf("\nAddress organization");

    printf(OUTPUT_ROW_BANK_COL_INFO, 
        rh_test_address_width_csr_read() - (rh_test_bank_width_csr_read() + rh_test_col_width_csr_read()),
        rh_test_bank_width_csr_read(),
        rh_test_col_width_csr_read()
    );

    printf(OUTPUT_ADDR_DATA_WIDTH_INFO,
        rh_test_address_width_csr_read(),
        rh_test_data_width_csr_read()
    );

}

// All data output csr registers
typedef uint32_t(*IntFuncVoid)(void);
IntFuncVoid data_output[] = {
    rh_test_output_data_pattern1_csr_read,
    rh_test_output_data_pattern2_csr_read,
    rh_test_output_data_pattern3_csr_read,
    rh_test_output_data_pattern4_csr_read,
    rh_test_output_data_pattern5_csr_read,
    rh_test_output_data_pattern6_csr_read,
    rh_test_output_data_pattern7_csr_read,
    rh_test_output_data_pattern8_csr_read,
    rh_test_output_data_pattern9_csr_read,
    rh_test_output_data_pattern10_csr_read,
    rh_test_output_data_pattern11_csr_read,
    rh_test_output_data_pattern12_csr_read,
    rh_test_output_data_pattern13_csr_read,
    rh_test_output_data_pattern14_csr_read,
    rh_test_output_data_pattern15_csr_read,
    rh_test_output_data_pattern16_csr_read,
    rh_test_output_data_pattern17_csr_read,
    rh_test_output_data_pattern18_csr_read
};



// Run the rowhammer test!
void run_rowhammer_test(void) {

    // Beginning output
    printf(OUTPUT_SEPARATER_TITLE_STR, "Row Hammer Test");
    printf("\n\n");

    printf("%ld\n", rh_test_feedback_state_csr_read());

    // Start the row hammer fsm
    rh_test_rowhammer_start_fsm_csr_write(TRUE_CONST);
    while (rh_test_rowhammer_start_prev_fsm_csr_read() == FALSE_CONST) {}

    // Print errors only on the reads
    int first_error = TRUE_CONST;

    while ((rh_test_feedback_state_csr_read() & RH_FINAL_CHECK) != RH_FINAL_CHECK) {

        // printf("%ld\n", rh_test_feedback_state_csr_read());

        // Print feedback
        if ((rh_test_feedback_state_csr_read() & RH_WRITE_FILL_INIT_STATE) == RH_WRITE_FILL_INIT_STATE) {
            printf("\rFilling memory with data              ");
        } else if ((rh_test_feedback_state_csr_read() & RH_READ_CHECK_STATE) == RH_READ_CHECK_STATE) {
            printf("\rReading/Checking memory for errors    ");
        } else if ((rh_test_feedback_state_csr_read() & RH_READ_SEND_ERRORS_STATE) == RH_READ_SEND_ERRORS_STATE) {

            while ((rh_test_feedback_state_csr_read() & RH_READ_SEND_ERRORS_STATE) == RH_READ_SEND_ERRORS_STATE) {
                if (rh_test_error_found_flag_csr_read()) {

                    // Print out all title and number of errors found
                    if (first_error) {

                        // Check where reader is (before or after rh test)
                        printf("\n");
                        if (rh_test_before_after_rh_csr_read() == 0) {
                            printf(OUTPUT_SEPARATER_TITLE_STR, "Initial Read Errors           ");
                        } else {
                            printf(OUTPUT_SEPARATER_TITLE_STR, "Row Hammer Test Errors        ");
                        }

                        printf("\rNumber of addresses with errors found: %ld            \n", rh_test_rowhammer_err_cnt_csr_read());
                        printf("ADDRESS     DATA\n");
                        first_error = FALSE_CONST;
                    }

                    printf(" 0x%07lx, ROW: %ld, BANK: %ld, COL: %ld:  ", 
                        rh_test_address_csr_read(), 
                        extract_row_from_addr(rh_test_address_csr_read(), rh_test_bank_width_csr_read(), rh_test_col_width_csr_read()),
                        extract_bank_from_addr(rh_test_address_csr_read(), rh_test_bank_width_csr_read(), rh_test_col_width_csr_read()),
                        extract_col_from_addr(rh_test_address_csr_read(), rh_test_col_width_csr_read())
                    );

                    for (int i = 0; i < sizeof(data_output) / sizeof(*data_output); i++) {

                        if (rh_test_data_width_csr_read() >= ((i * BYTE_INTERVALS) + BYTE_INTERVALS)) {
                            printf("%8lx ", data_output[i]());
                        }
                    }
                    rh_test_error_ack_csr_write(TRUE_CONST);
                    while (rh_test_error_ack_prev_csr_read() == FALSE_CONST) {}
                    rh_test_error_ack_csr_write(FALSE_CONST);
                    while (rh_test_error_ack_prev_csr_read() == TRUE_CONST) {}
                    // while loop here for err

                    printf("\n");
                    
                }
            }

            // Print title again when entering this state
            printf("\n\n");
            first_error = TRUE_CONST;

        } else if ((rh_test_feedback_state_csr_read() & RH_INIT_SETTINGS_STATE) == RH_INIT_SETTINGS_STATE) {
            printf("\rReadying for Row Hammer Attack        , val: %ld ", rh_test_feedback_state_csr_read());
        } else if ((rh_test_feedback_state_csr_read() & RH_ROWHAMMER_STATE) == RH_ROWHAMMER_STATE) {
            printf("\rRunning Row Hammer Attack, val: %ld ", rh_test_feedback_state_csr_read());
        } else if ((rh_test_feedback_state_csr_read() & RH_RESET_SETTNGS_STATE) == RH_RESET_SETTNGS_STATE) {
            printf("\rResetting after rowhammer attack, val: %ld ", rh_test_feedback_state_csr_read());
        } else {
            printf("\rValue of test feedback state csr: %ld", rh_test_feedback_state_csr_read());
        }
    }
    
    printf("\n\nRow hammer test executed, finishing\n\n");

    // Stop the row hammer fsm (final step)
    rh_test_rowhammer_start_fsm_csr_write(FALSE_CONST);
    while (rh_test_rowhammer_start_prev_fsm_csr_read() == TRUE_CONST) {}

}

