/**
 * @file    main.c
 * @brief   Main function and RPMSG example application.
 * @date    2020.10.27
 * @author  Copyright (c) 2020, eForce Co., Ltd. All rights reserved.
 * @license SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************
 * @par     History
 *          - rev 1.0 (2019.10.23) nozaki
 *            Initial version.
 *          - rev 1.1 (2020.01.28) Imada
 *            Modification for OpenAMP 2018.10.
 *          - rev 1.2 (2020.10.27) Imada
 *            Added the license description.
 ****************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "metal/alloc.h"
#include "openamp/open_amp.h"
#include "platform_info.h"
#include "rsc_table.h"

#define SHUTDOWN_MSG    (0xEF56A55A)

/* Payload definition */
struct _payload {
    unsigned long num;
    unsigned long size;
    unsigned char data[];
};

/* Payload information */
struct payload_info {
    int min;
    int max;
    int num;
};

/* Internal functions */
static void rpmsg_service_bind(struct rpmsg_device *rdev, const char *name, uint32_t dest);
static void rpmsg_service_unbind(struct rpmsg_endpoint *ept);
static int rpmsg_service_cb0(struct rpmsg_endpoint *rp_ept, void *data, size_t len, uint32_t src, void *priv);
static int payload_init(struct rpmsg_device *rdev, struct payload_info *pi);

/* Globals */
static struct rpmsg_endpoint rp_ept = { 0 };
static struct _payload *i_payload;
static int rnum = 0;
static int err_cnt = 0;
static char *svc_name = NULL;

/* External functions */
extern void init_system();
extern void cleanup_system();

/* Application entry point */
int app (struct rpmsg_device *rdev, void *priv, unsigned long svcno)
{
    int ret = 0;
    int shutdown_msg = SHUTDOWN_MSG;
    int i;
    int size;
    int expect_rnum = 0;
    struct payload_info pi = { 0 };

    LPRINTF(" 1 - Send data to remote core, retrieve the echo");
    LPRINTF(" and validate its integrity ..\n");

    /* Initialization of the payload and its related information */
    if ((ret = payload_init(rdev, &pi))) {
        return ret;
    }

    LPRINTF("Remote proc init.\n");

    /* Create RPMsg endpoint */
    if (svcno == 0) {
        svc_name = (char *)CFG_RPMSG_SVC_NAME0;
    } else {
        svc_name = (char *)CFG_RPMSG_SVC_NAME1;
    }
    ret = rpmsg_create_ept(&rp_ept, rdev, svc_name, APP_EPT_ADDR,
                   RPMSG_ADDR_ANY,
                   rpmsg_service_cb0, rpmsg_service_unbind);
    if (ret) {
        LPERROR("Failed to create RPMsg endpoint.\n");
        return ret;
    }
    LPRINTF("RPMSG endpoint has created.\n");

    while (!is_rpmsg_ept_ready(&rp_ept))
        platform_poll(priv);

    LPRINTF("RPMSG service has created.\n");
    for (i = 0, size = pi.min; i < (int)pi.num; i++, size++) {
        i_payload->num = i;
        i_payload->size = size;

        /* Mark the data buffer. */
        memset(&(i_payload->data[0]), 0xA5, size);

        LPRINTF("sending payload number %lu of size %lu\n",
             i_payload->num, (2 * sizeof(unsigned long)) + size);

        ret = rpmsg_send(&rp_ept, i_payload,
             (2 * sizeof(unsigned long)) + size);

        if (ret < 0) {
            LPRINTF("Error sending data...%d\n", ret);
        break;
        }
        LPRINTF("echo test: sent : %lu\n", (2 * sizeof(unsigned long)) + size);

        expect_rnum++;
        do {
            platform_poll(priv);
        } while ((rnum < expect_rnum) && !err_cnt);
        usleep(10000);
    }

    LPRINTF("************************************\n");
    LPRINTF(" Test Results: Error count = %d \n", err_cnt);
    LPRINTF("************************************\n");
    /* Send shutdown message to remote */
    rpmsg_send(&rp_ept, &shutdown_msg, sizeof(int));
    sleep(1);
    LPRINTF("Quitting application .. Echo test end\n");

    metal_free_memory(i_payload);
    return 0;
}

static void rpmsg_service_bind(struct rpmsg_device *rdev, const char *name, uint32_t dest)
{
    LPRINTF("new endpoint notification is received.\n");
    if (strcmp(name, svc_name)) {
        LPERROR("Unexpected name service %s.\n", name);
    }
    else
        (void)rpmsg_create_ept(&rp_ept, rdev, svc_name,
                       APP_EPT_ADDR, dest,
                       rpmsg_service_cb0,
                       rpmsg_service_unbind);
    return ;
}

static void rpmsg_service_unbind(struct rpmsg_endpoint *ept)
{
    (void)ept;
    /* service 0 */
    rpmsg_destroy_ept(&rp_ept);
    memset(&rp_ept, 0x0, sizeof(struct rpmsg_endpoint));
    return ;
}

static int rpmsg_service_cb0(struct rpmsg_endpoint *cb_rp_ept, void *data, size_t len, uint32_t src, void *priv)
{
    (void)cb_rp_ept;
    (void)src;
    (void)priv;
    int i;
    int ret = 0;
    struct _payload *r_payload = (struct _payload *)data;

    LPRINTF(" received payload number %lu of size %lu \r\n",
    r_payload->num, len);

    if (r_payload->size == 0) {
        LPERROR(" Invalid size of package is received.\n");
        err_cnt++;
        return -1;
    }
    /* Validate data buffer integrity. */
    for (i = 0; i < (int)r_payload->size; i++) {
        if (r_payload->data[i] != 0xA5) {
            LPRINTF("Data corruption at index %d\n", i);
            err_cnt++;
            ret = -1;
            break;
        }
    }
    rnum = r_payload->num + 1;
    return ret;
}

static int payload_init(struct rpmsg_device *rdev, struct payload_info *pi) {
    int rpmsg_buf_size = 0;

    /* Get the maximum buffer size of a rpmsg packet */
    if ((rpmsg_buf_size = rpmsg_virtio_get_buffer_size(rdev)) <= 0) {
        return rpmsg_buf_size;
    }

    pi->min = 1;
    pi->max = rpmsg_buf_size - 24;
    pi->num = pi->max / pi->min;

    i_payload =
        (struct _payload *)metal_allocate_memory(2 * sizeof(unsigned long) +
                      pi->max);
    if (!i_payload) {
        LPERROR("memory allocation failed.\n");
        return -ENOMEM;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    void *platform;
    struct rpmsg_device *rpdev;
    unsigned long proc_id = 0;
    unsigned long rsc_id = 0;
    int ret = 0;

    /* Initialize HW system components */
    LPRINTF("Initialize HW system components...\n");
    init_system();
    LPRINTF("Initialize HW system components done\n");

    if (argc >= 2) {
        proc_id = strtoul(argv[1], NULL, 0);
        rsc_id = proc_id;
    }

    /* Initialize platform */
    LPRINTF("Initialize platform...\n");
    ret = platform_init(proc_id, rsc_id, &platform);
    LPRINTF("Initialize platform done, ret=%d\n", ret);

    if (ret) {
        LPERROR("Failed to initialize platform.\n");
        ret = -1;
    } else {
        rpdev = platform_create_rpmsg_vdev(platform, 0,
                          VIRTIO_DEV_MASTER,
                          NULL,
                          rpmsg_service_bind);
        if (!rpdev) {
            LPERROR("Failed to create rpmsg virtio device.\n");
            ret = -1;
        } else {
            (void)app(rpdev, platform, proc_id);
            platform_release_rpmsg_vdev(platform, rpdev);
            ret = 0;
        }
    }

    LPRINTF("Stopping application...\n");
    platform_cleanup(platform);

    cleanup_system();

    return ret;
}
