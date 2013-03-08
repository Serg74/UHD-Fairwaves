//
// Copyright 2012-2013 Fairwaves LLC
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef INCLUDED_LMS6002D_HPP
#define INCLUDED_LMS6002D_HPP

#include <stdio.h>
#include <inttypes.h>

/*!
 * LMS6002D control class
 */
class lms6002d_dev {
public:
    virtual ~lms6002d_dev() {}

    void init();
    void dump();

    /** Write through SPI */
    virtual void write_reg(uint8_t addr, uint8_t val) = 0;
    /** Read through SPI */
    virtual uint8_t read_reg(uint8_t addr) = 0;

    /** Tune TX PLL to a given frequency. */
    double tx_pll_tune(double ref_clock, double out_freq) {
        return txrx_pll_tune(0x10, ref_clock, out_freq);
    }
    /** Tune TX PLL to a given frequency. */
    double rx_pll_tune(double ref_clock, double out_freq) {
        return txrx_pll_tune(0x20, ref_clock, out_freq);
    }

    void tx_enable() {
        // STXEN: Soft transmit enable
        lms_set_bits(0x05, (1 << 3));
        // Tx DSM SPI clock enabled
        lms_set_bits(0x09, (1 << 0));
    }

    void rx_enable() {
        // SRXEN: Soft receive enable
        lms_set_bits(0x05, (1 << 2));
        // Rx DSM SPI clock enabled
        lms_set_bits(0x09, (1 << 2));
    }

    void tx_disable() {
        // STXEN: Soft transmit enable
        lms_clear_bits(0x05, (1 << 3));
        // Tx DSM SPI clock enabled
        lms_clear_bits(0x09, (1 << 0));
    }
    void rx_disable() {
        // SRXEN: Soft receive enable
        lms_clear_bits(0x05, (1 << 2));
        // Rx DSM SPI clock enabled
        lms_clear_bits(0x09, (1 << 2));
    }

    uint8_t get_tx_pa() {
        return lms_read_shift(0x44, (0x07 << 3), 3);
    }
    /** Turn on selected Tx PA.
        'pa' parameter is in [0..2] range, where 0 is to turn off all PAs. */
    void set_tx_pa(uint8_t pa) {
        lms_write_bits(0x44, (0x07 << 3), (pa << 3));
    }

    uint8_t get_rx_lna() {
        //   Note: We should also check register 0x25 here, but it's not clear
        //   what to return if 0x75 and 0x25 registers select different LNAs.
        //
        // LNASEL_RXFE[1:0]: Selects the active LNA.
        return lms_read_shift(0x75, (0x03 << 4), 4);
    }

    /** Turn on selected Rx LNA.
    'lna' parameter is in [0..3] range, where 0 is to turn off all LNAs.*/
    void set_rx_lna(uint8_t lna) {
        // LNASEL_RXFE[1:0]: Selects the active LNA.
        lms_write_bits(0x75, (0x03 << 4), (lna << 4));
        // SELOUT[1:0]: Select output buffer in RX PLL, not used in TX PLL
        lms_write_bits(0x25, 0x03, lna);
    }
    /**  Set Tx VGA1 gain in dB.
        gain is in [-4 .. -35] dB range
        Returns the old gain value */
    int8_t set_tx_vga1gain(int8_t gain) {
        /* Safety check */
        if (not(-35 <= gain and gain <= -4))
            gain = -35;
        int8_t old_bits = lms_write_bits(0x41, 0x1f, 35 + gain);
        return (old_bits & 0x1f) - 35;
    }

    /**  Get Tx VGA1 gain in dB.
    gain is in [-4 .. -35] dB range */
    int8_t get_tx_vga1gain() {
        return lms_read_shift(0x41, 0x1f, 0) - 35;
    }

    /**  Set VGA2 gain.
    gain is in dB [0 .. 25]
    Returns the old gain value */
    int8_t set_tx_vga2gain(int8_t gain) {
        /* Safety check */
        if (not(0 <= gain and gain <= 25))
            gain = 0;
        int8_t old_bits = lms_write_bits(0x45, (0x1f << 3), (gain << 3));
        return old_bits >> 3;
    }

    /**  Get TX VGA2 gain in dB.
    gain is in [0 .. 25] dB range
    Returns the gain value */
    int8_t get_tx_vga2gain() {
        int8_t gain = lms_read_shift(0x45, (0x1f << 3), 3);
        gain = (gain <= 25) ? gain : 25;
        return gain;
    }

    /**  Set Rx VGA2 gain.
    gain is in dB [0 .. 60]
    Returns the old gain value */
    int8_t set_rx_vga2gain(int8_t gain) {
        /* Safety check */
        if (not (0 <= gain and gain <= 60))
            gain = 0;
        int8_t old_bits = lms_write_bits(0x65, 0x1f, gain/3);
        return (old_bits & 0x1f) * 3;
    }

    /**  Get Rx VGA2 gain in dB.
    gain is in [0 .. 60] dB range
    Returns the gain value */
    int8_t get_rx_vga2gain() {
        int8_t gain = lms_read_shift(0x65, 0x1f, 0);
        gain = (gain <= 20) ? gain : 20;
        return gain * 3;
    }

    /**  Set Tx VGA1 DC offset, I channel.
    offset is a raw value [0 .. 255]
    Returns the old offset value */
    uint8_t set_tx_vga1dc_i_int(uint8_t offset) {
        uint8_t old_bits = lms_write_bits(0x42, 0xff, offset);
        return old_bits;
    }

    /**  Get Tx VGA1 DC offset, I channel.
    offset is a raw value [0 .. 255]
    Returns the offset value */
    uint8_t get_tx_vga1dc_i_int() {
        uint8_t offset = lms_read_shift(0x42, 0xff, 0);
        return offset;
    }

    /**  Set Tx VGA1 DC offset, Q channel.
    offset is a raw value [0 .. 255]
    Returns the old offset value */
    uint8_t set_tx_vga1dc_q_int(uint8_t offset) {
        uint8_t old_bits = lms_write_bits(0x43, 0xff, offset);
        return old_bits;
    }

    /**  Get Tx VGA1 DC offset, Q channel.
    offset is a raw value [0 .. 255]
    Returns the offset value */
    uint8_t get_tx_vga1dc_q_int() {
        uint8_t offset = lms_read_shift(0x43, 0xff, 0);
        return offset;
    }

    /** Convert a width into a width code.
    width is in kHz [750 .. 14000]
    Returns the corresponding width code */
    int8_t lpf_width_to_code(int width) {
        switch (width) {
        case   750: return 15;
        case   875: return 14;
        case  1250: return 13;
        case  1375: return 12;
        case  1500: return 11;
        case  1920: return 10;
        case  2500: return 9;
        case  2750: return 8;
        case  3000: return 7;
        case  3500: return 6;
        case  4375: return 5;
        case  5000: return 4;
        case  6000: return 3;
        case  7000: return 2;
        case 10000: return 1;
        case 14000: return 0;
        default:
            printf("ERROR: Unsupported width. Setting to 14MHz\n");
            return 0;
        }
    }

    /** Convert a width code into a width.
    width is an integer code as BWC_LPF register.
    Returns the corresponding width in kHz */
    int lpf_code_to_width(int8_t code) {
        switch (code) {
        case 15: return   750;
        case 14: return   875;
        case 13: return  1250;
        case 12: return  1375;
        case 11: return  1500;
        case 10: return  1920;
        case  9: return  2500;
        case  8: return  2750;
        case  7: return  3000;
        case  6: return  3500;
        case  5: return  4375;
        case  4: return  5000;
        case  3: return  6000;
        case  2: return  7000;
        case  1: return 10000;
        case  0: return 14000;
        default:
            printf("ERROR: Unknown width code. Setting to 14MHz\n");
            return 14000;
        }
    }

    /**  Set Tx LPF width.
    width is in kHz [750 .. 14000]
    Returns the old width value in kHz */
    int8_t set_tx_lpf(int width) {
        int8_t width_code = lpf_width_to_code(width);
        int8_t old_bits = lms_write_bits(0x34, (0x0f<<2), (width_code<<2));
        return lpf_code_to_width((old_bits>>2) & 0x0f);
    }

    /**  Get Tx LPF width.
    Returns the width value in kHz */
    int8_t get_tx_lpf() {
        int8_t code = lms_read_shift(0x34, (0x0f<<2), 2);
        return lpf_code_to_width(code);
    }

    /**  Set Rx LPF width.
    width is in kHz [750 .. 14000]
    Returns the old width value in kHz */
    int8_t set_rx_lpf(int width) {
        int8_t width_code = lpf_width_to_code(width);
        int8_t old_bits = lms_write_bits(0x54, (0x0f<<2), (width_code<<2));
        return lpf_code_to_width((old_bits>>2) & 0x0f);
    }

    /**  Get Rx LPF width.
    Returns the width value in kHz */
    int8_t get_rx_lpf() {
        int8_t code = lms_read_shift(0x54, (0x0f<<2), 2);
        return lpf_code_to_width(code);
    }

    /** Programming and Calibration Guide: 4.1 General DC Calibration Procedure */
    int general_dc_calibration_loop(uint8_t dc_addr, uint8_t calibration_reg_base);

    /** This procedure is outlined in FAQ, section 4.7.
    It's purpose is to circumvent the fact that in some edge cases calibration
    may be successful even is DC_LOCK shows 0 or 7.
    */
    int general_dc_calibration(uint8_t dc_addr, uint8_t calibration_reg_base);

    /** Programming and Calibration Guide: 4.2 DC Offset Calibration of LPF Tuning Module */
    bool lpf_tuning_dc_calibration();

    /** Programming and Calibration Guide: 4.3 TX/RX LPF DC Offset Calibration */
    bool txrx_lpf_dc_calibration(bool is_tx);

    /** Programming and Calibration Guide: 4.4 RXVGA2 DC Offset Calibration */
    int rxvga2_dc_calibration();

    /** Programming and Calibration Guide: 4.5 LPF Bandwidth Tuning.
    Note, that this function modifies Tx PLL settings.
    */
    void lpf_bandwidth_tuning(int ref_clock, uint8_t lpf_bandwidth_code);

    /** Performs all automatic calibration procedures in a recommeded order.

        Notes:
          0. Do not forget, that you should not apply any data for Tx during
             the calibration. Rx should be disconnected as well, but we try
             to handle this in the code.
          1. It tunes Tx to 320MHz, so you have to re-tune to your frequency
             after the calibration.
          2. It's better to calibrate with your target TxVGA1 gain. If you
             don't know your target gain yet, choose one <= -7dB to TX mixer
             overload. TxVGA1 gain of -10dB is good choice.
          3. TxVGA2 gain doesn't impact DC offset or LO leakage, because
             it is in RF and is AC coupled. So we don't touch it. Thus TxVGA2
             gain is irrelevant for the purpose of this calibration.
          4. RxVGA2 gain is irrelevant, because it's set to 30dB during the
             calibration and then restored to the original value.
    */
    void auto_calibration(int ref_clock, int lpf_bandwidth_code);


protected:
    double txrx_pll_tune(uint8_t reg, double ref_clock, double out_freq);

    void lms_set_bits(uint8_t address, uint8_t mask) {
        write_reg(address, read_reg(address) | (mask));
    }
    void lms_clear_bits(uint8_t address, uint8_t mask) {
        write_reg(address, read_reg(address) & (~mask));
    }

    uint8_t lms_write_bits(uint8_t address, uint8_t mask, uint8_t bits) {
        uint8_t reg = read_reg(address);
        write_reg(address,  (reg & (~mask)) | bits);
        return reg;
    }
    uint8_t lms_read_shift(uint8_t address, uint8_t mask, uint8_t shift) {
        return (read_reg(address) & mask) >> shift;
    }

};

#endif /* INCLUDED_LMS6002D_HPP */
