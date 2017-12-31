#ifndef __PIN_CONFIGURATION__
#define __PIN_CONFIGURATION__

#define __IN__
#define __OUT__

#define PIN_IS_CONNECTED __IN__(16)

#define PIN_MUX_SELECTOR_LINE_0 __OUT__(2)
#define PIN_MUX_SELECTOR_LINE_1 __OUT__(0)
#define PIN_MUX_SELECTOR_LINE_2 __OUT__(4)
#define PIN_MUX_SELECTOR_LINE_3 __OUT__(5)

#define PIN_RTS __OUT__(14)

#define PIN_MODE_SELECTOR __IN__(12)

#define PIN_PERIPHERAL_RX __IN__(13)
#define PIN_PERIPHERAL_TX __OUT__(15)

/* Platform decider pin tells whether current platform is SPP or MPP. 
* SPP if INPUT =0
* MPP if INPUT =1
*/
#define PIN_PLATFORM_DECIDER __IN__(10)

#define PIN_PERIPHERAL_ACTIVATE_VCC __OUT__(10)

#endif
