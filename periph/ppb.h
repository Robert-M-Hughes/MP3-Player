/*
 * ppb.h
 *
 *  Created on: Sep 12, 2018
 *      Author: Mason Lee and Robbie Hughes
 */

#ifndef PPB_H_
#define PPB_H_

#define PPB                     ((volatile uint32_t *) 0xe000e000)

enum {
  PPB_EN0 = (0x100 >> 2),
  PPB_STCTRL = (0x10 >> 2),
#define PPB_EN0_TIMER2A (1 << 23)
#define PPB_EN1_TIMER3A (1 << 3)


#define   PPB_STCTRL_COUNT   (1<<16)
#define   PPB_STCTRL_CLK_SRC (1<<2)
#define   PPB_STCTRL_INTEN   (1<<1)
#define   PPB_STCTRL_ENABLE  (1<<0)
  PPB_STRELOAD,
  PPB_STCURRENT,

 //Added for the Timer 5, worksheet + lab9
  PPB_EN2 = (0x108 >> 2),
  //PPB_EN1 = (1x104 >> 4),
#define  PPB_EN2_TIMER5A (1 << 1)
  PPB_PRI16 = (0x440 >> 2),
#define  PPB_PRI_INTB_M (0x111 << 13)
#define  PPB_PRI_INTB_S (1 << 13)

};

#endif /* PPB_H_ */
