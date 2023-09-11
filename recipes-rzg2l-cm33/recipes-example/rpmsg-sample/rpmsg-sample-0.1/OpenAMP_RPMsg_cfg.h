/**
 * @file    OpenAMP_RPMsg_cfg.h
 * @brief   OpenAMP configurations
 * @date    2020.10.27
 * @author  Copyright (c) 2020, eForce Co., Ltd. All rights reserved.
 * @license SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************
 * @par     History
 *          - rev 1.0 (2020.10.27) Imada
 *            Initial version for RZ/G2.
 ****************************************************************************
 */

#ifndef OPENAMP_RPMSG_CFG_H_
#define OPENAMP_RPMSG_CFG_H_

// RPMSG config
#define APP_EPT_ADDR (0x0U)

// Memory region reserved between 0x41000000 - 0x41FFFFFF for RPMSG
#define UC3_RPMSG_MEM_BASE (0x41000000U)
#define UC3_RPMSG_MEM_SIZE (0x01000000U)

#define VRING_SIZE (0x100000U)
#define VRING_SHM_SIZE (0x700000U)

#define CFG_RPMSG_SVCNO (0x2U)
// RPMSG channel #0
#define CFG_RPMSG_SVC_NAME0 "rpmsg-service-0"
#define CFG_VRING0_BASE0    (0x43000000U)
#define CFG_VRING1_BASE0    (0x43050000U)
#define CFG_VRING_SIZE0     (VRING_SIZE)
#define CFG_VRING_ALIGN0    (0x100U)
#define CFG_RPMSG_NUM_BUFS0 (512U)
#define CFG_VRING_SHM_BASE0 (0x43200000U)
#define CFG_VRING_SHM_SIZE0 (VRING_SHM_SIZE)
#define CFG_VRING_CTL_NAME0 "43000000.vring-ctl0"
#define CFG_VRING_SHM_NAME0 "43200000.vring-shm0"
#define VRING_NOTIFYID0     (0U)
// RPMSG channel #1
#define CFG_RPMSG_SVC_NAME1 "rpmsg-service-1"
#define CFG_VRING0_BASE1    (0x43100000U)
#define CFG_VRING1_BASE1    (0x43150000U)
#define CFG_VRING_SIZE1     (VRING_SIZE)
#define CFG_VRING_ALIGN1    (0x100U)
#define CFG_RPMSG_NUM_BUFS1 (512U)
#define CFG_VRING_SHM_BASE1 (0x43900000U)
#define CFG_VRING_SHM_SIZE1 (VRING_SHM_SIZE)
#define CFG_VRING_CTL_NAME1 "43100000.vring-ctl1"
#define CFG_VRING_SHM_NAME1 "43900000.vring-shm1"
#define VRING_NOTIFYID1     (1U)

#endif /* OPENAMP_RPMSG_CFG_H_ */
