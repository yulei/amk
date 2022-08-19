/*!
    \file  system_gd32e50x.c
    \brief CMSIS Cortex-M33 Device Peripheral Access Layer Source File for
           GD32E50x Device Series
*/

/* Copyright (c) 2012 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

/* This file refers the CMSIS standard, some adjustments are made according to GigaDevice chips */

#include "gd32e50x.h"

/* system frequency define */
#define __IRC8M           (IRC8M_VALUE)            /* internal 8 MHz RC oscillator frequency */
#define __HXTAL           (HXTAL_VALUE)            /* high speed crystal oscillator frequency */
#define __SYS_OSC_CLK     (__IRC8M)                /* main oscillator frequency */

#ifndef VECT_TAB_OFFSET
#define VECT_TAB_OFFSET  (uint32_t)0x00            /* vector table base offset */
#endif

#define SEL_IRC8M       0x00
#define SEL_HXTAL       0x01
#define SEL_PLL         0x02

uint32_t SystemCoreClock = HXTAL_VALUE;

/*!
    \brief      setup the micro-controller system, initialize the system
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SystemInit (void)
{
  /* FPU settings */
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
#endif
    /* reset the RCU clock configuration to the default reset state */
    /* Set IRC8MEN bit */
    RCU_CTL |= RCU_CTL_IRC8MEN;

    RCU_CFG0 &= ~RCU_CFG0_SCS;
#if (defined(GD32EPRT) || defined(GD32E50X_HD) || defined(GD32E50X_XD))
    /* reset HXTALEN, CKMEN and PLLEN bits */
    RCU_CTL &= ~(RCU_CTL_PLLEN | RCU_CTL_CKMEN | RCU_CTL_HXTALEN);
    /* disable all interrupts */
    RCU_INT = 0x009f0000U;
#elif (defined(GD32E50X_CL) || defined(GD32E508))
    /* Reset HXTALEN, CKMEN, PLLEN, PLL1EN and PLL2EN bits */
    RCU_CTL &= ~(RCU_CTL_PLLEN |RCU_CTL_PLL1EN | RCU_CTL_PLL2EN | RCU_CTL_CKMEN | RCU_CTL_HXTALEN);
    /* disable all interrupts */
    RCU_INT = 0x00ff0000U;
#endif /* GD32F50X_EPRT and GD32F50X_HD and GD32F50X_XD */

    /* Reset CFG0 and CFG1 registers */
    RCU_CFG0 = 0x00000000U;
    RCU_CFG1 = 0x00000000U;
    /* reset HXTALBPS bit */
    RCU_CTL &= ~(RCU_CTL_HXTALBPS);

    /* configure the system clock source, PLL Multiplier, AHB/APBx prescalers and Flash settings */
    //system_clock_config();
    
#ifdef VECT_TAB_SRAM
    nvic_vector_table_set(NVIC_VECTTAB_RAM,VECT_TAB_OFFSET);
#else
    nvic_vector_table_set(NVIC_VECTTAB_FLASH,VECT_TAB_OFFSET);
#endif
}

/*!
    \brief      update the SystemCoreClock with current core clock retrieved from CPU registers
    \param[in]  none
    \param[out] none
    \retval     none
*/
/// @brief 
/// @param  
void SystemCoreClockUpdate (void)
{
    uint32_t sws;
    uint32_t pllsel, pllpresel, predv0sel, pllmf, ck_src, idx, clk_exp;
#if (defined(GD32E50X_CL) || defined(GD32EPRT) || defined(GD32E508))
    uint32_t predv0, predv1, pll1mf;
#endif /* GD32E50X_CL and GD32EPRT*/

    /* exponent of AHB, APB1 and APB2 clock divider */
    uint8_t ahb_exp[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};

    sws = GET_BITS(RCU_CFG0, 2, 3);
    switch(sws){
    /* IRC8M is selected as CK_SYS */
    case SEL_IRC8M:
        SystemCoreClock = IRC8M_VALUE;
        break;
    /* HXTAL is selected as CK_SYS */
    case SEL_HXTAL:
        SystemCoreClock = HXTAL_VALUE;
        break;
    /* PLL is selected as CK_SYS */
    case SEL_PLL:
        /* PLL clock source selection, HXTAL, IRC48M or IRC8M/2 */
        pllsel = (RCU_CFG0 & RCU_CFG0_PLLSEL);

        if(RCU_PLLSRC_HXTAL_IRC48M == pllsel) {
            /* PLL clock source is HXTAL or IRC48M */
            pllpresel = (RCU_CFG1 & RCU_CFG1_PLLPRESEL);
            
            if(RCU_PLLPRESRC_HXTAL == pllpresel){
                /* PLL clock source is HXTAL */
                ck_src = HXTAL_VALUE;
            }else{
                /* PLL clock source is IRC48 */
                ck_src = IRC48M_VALUE;
            }

#if defined(GD32E50X_HD) || defined(GD32E50X_XD)
            predv0sel = (RCU_CFG0 & RCU_CFG0_PREDV0);
            /* PREDV0 input source clock divided by 2 */
            if(RCU_CFG0_PREDV0 == predv0sel){
                ck_src = ck_src/2U;
            }
#elif (defined(GD32E50X_CL) || defined(GD32EPRT) || defined(GD32E508))
            predv0sel = (RCU_CFG1 & RCU_CFG1_PREDV0SEL);
            /* source clock use PLL1 */
            if(RCU_PREDV0SRC_CKPLL1 == predv0sel){
                predv1 = ((RCU_CFG1 & RCU_CFG1_PREDV1) >> ((uint32_t)4U)) + 1U;
                pll1mf = (uint32_t)((RCU_CFG1 & RCU_CFG1_PLL1MF) >> ((uint32_t)8U)) + 2U;
                if(15U == pll1mf){
                    pll1mf = 20U;
                }
                ck_src = (ck_src/predv1)*pll1mf;
            }
            predv0 = (RCU_CFG1 & RCU_CFG1_PREDV0) + 1U;
            ck_src /= predv0;
#endif /* GD32E50X_HD and GD32E50X_XD */
        }else{
            /* PLL clock source is IRC8M/2 */
            ck_src = IRC8M_VALUE/2U;
        }

        /* PLL multiplication factor */
        pllmf = GET_BITS(RCU_CFG0, 18, 21);
        if((RCU_CFG0 & RCU_CFG0_PLLMF_4)){
            pllmf |= 0x10U;
        }
        if((RCU_CFG0 & RCU_CFG0_PLLMF_5)){
            pllmf |= 0x20U;
        }
        if(pllmf < 15U){
            pllmf += 2U;
        }else if((pllmf >= 15U) && (pllmf <= 64U)){
            pllmf += 1U;
        }
        SystemCoreClock = ck_src*pllmf;
#if (defined(GD32E50X_CL) || defined(GD32EPRT) || defined(GD32E508))
        if(15U == pllmf){
            SystemCoreClock = ck_src*6U + ck_src/2U;
        }
#endif /* GD32E50X_CL and GD32EPRT and GD32E508 */

        break;
    /* IRC8M is selected as CK_SYS */
    default:
        SystemCoreClock = IRC8M_VALUE;
        break;
    }

    /* calculate AHB clock frequency */
    idx = GET_BITS(RCU_CFG0, 4, 7);
    clk_exp = ahb_exp[idx];
    SystemCoreClock >>= clk_exp;
}
