/**
 * @file ble_hids_service.c
 * @brief hid service implementation
 */

#include "ble_hids.h"
#include "ble_hids_service.h"
#include "ble_hids_descriptor.h"

typedef struct {
    uint8_t report_id;
    uint8_t report_len;
} report_entry_t;

static report_entry_t report_entries[NRF_REPORT_ID_MAX] = {
    {NRF_REPORT_ID_KEYBOARD, NRF_INPUT_REPORT_KEYBOARD_MAX_LEN},
#ifdef MOUSE_ENABLE
    {NRF_REPORT_ID_MOUSE, NRF_INPUT_REPORT_MOUSE_MAX_LEN},
#endif
#ifdef EXTRAKEY_ENABLE
    {NRF_REPORT_ID_SYSTEM, NRF_INPUT_REPORT_SYSTEM_MAX_LEN},
    {NRF_REPORT_ID_CONSUMER, NRF_INPUT_REPORT_CONSUMER_MAX_LEN},
#endif
    };

#define REPORT_ID_TO_INDEX(x) ((x)-1)

/**Buffer queue access macros
 *
 * @{ */
/** Initialization of buffer list */
#define BUFFER_LIST_INIT()     \
    do {                       \
        buffer_list.rp    = 0; \
        buffer_list.wp    = 0; \
        buffer_list.count = 0; \
    } while (0)

/** Provide status of data list is full or not */
#define BUFFER_LIST_FULL() ((MAX_BUFFER_ENTRIES == buffer_list.count - 1) ? true : false)

/** Provides status of buffer list is empty or not */
#define BUFFER_LIST_EMPTY() ((0 == buffer_list.count) ? true : false)

#define BUFFER_ELEMENT_INIT(i)                                  \
    do {                                                        \
        memset(&(buffer_list.buffer[(i)].report_data[0]), 0,    \
                sizeof(buffer_list.buffer[(i)].report_data));   \
    } while (0)

/** @} */

/** Abstracts buffer element */
typedef struct hid_key_buffer {
    uint8_t report_index;                                   /**< report index*/
    uint8_t report_len;                                     /**< report size*/
    uint8_t report_data[NRF_INPUT_REPORT_KEYBOARD_MAX_LEN]; /**< report body*/
    ble_hids_t * p_instance;                                /**< Identifies peer and service instance */
} buffer_entry_t;

STATIC_ASSERT(sizeof(buffer_entry_t) % 4 == 0);

/** Circular buffer list */
typedef struct {
    buffer_entry_t buffer[MAX_BUFFER_ENTRIES]; /**< Maximum number of entries that can enqueued in the list */
    uint8_t        rp;                         /**< Index to the read location */
    uint8_t        wp;                         /**< Index to write location */
    uint8_t        count;                      /**< Number of elements in the list */
} buffer_list_t;

STATIC_ASSERT(sizeof(buffer_list_t) % 4 == 0);

static buffer_list_t     buffer_list;                               /**< List to enqueue not just data to be sent, but also related information like the handle, connection handle etc */
static bool              m_in_boot_mode = false;                    /**< Current protocol mode. */

BLE_HIDS_DEF(m_hids,                                                /**< Structure used to identify the HID service. */
             NRF_SDH_BLE_TOTAL_LINK_COUNT,
             NRF_INPUT_REPORT_KEYBOARD_MAX_LEN,
            #ifdef MOUSE_ENABLE
             NRF_INPUT_REPORT_MOUSE_MAX_LEN,
            #endif
            #ifdef EXTRAKEY_ENABLE
             NRF_INPUT_REPORT_SYSTEM_MAX_LEN,
             NRF_INPUT_REPORT_CONSUMER_MAX_LEN,
            #endif
             OUTPUT_REPORT_MAX_LEN,
             FEATURE_REPORT_MAX_LEN);

static void on_hids_evt(ble_hids_t * p_hids, ble_hids_evt_t * p_evt);
static void on_hid_rep_char_write(ble_hids_evt_t * p_evt);
static void service_error_handler(uint32_t nrf_error);
static uint32_t send_report(ble_hids_t *p_hids, uint8_t report_index, uint8_t *report_data, uint8_t report_lend);
static void     buffer_init(void);
static uint32_t buffer_enqueue(ble_hids_t *p_hids, uint8_t report_index, uint8_t *report_data, uint8_t report_len);
static uint32_t buffer_dequeue(bool tx_flag);

void ble_hids_service_init(void) {
    ret_code_t                    err_code;
    ble_hids_init_t               hids_init_obj;
    ble_hids_inp_rep_init_t     * p_input_report;
    ble_hids_outp_rep_init_t    * p_output_report;
    ble_hids_feature_rep_init_t * p_feature_report;

    static ble_hids_inp_rep_init_t     input_report_array[NRF_REPORT_ID_MAX];
    static ble_hids_outp_rep_init_t    output_report_array[1];
    static ble_hids_feature_rep_init_t feature_report_array[1];

    memset((void *)input_report_array, 0, sizeof(input_report_array));
    memset((void *)output_report_array, 0, sizeof(output_report_array));
    memset((void *)feature_report_array, 0, sizeof(feature_report_array));

    for (int i = 0; i < NRF_REPORT_ID_MAX; i++) {
        // Initialize HID Service
        p_input_report = &input_report_array[i];
        p_input_report->max_len             = report_entries[i].report_len;
        p_input_report->rep_ref.report_id   = report_entries[i].report_id;
        p_input_report->rep_ref.report_type = BLE_HIDS_REP_TYPE_INPUT;

        p_input_report->sec.cccd_wr = SEC_JUST_WORKS;
        p_input_report->sec.wr      = SEC_JUST_WORKS;
        p_input_report->sec.rd      = SEC_JUST_WORKS;
    }

    p_output_report                      = &output_report_array[OUTPUT_REPORT_INDEX];
    p_output_report->max_len             = OUTPUT_REPORT_MAX_LEN;
    p_output_report->rep_ref.report_id   = OUTPUT_REP_REF_ID;
    p_output_report->rep_ref.report_type = BLE_HIDS_REP_TYPE_OUTPUT;

    p_output_report->sec.wr = SEC_JUST_WORKS;
    p_output_report->sec.rd = SEC_JUST_WORKS;

    p_feature_report                      = &feature_report_array[FEATURE_REPORT_INDEX];
    p_feature_report->max_len             = FEATURE_REPORT_MAX_LEN;
    p_feature_report->rep_ref.report_id   = FEATURE_REP_REF_ID;
    p_feature_report->rep_ref.report_type = BLE_HIDS_REP_TYPE_FEATURE;

    p_feature_report->sec.rd              = SEC_JUST_WORKS;
    p_feature_report->sec.wr              = SEC_JUST_WORKS;

    memset(&hids_init_obj, 0, sizeof(hids_init_obj));

    hids_init_obj.evt_handler                    = on_hids_evt;
    hids_init_obj.error_handler                  = service_error_handler;
    hids_init_obj.is_kb                          = true;
    hids_init_obj.is_mouse                       = false;
    hids_init_obj.inp_rep_count                  = sizeof(input_report_array)/sizeof(input_report_array[0]);
    hids_init_obj.p_inp_rep_array                = input_report_array;
    hids_init_obj.outp_rep_count                 = sizeof(output_report_array)/sizeof(output_report_array[0]);
    hids_init_obj.p_outp_rep_array               = output_report_array;
    hids_init_obj.feature_rep_count              = sizeof(feature_report_array)/sizeof(feature_report_array[0]);
    hids_init_obj.p_feature_rep_array            = feature_report_array;
    hids_init_obj.rep_map.data_len               = sizeof(hid_report_descriptor);
    hids_init_obj.rep_map.p_data                 = (uint8_t*)&(hid_report_descriptor[0]);
    hids_init_obj.hid_information.bcd_hid        = BASE_USB_HID_SPEC_VERSION;
    hids_init_obj.hid_information.b_country_code = 0;
    hids_init_obj.hid_information.flags          = HID_INFO_FLAG_REMOTE_WAKE_MSK | HID_INFO_FLAG_NORMALLY_CONNECTABLE_MSK;
    hids_init_obj.included_services_count        = 0;
    hids_init_obj.p_included_services_array      = NULL;

    hids_init_obj.rep_map.rd_sec         = SEC_JUST_WORKS;
    hids_init_obj.hid_information.rd_sec = SEC_JUST_WORKS;

    hids_init_obj.boot_kb_inp_rep_sec.cccd_wr = SEC_JUST_WORKS;
    hids_init_obj.boot_kb_inp_rep_sec.rd      = SEC_JUST_WORKS;

    hids_init_obj.boot_kb_outp_rep_sec.rd = SEC_JUST_WORKS;
    hids_init_obj.boot_kb_outp_rep_sec.wr = SEC_JUST_WORKS;

    hids_init_obj.protocol_mode_rd_sec = SEC_JUST_WORKS;
    hids_init_obj.protocol_mode_wr_sec = SEC_JUST_WORKS;
    hids_init_obj.ctrl_point_wr_sec    = SEC_JUST_WORKS;

    err_code = ble_hids_init(&m_hids, &hids_init_obj);
    APP_ERROR_CHECK(err_code);

    buffer_init();
}

void ble_hids_service_start(void) {}

void ble_hids_service_flush(bool send) {
    // flusing the pending report
    buffer_dequeue(send);
}

/**@brief Function for sending hid report ot peer.
 *
 * @param[in]   report_id       id of the report.
 * @param[in]   report_data     data of the report.
 */
void ble_hids_service_send_report(uint8_t report_id, uint8_t* report_data) {
    if (report_id > NRF_REPORT_ID_MAX) {
        NRF_LOG_WARNING("Invalid report_id: %d", report_id);
        return;
    }
    uint8_t report_index  = REPORT_ID_TO_INDEX(report_id);
    uint8_t report_len    = report_entries[report_index].report_len;

    ret_code_t err_code = send_report(&m_hids, report_index, report_data, report_len);

    if (err_code == NRF_ERROR_RESOURCES) {
        // if report can't sent due to the resource limitation, we buffered
        // the report and will send out later

        UNUSED_VARIABLE(buffer_enqueue(&m_hids, report_index, report_data, report_len));
    }

    if ((err_code != NRF_SUCCESS) &&
        (err_code != NRF_ERROR_INVALID_STATE) &&
        (err_code != NRF_ERROR_RESOURCES) &&
        (err_code != NRF_ERROR_BUSY) &&
        (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
       ) {
        APP_ERROR_HANDLER(err_code);
    }
}

/**@brief   Function for sending hid report
 *
 *
 * @param[in]  p_hids           instance of the hid service.
 * @param[in]  report_index     index of the report.
 * @param[in]  report_data      data buffer of the report.
 * @param[in]  report_len       length of the report.
 *
 * @return     NRF_SUCCESS on success, NRF_ERROR_RESOURCES in case transmission could not be
 *             completed due to lack of transmission buffer or other error codes indicating reason
 *             for failure.
 *
 */
static uint32_t send_report(ble_hids_t * p_hids, uint8_t report_index, uint8_t* report_data, uint8_t report_len) {
    ret_code_t err_code = NRF_SUCCESS;

    NRF_LOG_INFO("BLE report: index=%d, size=%d", report_index, report_len);

    if (!m_in_boot_mode) {
        err_code = ble_hids_inp_rep_send(p_hids, report_index, report_len, report_data, ble_driver.conn_handle);
    } else {
        // in boot mode, only keyboard report was supported
        if (report_index == 0) {
            err_code = ble_hids_boot_kb_inp_rep_send(p_hids, report_len, report_data, ble_driver.conn_handle);
        }
    }

    if (err_code != NRF_SUCCESS) {
        NRF_LOG_WARNING("send report error: %d\n", err_code);
    }
    return err_code;
}


/**@brief   Function for initializing the buffer queue used to key events that could not be
 *          transmitted
 *
 * @warning This handler is an example only. You need to analyze how you wish to buffer or buffer at
 *          all.
 *
 * @note    In case of HID keyboard, a temporary buffering could be employed to handle scenarios
 *          where encryption is not yet enabled or there was a momentary link loss or there were no
 *          Transmit buffers.
 */
static void buffer_init(void) {
    uint32_t buffer_count;

    BUFFER_LIST_INIT();

    for (buffer_count = 0; buffer_count < MAX_BUFFER_ENTRIES; buffer_count++) {
        BUFFER_ELEMENT_INIT(buffer_count);
    }
}


/**@brief Function for enqueuing key scan patterns that could not be transmitted either completely
 *        or partially.
 *
 * @warning This handler is an example only. You need to analyze how you wish to send the key
 *          release.
 *
 * @param[in]  p_hids         Identifies the service for which Key Notifications are buffered.
 * @param[in]  report_index   Index of the report.
 * @param[in]  report_data    Data of the report.
 * @param[in]  report_len     Length of the report.
 *
 * @return     NRF_SUCCESS on success, else an error code indicating reason for failure.
 */
static uint32_t buffer_enqueue(ble_hids_t * p_hids, uint8_t report_index, uint8_t* report_data, uint8_t report_len) {
    buffer_entry_t * element;
    uint32_t         err_code = NRF_SUCCESS;

    if (BUFFER_LIST_FULL()) {
        // Element cannot be buffered.
        err_code = NRF_ERROR_NO_MEM;
    } else {
        // Make entry of buffer element and copy data.
        element                 = &buffer_list.buffer[(buffer_list.wp)];
        element->p_instance     = p_hids;
        element->report_index   = report_index;
        element->report_len     = report_len;
        memcpy(&element->report_data[0], report_data, report_len);

        buffer_list.count++;
        buffer_list.wp++;

        if (buffer_list.wp == MAX_BUFFER_ENTRIES) {
            buffer_list.wp = 0;
        }
    }

    return err_code;
}


/**@brief   Function to dequeue key scan patterns that could not be transmitted either completely of
 *          partially.
 *
 * @warning This handler is an example only. You need to analyze how you wish to send the key
 *          release.
 *
 * @param[in]  tx_flag   Indicative of whether the dequeue should result in transmission or not.
 * @note       A typical example when all keys are dequeued with transmission is when link is
 *             disconnected.
 *
 * @return     NRF_SUCCESS on success, else an error code indicating reason for failure.
 */
static uint32_t buffer_dequeue(bool tx_flag) {
    buffer_entry_t * p_element;
    uint32_t         err_code = NRF_SUCCESS;

    if (BUFFER_LIST_EMPTY()) {
        err_code = NRF_ERROR_NOT_FOUND;
    } else {
        bool remove_element = true;

        p_element = &buffer_list.buffer[(buffer_list.rp)];

        if (tx_flag) {
            err_code = send_report(p_element->p_instance, p_element->report_index, p_element->report_data, p_element->report_len);

            if (err_code == NRF_ERROR_RESOURCES) {
                // Transmission could not be completed, do not remove the entry, adjust next data to
                // be transmitted
                remove_element         = false;
            }
        }

        if (remove_element) {
            BUFFER_ELEMENT_INIT(buffer_list.rp);

            buffer_list.rp++;
            buffer_list.count--;

            if (buffer_list.rp == MAX_BUFFER_ENTRIES) {
                buffer_list.rp = 0;
            }
        }
    }

    return err_code;
}

/**@brief Function for handling the HID Report Characteristic Write event.
 *
 * @param[in]   p_evt   HID service event.
 */
static void on_hid_rep_char_write(ble_hids_evt_t * p_evt) {
    if (p_evt->params.char_write.char_id.rep_type == BLE_HIDS_REP_TYPE_OUTPUT) {
        ret_code_t err_code;
        uint8_t  report_val = 0;
        uint8_t  report_index = p_evt->params.char_write.char_id.rep_index;

        if (report_index == OUTPUT_REPORT_INDEX) {
            // This code assumes that the output report is one byte long. Hence the following
            // static assert is made.
            STATIC_ASSERT(OUTPUT_REPORT_MAX_LEN == 1);

            err_code = ble_hids_outp_rep_get(&m_hids,
                                             report_index,
                                             OUTPUT_REPORT_MAX_LEN,
                                             0,
                                             ble_driver.conn_handle,
                                             &report_val);
            APP_ERROR_CHECK(err_code);
            ble_driver.ble_led = report_val;
        }
    }
}
/**@brief Function for handling HID events.
 *
 * @details This function will be called for all HID events which are passed to the application.
 *
 * @param[in]   p_hids  HID service structure.
 * @param[in]   p_evt   Event received from the HID service.
 */
static void on_hids_evt(ble_hids_t * p_hids, ble_hids_evt_t * p_evt) {
    switch (p_evt->evt_type) {
    case BLE_HIDS_EVT_BOOT_MODE_ENTERED:
        m_in_boot_mode = true;
        break;

    case BLE_HIDS_EVT_REPORT_MODE_ENTERED:
        m_in_boot_mode = false;
        break;

    case BLE_HIDS_EVT_REP_CHAR_WRITE:
        on_hid_rep_char_write(p_evt);
        break;

    case BLE_HIDS_EVT_HOST_SUSP:                         /**< Suspend command received. */
    case BLE_HIDS_EVT_HOST_EXIT_SUSP:                    /**< Exit suspend command received. */
    case BLE_HIDS_EVT_NOTIF_ENABLED:                     /**< Notification enabled event. */
    case BLE_HIDS_EVT_NOTIF_DISABLED:                    /**< Notification disabled event. */
        break;

    default:
        // No implementation needed.
        break;
    }
}

/**@brief Function for handling Service errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void service_error_handler(uint32_t nrf_error) {
    APP_ERROR_HANDLER(nrf_error);
}
