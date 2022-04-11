/*
 * File	: uart_register.h
 * Copyright (C) 2013 - 2016, Espressif Systems
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of version 3 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/*
 *  Copyright (c) 2010 - 2011 Espressif System
 *
 */

#ifndef UART_REGISTER_H_
#define UART_REGISTER_H_

#define REG_UART_BASE(i)                    (0x60000000 + (i)*0xf00)
//version value:32'h062000

#define UART_FIFO(i)                        (REG_UART_BASE(i) + 0x0)
#define UART_RXFIFO_RD_BYTE                 (0x000000FF)
#define UART_RXFIFO_RD_BYTE_S               (0)

#define UART_INT_RAW(i)                     (REG_UART_BASE(i) + 0x4)
#define UART_RXFIFO_TOUT_INT_RAW            (0x00000100)    // (BIT(8))
#define UART_BRK_DET_INT_RAW                (0x00000080)    // (BIT(7))
#define UART_CTS_CHG_INT_RAW                (0x00000040)    // (BIT(6))
#define UART_DSR_CHG_INT_RAW                (0x00000020)    // (BIT(5))
#define UART_RXFIFO_OVF_INT_RAW             (0x00000010)    // (BIT(4))
#define UART_FRM_ERR_INT_RAW                (0x00000008)    // (BIT(3))
#define UART_PARITY_ERR_INT_RAW             (0x00000004)    // (BIT(2))
#define UART_TXFIFO_EMPTY_INT_RAW           (0x00000002)    // (BIT(1))
#define UART_RXFIFO_FULL_INT_RAW            (0x00000001)    // (BIT(0))

#define UART_INT_ST(i)                      (REG_UART_BASE(i) + 0x8)
#define UART_RXFIFO_TOUT_INT_ST             (0x00000100)    // (BIT(8))
#define UART_BRK_DET_INT_ST                 (0x00000080)    // (BIT(7))
#define UART_CTS_CHG_INT_ST                 (0x00000040)    // (BIT(6))
#define UART_DSR_CHG_INT_ST                 (0x00000020)    // (BIT(5))
#define UART_RXFIFO_OVF_INT_ST              (0x00000010)    // (BIT(4))
#define UART_FRM_ERR_INT_ST                 (0x00000008)    // (BIT(3))
#define UART_PARITY_ERR_INT_ST              (0x00000004)    // (BIT(2))
#define UART_TXFIFO_EMPTY_INT_ST            (0x00000002)    // (BIT(1))
#define UART_RXFIFO_FULL_INT_ST             (0x00000001)    // (BIT(0))

#define UART_INT_ENA(i)                     (REG_UART_BASE(i) + 0xC)
#define UART_RXFIFO_TOUT_INT_ENA            (0x00000100)    // (BIT(8))
#define UART_BRK_DET_INT_ENA                (0x00000080)    // (BIT(7))
#define UART_CTS_CHG_INT_ENA                (0x00000040)    // (BIT(6))
#define UART_DSR_CHG_INT_ENA                (0x00000020)    // (BIT(5))
#define UART_RXFIFO_OVF_INT_ENA             (0x00000010)    // (BIT(4))
#define UART_FRM_ERR_INT_ENA                (0x00000008)    // (BIT(3))
#define UART_PARITY_ERR_INT_ENA             (0x00000004)    // (BIT(2))
#define UART_TXFIFO_EMPTY_INT_ENA           (0x00000002)    // (BIT(1))
#define UART_RXFIFO_FULL_INT_ENA            (0x00000001)    // (BIT(0))

#define UART_INT_CLR(i)                     (REG_UART_BASE(i) + 0x10)
#define UART_RXFIFO_TOUT_INT_CLR            (0x00000100)    // (BIT(8))
#define UART_BRK_DET_INT_CLR                (0x00000080)    // (BIT(7))
#define UART_CTS_CHG_INT_CLR                (0x00000040)    // (BIT(6))
#define UART_DSR_CHG_INT_CLR                (0x00000020)    // (BIT(5))
#define UART_RXFIFO_OVF_INT_CLR             (0x00000010)    // (BIT(4))
#define UART_FRM_ERR_INT_CLR                (0x00000008)    // (BIT(3))
#define UART_PARITY_ERR_INT_CLR             (0x00000004)    // (BIT(2))
#define UART_TXFIFO_EMPTY_INT_CLR           (0x00000002)    // (BIT(1))
#define UART_RXFIFO_FULL_INT_CLR            (0x00000001)    // (BIT(0))

#define UART_CLKDIV(i)                      (REG_UART_BASE(i) + 0x14)
#define UART_CLKDIV_CNT                     (0x000FFFFF)
#define UART_CLKDIV_S                       (0)

#define UART_AUTOBAUD(i)                    (REG_UART_BASE(i) + 0x18)
#define UART_GLITCH_FILT                    (0x000000FF)
#define UART_GLITCH_FILT_S                  (8)
#define UART_AUTOBAUD_EN                    (0x00000001)    // (BIT(0))

#define UART_STATUS(i)                      (REG_UART_BASE(i) + 0x1C)
#define UART_TXD                            (0x80000000)    // (BIT(31))
#define UART_RTSN                           (0x40000000)    // (BIT(30))
#define UART_DTRN                           (0x20000000)    // (BIT(29))
#define UART_TXFIFO_CNT                     (0x000000FF)
#define UART_TXFIFO_CNT_S                   (16)
#define UART_RXD                            (0x00008000)    // (BIT(15))
#define UART_CTSN                           (0x00004000)    // (BIT(14))
#define UART_DSRN                           (0x00002000)    // (BIT(13))
#define UART_RXFIFO_CNT                     (0x000000FF)
#define UART_RXFIFO_CNT_S                   (0)

#define UART_CONF0(i)                       (REG_UART_BASE(i) + 0x20)
#define UART_DTR_INV                        (0x01000000)    // (BIT(24))
#define UART_RTS_INV                        (0x00800000)    // (BIT(23))
#define UART_TXD_INV                        (0x00400000)    // (BIT(22))
#define UART_DSR_INV                        (0x00200000)    // (BIT(21))
#define UART_CTS_INV                        (0x00100000)    // (BIT(20))
#define UART_RXD_INV                        (0x00080000)    // (BIT(19))
#define UART_TXFIFO_RST                     (0x00040000)    // (BIT(18))
#define UART_RXFIFO_RST                     (0x00020000)    // (BIT(17))
#define UART_IRDA_EN                        (0x00010000)    // (BIT(16))
#define UART_TX_FLOW_EN                     (0x00008000)    // (BIT(15))
#define UART_LOOPBACK                       (0x00004000)    // (BIT(14))
#define UART_IRDA_RX_INV                    (0x00002000)    // (BIT(13))
#define UART_IRDA_TX_INV                    (0x00001000)    // (BIT(12))
#define UART_IRDA_WCTL                      (0x00000800)    // (BIT(11))
#define UART_IRDA_TX_EN                     (0x00000400)    // (BIT(10))
#define UART_IRDA_DPLX                      (0x00000200)    // (BIT(9))
#define UART_TXD_BRK                        (0x00000100)    // (BIT(8))
#define UART_SW_DTR                         (0x00000080)    // (BIT(7))
#define UART_SW_RTS                         (0x00000040)    // (BIT(6))
#define UART_STOP_BIT_NUM                   (0x00000003)
#define UART_STOP_BIT_NUM_S                 (4)
#define UART_BIT_NUM                        (0x00000003)
#define UART_BIT_NUM_S                      (2)
#define UART_PARITY_EN                      (0x00000002)    // (BIT(1))
#define UART_PARITY_EN_M                    (0x00000001)
#define UART_PARITY_EN_S                    (1)
#define UART_PARITY                         (0x00000001)    // (BIT(0))
#define UART_PARITY_M                       (0x00000001)
#define UART_PARITY_S                       (0)

#define UART_CONF1(i)                       (REG_UART_BASE(i) + 0x24)
#define UART_RX_TOUT_EN                     (0x80000000)    // (BIT(31))
#define UART_RX_TOUT_THRHD                  (0x0000007F)
#define UART_RX_TOUT_THRHD_S                (24)
#define UART_RX_FLOW_EN                     (0x00800000)    // (BIT(23))
#define UART_RX_FLOW_THRHD                  (0x0000007F)
#define UART_RX_FLOW_THRHD_S                (16)
#define UART_TXFIFO_EMPTY_THRHD             (0x0000007F)
#define UART_TXFIFO_EMPTY_THRHD_S           (8)
#define UART_RXFIFO_FULL_THRHD              (0x0000007F)
#define UART_RXFIFO_FULL_THRHD_S            (0)

#define UART_LOWPULSE(i)                    (REG_UART_BASE(i) + 0x28)
#define UART_LOWPULSE_MIN_CNT               (0x000FFFFF)
#define UART_LOWPULSE_MIN_CNT_S             (0)

#define UART_HIGHPULSE(i)                   (REG_UART_BASE(i) + 0x2C)
#define UART_HIGHPULSE_MIN_CNT              (0x000FFFFF)
#define UART_HIGHPULSE_MIN_CNT_S            (0)

#define UART_PULSE_NUM(i)                   (REG_UART_BASE(i) + 0x30)
#define UART_PULSE_NUM_CNT                  (0x0003FF)
#define UART_PULSE_NUM_CNT_S                (0)

#define UART_DATE(i)                        (REG_UART_BASE(i) + 0x78)
#define UART_ID(i)                          (REG_UART_BASE(i) + 0x7C)

#endif // UART_REGISTER_H_INCLUDED

