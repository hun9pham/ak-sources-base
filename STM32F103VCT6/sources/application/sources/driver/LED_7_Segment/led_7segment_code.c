//=============================================================================
//
//=============================================================================
// Project   :  led_7segment_code.h
// Author    :  HungPNQ
// Date      :  2022-07-22
// Brief     :  Hex code display number of LED 7 Segment
//=============================================================================

#include "led_7segment_code.h"

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Extern variables ----------------------------------------------------------*/
const struct LED_7_SEGMENT CC_Led_7_Segment_tbl[] = {
    {    (const char)'0',   CC_NUMBER_0         },
    {    (const char)'1',   CC_NUMBER_1         },
    {    (const char)'2',   CC_NUMBER_2         },
    {    (const char)'3',   CC_NUMBER_3         },
    {    (const char)'4',   CC_NUMBER_4         },
    {    (const char)'5',   CC_NUMBER_5         },
    {    (const char)'6',   CC_NUMBER_6         },
    {    (const char)'7',   CC_NUMBER_7         },
    {    (const char)'8',   CC_NUMBER_8         },
    {    (const char)'9',   CC_NUMBER_9         },

    {    (const char)'A',    CC_LETTER_A        },
    {    (const char)'b',    CC_LETTER_b        },
    {    (const char)'C',    CC_LETTER_C        },
    {    (const char)'d',    CC_LETTER_d        },
    {    (const char)'E',    CC_LETTER_E        },
    {    (const char)'F',    CC_LETTER_F        },
    {    (const char)'H',    CC_LETTER_H        },
    {    (const char)'J',    CC_LETTER_J        },
    {    (const char)'L',    CC_LETTER_L        },
    {    (const char)'n',    CC_LETTER_n        },
    {    (const char)'U',    CC_LETTER_U        },
    {    (const char)'u',    CC_LETTER_u        },
    {    (const char)'Y',    CC_LETTER_Y        },
    {    (const char)'o',    CC_LETTER_o        },
    {    (const char)'i',    CC_LETTER_i        },
    {    (const char)'q',    CC_LETTER_q        },
};

const struct LED_7_SEGMENT CA_Led_7_Segment_tbl[] = {
    {    (const char)'0',   CA_NUMBER_0         },
    {    (const char)'1',   CA_NUMBER_1         },
    {    (const char)'2',   CA_NUMBER_2         },
    {    (const char)'3',   CA_NUMBER_3         },
    {    (const char)'4',   CA_NUMBER_4         },
    {    (const char)'5',   CA_NUMBER_5         },
    {    (const char)'6',   CA_NUMBER_6         },
    {    (const char)'7',   CA_NUMBER_7         },
    {    (const char)'8',   CA_NUMBER_8         },
    {    (const char)'9',   CA_NUMBER_9         },
    
    {    (const char)'A',    CA_LETTER_A        },
    {    (const char)'b',    CA_LETTER_b        },
    {    (const char)'C',    CA_LETTER_C        },
    {    (const char)'d',    CA_LETTER_d        },
    {    (const char)'E',    CA_LETTER_E        },
    {    (const char)'F',    CA_LETTER_F        },
    {    (const char)'H',    CA_LETTER_H        },
    {    (const char)'J',    CA_LETTER_J        },
    {    (const char)'L',    CA_LETTER_L        },
    {    (const char)'n',    CA_LETTER_n        },
    {    (const char)'U',    CA_LETTER_U        },
    {    (const char)'u',    CA_LETTER_u        },
    {    (const char)'Y',    CA_LETTER_Y        },
    {    (const char)'o',    CA_LETTER_o        },
    {    (const char)'i',    CA_LETTER_i        },
    {    (const char)'q',    CA_LETTER_q        },
};
