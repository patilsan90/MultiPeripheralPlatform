#ifndef __PIN_CONFIGURATION__
#define __PIN_CONFIGURATION__

#define __IN__
#define __OUT__

#define PIN_PERIPHERAL_TX __OUT__(15)
#define PIN_PERIPHERAL_RX __IN__(2)

#define PIN_RTS __OUT__(4)

#define PIN_IS_CONNECTED __IN__(16)

#define PIN_MUX_SELECTOR_LINE_0 __OUT__(17)
#define PIN_MUX_SELECTOR_LINE_1 __OUT__(5)
#define PIN_MUX_SELECTOR_LINE_2 __OUT__(18)
#define PIN_MUX_SELECTOR_LINE_3 __OUT__(19)

#define PIN_PERIPHERAL_ACTIVATE_VCC __OUT__(21)

#endif
