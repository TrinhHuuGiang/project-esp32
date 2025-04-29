#ifndef _MAIN_H_
#define _MAIN_H_

/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
// common libs
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

// esp libs
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"        //log ram

// user defined

// peripherals
#include "_peripherals_err.h"  // peripheral debug
#include "__task_sync.h"       // init sync tools
#include "i2c_master_handle.h" // i2c
#include "spi_master_handle.h" // spi
#include "gpio_setup_handle.h" // gpio
#include "ledc_setup_handle.h" // ledc pwm
#include "wifi_setup_handle.h" // wifi

// protocols
#include "mqtt_pubsub_handle.h" // mqtt client
#include "http_server_handle.h" // http server



// io device
#include "i2c_ssd1306.h"
#include "spi_sd_mount_to_fs.h"
#include "gpio_74hc165.h"



// debug
#if CONFIG_DEBUG_ENABLE !=0

#define CONFIG_ESP_ERR_TO_NAME_LOOKUP // esp_err_to_name need define this macro otherwise it will return UNKOWN ERROR

#endif

// define
#define SSD1306_ADDR (0x3C)



#define MQTT_BROKER_CERTIFICATE "-----BEGIN CERTIFICATE-----\n" \
"MIIFPDCCBCSgAwIBAgISBlvO6+BhAYoAodu3Tizj+vEDMA0GCSqGSIb3DQEBCwUA\n" \
"MDMxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MQwwCgYDVQQD\n" \
"EwNSMTEwHhcNMjUwNDIyMjA1OTMwWhcNMjUwNzIxMjA1OTI5WjAfMR0wGwYDVQQD\n" \
"DBQqLnMxLmV1LmhpdmVtcS5jbG91ZDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCC\n" \
"AQoCggEBAKVuz2sMPmxx2w/f81/YAEKTbNZMJPk2+ooLFg5hxXvReF+AwIT4XvZ+\n" \
"MLhSKvFxmghJF+BB9WyhqrcJLGDCP4s6SOLWTYixEoTcaLUviqqn+06kYqDJ6E83\n" \
"NGsc7T42DlPnzqcZZjPRed9rt4CP3RgeZlWyYZgiD8FoJG9gie8ytihF/FkGZT8T\n" \
"N4Vkl2vQa3mfBWeeKrcuhcLPxqIWDz/30iYfLtEe5JYYScoCKTXcP9SUStjpR8pD\n" \
"vfOWdvasOAuBy7yBbx01/4lcQt50hfbhTR/K14/D4rNkuuvU7ktSQnoxVXC8YDwG\n" \
"zkny10DFt65mVYLNZcBQtOLHHOZGV30CAwEAAaOCAlwwggJYMA4GA1UdDwEB/wQE\n" \
"AwIFoDAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwDAYDVR0TAQH/BAIw\n" \
"ADAdBgNVHQ4EFgQUgsEjDU35+EWJKBsFxJ0lM0PXMi4wHwYDVR0jBBgwFoAUxc9G\n" \
"pOr0w8B6bJXELbBeki8m47kwVwYIKwYBBQUHAQEESzBJMCIGCCsGAQUFBzABhhZo\n" \
"dHRwOi8vcjExLm8ubGVuY3Iub3JnMCMGCCsGAQUFBzAChhdodHRwOi8vcjExLmku\n" \
"bGVuY3Iub3JnLzAzBgNVHREELDAqghQqLnMxLmV1LmhpdmVtcS5jbG91ZIISczEu\n" \
"ZXUuaGl2ZW1xLmNsb3VkMBMGA1UdIAQMMAowCAYGZ4EMAQIBMC0GA1UdHwQmMCQw\n" \
"IqAgoB6GHGh0dHA6Ly9yMTEuYy5sZW5jci5vcmcvNC5jcmwwggEFBgorBgEEAdZ5\n" \
"AgQCBIH2BIHzAPEAdgAN4fIwK9MNwUBiEgnqVS78R3R8sdfpMO8OQh60fk6qNAAA\n" \
"AZZfgg0JAAAEAwBHMEUCIQCENUD4FWITFwnyxsOr4D54wR+LUgZyEjwMd+GwHiha\n" \
"agIgOdeXyofPYtzl2DajwNvR+6XbCikAbbQvZTZ4Eahu2coAdwDM+w9qhXEJZf6V\n" \
"m1PO6bJ8IumFXA2XjbapflTA/kwNsAAAAZZfghU/AAAEAwBIMEYCIQDu8/zVPYFl\n" \
"bmd1vt5Fqk0sXJLV+MEFhQH75Kn6jlvtFgIhAOA8DAE1QBWXxmYSyFXw9UvC4EvH\n" \
"4+VR1cA8merS5vl4MA0GCSqGSIb3DQEBCwUAA4IBAQBVET3hPDZX/protLVPy/vX\n" \
"4i41k5J3teGokrEMu/TdMN6i/W7555Vsgl1zXj5a1f+4FsQ2Nfh1sDMuz/Djzgxp\n" \
"M8HMifB5HJTX+slAuElLzlQFCxMVNn3+b4BgpxvwA3srrXGudF3cya0qztg5lNju\n" \
"y6zAjYfxMQA0uHtCSuxKk033uFkeBv1ui3XWC1JcISbsoF47RVBp/a5O3kBr+j18\n" \
"k5qL7dWcKWr2S9JctGCH4ezYNmAG9W6w/KoTHH3HJCWrTzziJutY48Rwt4gJcS1s\n" \
"OSV8OT5pGKVpVnKSSOz4ItIaqis6fdetTiba38lUyzjDNklYL72Ye4Ck+qvjyc33\n" \
"-----END CERTIFICATE-----" \


#endif