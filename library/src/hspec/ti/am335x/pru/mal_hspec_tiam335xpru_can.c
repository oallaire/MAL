/*
 * mal_hspec_tiam335xpru_can.c
 *
 *  Created on: Feb 6, 2018
 *      Author: olivi
 */

#include "can/mal_can.h"
#include "std/mal_stdint.h"
#include "std/mal_bool.h"

#include "evmAM335x.h"
#include "soc_AM335x.h"
#include "dcan.h"
#include "dcan_frame.h"
#include "hw_types.h"

#define DCAN_IN_CLK                       (24000000u)
#define CAN_NUM_OF_RX_MSG_OBJS            (CAN_NUM_OF_MSG_OBJS - 1)
// Note that message object indexes starts at 1
#define CAN_TX_MSG_OBJ                    CAN_NUM_OF_MSG_OBJS

static void can_transmit_msg(mal_can_msg_s *msg);

static bool interface_active = false;

mal_error_e mal_can_init(mal_can_init_s *init) {
    uint8_t index;
    // Only support can 1 for now
    if (init->interface != MAL_CAN_1) {
        return MAL_ERROR_UNSOPPORTED;
    }

    // Enable the DCAN1 module clock
    DCANModuleClkConfig();

    // Note that this driver assumes pin mux is taken care of by the device tree.
    // Initialize the DCAN message RAM
    DCANMsgRAMInit(2);

    // Reset the DCAN module
    DCANReset(SOC_DCAN_1_REGS);

    // Enter the Initialization mode of CAN controller
    DCANInitModeSet(SOC_DCAN_1_REGS);

    // Enable the write access to the DCAN configuration registers
    DCANConfigRegWriteAccessControl(SOC_DCAN_1_REGS, DCAN_CONF_REG_WR_ACCESS_ENABLE);

    // Configure the bit timing values for CAN communication
    if (CANSetBitTiming(SOC_DCAN_1_REGS, DCAN_IN_CLK, init->bitrate)) {
        return MAL_ERROR_CLOCK_ERROR;
    }

    // Disable the write access to the DCAN configuration registers
    DCANConfigRegWriteAccessControl(SOC_DCAN_1_REGS, DCAN_CONF_REG_WR_ACCESS_DISABLE);

    // Enable automatic bus on
    DCANAutoBusOnControl(SOC_DCAN_1_REGS, DCAN_AUTO_BUS_ON_ENABLE);

    // Enable the interrupt line 0 of DCAN module
    DCANIntLineEnable(SOC_DCAN_1_REGS, DCAN_INT_LINE0);

    // Invalidate all message objects in the message RAM
    for (index = 1; index <= CAN_NUM_OF_MSG_OBJS; index++) {
        CANInValidateMsgObject(SOC_DCAN_1_REGS, index, DCAN_IF2_REG);
    }

    // Enable CAN module
    DCANNormalModeSet(SOC_DCAN_1_REGS);

    return MAL_ERROR_OK;
}

void mal_can_deinit(mal_can_e interface) {
    // Only support can 1 for now
    if (interface != MAL_CAN_1) {
        return;
    }
    DCANReset(SOC_DCAN_1_REGS);
}

mal_error_e mal_can_add_filter(mal_can_e interface, mal_can_filter_s *filter) {
    uint8_t index;
    // Find a free message object
    for (index = 1; index <= CAN_NUM_OF_RX_MSG_OBJS; index++) {
        if (!DCANMsgValidStatusGet(SOC_DCAN_1_REGS, index)) {
            break;
        }
    }
    if (index > CAN_NUM_OF_RX_MSG_OBJS) {
        return MAL_ERROR_FULL;
    }
    // Use acceptance mask
    DCANUseAcceptanceMaskControl(SOC_DCAN_1_REGS, DCAN_MASK_USED, DCAN_IF2_REG);
    // Configure the DCAN mask registers for acceptance filtering
    unsigned int mask_ext_en = DCAN_MSK_EXT_ID_DISABLE;
    unsigned int id_type = DCAN_ID_MSK_11_BIT;
    if (MAL_CAN_ID_EXTENDED == filter->id_type) {
        id_type = DCAN_ID_MSK_29_BIT;
        mask_ext_en = DCAN_MSK_EXT_ID_ENABLE;
    }
    DCANMsgObjectMskConfig(SOC_DCAN_1_REGS,
                           DCAN_IDENTIFIER_MSK(filter->mask, id_type),
                           DCAN_MSK_MSGDIR_DISABLE,
                           mask_ext_en,
                           DCAN_IF2_REG);
    // Set the message valid bit
    DCANMsgObjValidate(SOC_DCAN_1_REGS, DCAN_IF2_REG);

    // Set the message id of the frame to be received
    unsigned int id_len = DCAN_11_BIT_ID;
    if (MAL_CAN_ID_EXTENDED == filter->id_type) {
        id_len = DCAN_29_BIT_ID;
    }
    DCANMsgIdSet(SOC_DCAN_1_REGS,
                 DCAN_IDENTIFIER_MSK(filter->id, id_type),
                 id_len,
                 DCAN_IF2_REG);

    // Set the message object direction as receive
    DCANMsgDirectionSet(SOC_DCAN_1_REGS, DCAN_RX_DIR, DCAN_IF2_REG);

    // Enable the receive interrupt for the message object
    DCANMsgObjIntEnable(SOC_DCAN_1_REGS, DCAN_RECEIVE_INT, DCAN_IF2_REG);

    // Enable the FIFO end of block
    DCANFIFOEndOfBlockControl(SOC_DCAN_1_REGS, DCAN_END_OF_BLOCK_ENABLE, DCAN_IF2_REG);

    // Configure the command register
    DCANCommandRegSet(SOC_DCAN_1_REGS,
                      DCAN_ACCESS_CTL_BITS | DCAN_MSG_WRITE | DCAN_ACCESS_MSK_BITS | DCAN_ACCESS_ARB_BITS,
                      index,
                      DCAN_IF2_REG);

    return MAL_ERROR_OK;
}

mal_error_e mal_can_remove_filter(mal_can_e interface, mal_can_filter_s *filter) {
    uint8_t index;
    // Find index
    for (index = 1; index <= CAN_NUM_OF_RX_MSG_OBJS; index++) {
        if (!DCANMsgValidStatusGet(SOC_DCAN_1_REGS, index)) {
            continue;
        }
        // Read object
        DCANCommandRegSet(SOC_DCAN_1_REGS,
                          DCAN_DAT_A_ACCESS | DCAN_DAT_B_ACCESS | DCAN_TXRQST_ACCESS | DCAN_CLR_INTPND |
                          DCAN_ACCESS_CTL_BITS | DCAN_ACCESS_ARB_BITS | DCAN_ACCESS_MSK_BITS | DCAN_MSG_READ,
                          index,
                          DCAN_IF2_REG);
        // Wait until data is loaded
        while(DCANIFBusyStatusGet(SOC_DCAN_1_REGS, DCAN_IF2_REG));
        DCANNewDataControl(SOC_DCAN_1_REGS, DCAN_NEW_DAT_CLR, DCAN_IF2_REG);
        // Check mask id type
        uint32_t mask_reg = HWREG(SOC_DCAN_1_REGS + DCAN_IFMSK(DCAN_IF2_REG));
        unsigned int id_type = DCAN_ID_MSK_11_BIT;
        if (mask_reg & DCAN_MSK_EXT_ID_ENABLE) {
            id_type = DCAN_ID_MSK_29_BIT;
            // Mask is for an extended id, check filter
            if (MAL_CAN_ID_EXTENDED != filter->id_type) {
                continue;
            }
        }
        // Check mask
        uint32_t mask = mask_reg & DCAN_IFMSK_MSK;
        if (DCAN_IDENTIFIER_MSK(filter->mask, id_type) != mask) {
            continue;
        }
        // Check id
        uint32_t arb_reg = HWREG(SOC_DCAN_1_REGS + DCAN_IFARB(DCAN_IF2_REG));
        uint32_t id = arb_reg & DCAN_IFARB_MSK;
        if (DCAN_IDENTIFIER_MSK(filter->id, id_type) != id) {
            continue;
        }
        // This is the correct message object
        CANInValidateMsgObject(SOC_DCAN_1_REGS, index, DCAN_IF2_REG);
        break;
    }
    return MAL_ERROR_OK;
}

mal_error_e mal_can_transmit(mal_can_e interface, mal_can_msg_s *msg) {
    if (MAL_CAN_1 != interface) {
        return MAL_ERROR_UNSOPPORTED;
    }
    mal_error_e result = MAL_ERROR_OK;
    // Check if queue is empty
    if (!interface_active) {
        interface_active = true;
        can_transmit_msg(msg);
    } else {
        result = MAL_ERROR_HARDWARE_UNAVAILABLE;
    }

    return result;
}

static void can_transmit_msg(mal_can_msg_s *msg) {
    // Set the message valid bit
    DCANMsgObjValidate(SOC_DCAN_1_REGS, DCAN_IF1_REG);

    // Set the message id of the frame to be transmitted
    unsigned int id_type = DCAN_11_BIT_ID;
    if (MAL_CAN_ID_EXTENDED == msg->id_type) {
        id_type = DCAN_29_BIT_ID;
    }
    DCANMsgIdSet(SOC_DCAN_1_REGS, msg->id, id_type, DCAN_IF1_REG);

    // Set the message object direction as transmit
    DCANMsgDirectionSet(SOC_DCAN_1_REGS, DCAN_TX_DIR, DCAN_IF1_REG);

    // Set the data length code
    DCANDataLengthCodeSet(SOC_DCAN_1_REGS, msg->size, DCAN_IF1_REG);

    // Write data to the DCAN data registers
    uint8_t index;
    unsigned int data[MAL_CAN_MAX_DATA_SIZE];
    for (index = 0; index < msg->size; index++) {
        data[index] = msg->data[index];
    }
    DCANDataWrite(SOC_DCAN_1_REGS, data, DCAN_IF1_REG);

    // Enable the transmit interrupt for the message object
    DCANMsgObjIntEnable(SOC_DCAN_1_REGS, DCAN_TRANSMIT_INT, DCAN_IF1_REG);

    // Enable the DCAN FIFO End of block
    DCANFIFOEndOfBlockControl(SOC_DCAN_1_REGS, DCAN_END_OF_BLOCK_ENABLE, DCAN_IF1_REG);

    /* Configure the command register */
    DCANCommandRegSet(SOC_DCAN_1_REGS,
                      DCAN_DAT_A_ACCESS | DCAN_MSG_WRITE | DCAN_TXRQST_ACCESS |
                      DCAN_DAT_B_ACCESS | DCAN_ACCESS_CTL_BITS | DCAN_ACCESS_ARB_BITS,
                      CAN_TX_MSG_OBJ,
                      DCAN_IF1_REG);
}
