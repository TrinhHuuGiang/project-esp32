/* **********************************************************
 * Definitions
 * **********************************************************/
#include "main_init_api.h"
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// === Folder & file definitions ===
#define SYS_FOLD_ROOT       SPI_SD_CARD_ROOT_PATH_STRING      // "/sd_card0"
#define SYS_FOLD_PROJECT    "/prj_dt"
#define SYS_FOLD_SYSTEM     "/sys_fd"
#define SYS_FOLD_RESOURCE   "/rsrc_fd"


#define SYS_FILE_WIFI        "/wf_cf.dat"
#define SYS_FILE_MQTT_CERT   "/mqttcert.pem"
#define SYS_FILE_MQTT_CONFIG  "/mqtt_cf.dat"
#define SYS_FILE_LOGIC_INIT  "/lgb_init.dat"



// === WiFi config default values ===
#define WIFI_CONFIG_DEFAULT_AP_SSID      "MY_ESP_HOME"
#define WIFI_CONFIG_DEFAULT_AP_PASS      "123123123"
#define WIFI_CONFIG_DEFAULT_STA_SSID     "Trinh Toan"//"NOT_CONNECT_AP"
#define WIFI_CONFIG_DEFAULT_STA_PASS     "0972553271"//"00000000"
#define WIFI_CONFIG_DEFAULT_STA_BSSID    "c0:b1:01:dc:cf:32"//"00:11:22:33:44:55"

#define WIFI_CONFIG_PATH  SYS_FOLD_ROOT SYS_FOLD_PROJECT SYS_FOLD_SYSTEM SYS_FILE_WIFI


// === MQTT config default values ===
#define MQTT_CONFIG_PATH           SYS_FOLD_ROOT SYS_FOLD_PROJECT SYS_FOLD_SYSTEM SYS_FILE_MQTT_CONFIG
#define MQTT_DEFAULT_BROKER        "mqtts://mqttbroker:8883"
#define MQTT_DEFAULT_USERNAME      "user"
#define MQTT_DEFAULT_PASSWORD      "pass"

#define MQTT_CERT_PATH  SYS_FOLD_ROOT SYS_FOLD_PROJECT SYS_FOLD_SYSTEM SYS_FILE_MQTT_CERT

/* **********************************************************
 * Variable 
 * **********************************************************/
extern __task_sync_t* g_task_sync_tools; // get sync tools 

// wifi apsta manager
static wifi_manager_apsta_mode_t* s_apsta_driver_manager = NULL;

/* **********************************************************
 * Code 
 * **********************************************************/

/**
 * @brief Kiểm tra và tạo các thư mục và file hệ thống cần thiết
 * 
 * @retval 0 Thành công
 * @retval !=0 Lỗi tạo file hoặc thư mục tương ứng
 */
uint8_t main_fix_necess_system_file_folder()
{
    struct stat st;
    char path[256]; // safe for most cases

    // === 1. Tạo các thư mục chính nếu thiếu ===
    const char* folders[] = 
    {
        SYS_FOLD_PROJECT,
        SYS_FOLD_PROJECT SYS_FOLD_SYSTEM,
        SYS_FOLD_PROJECT SYS_FOLD_RESOURCE
    };
    for (int i = 0; i < sizeof(folders) / sizeof(folders[0]); ++i) 
    {
        snprintf(path, sizeof(path), "%s%s", SYS_FOLD_ROOT, folders[i]);
        if (stat(path, &st) != 0) {
            if (mkdir(path, 0777) != 0) return 1;
        }
    }

    // === 2. Tạo file wf_cf.dat nếu thiếu ===
    snprintf(path, sizeof(path), "%s%s%s%s", SYS_FOLD_ROOT, SYS_FOLD_PROJECT, SYS_FOLD_SYSTEM, SYS_FILE_WIFI);
    if (stat(path, &st) != 0 || !S_ISREG(st.st_mode)) 
    {
        FILE* f = fopen(path, "w");
        if (!f) return 2;
        fprintf(f,
            "{\n"
            "  \"ap\": {\n"
            "    \"ssid\": \"%s\",\n"
            "    \"pass\": \"%s\"\n"
            "  },\n"
            "  \"sta\": {\n"
            "    \"ssid\": \"%s\",\n"
            "    \"pass\": \"%s\",\n"
            "    \"bssid\": \"%s\"\n"
            "  }\n"
            "}\n",
            WIFI_CONFIG_DEFAULT_AP_SSID,
            WIFI_CONFIG_DEFAULT_AP_PASS,
            WIFI_CONFIG_DEFAULT_STA_SSID,
            WIFI_CONFIG_DEFAULT_STA_PASS,
            WIFI_CONFIG_DEFAULT_STA_BSSID
        );
        fclose(f);
    }

    // === 3. Tạo file lgb_init.dat nếu thiếu ===
    snprintf(path, sizeof(path), "%s%s%s%s", SYS_FOLD_ROOT, SYS_FOLD_PROJECT, SYS_FOLD_SYSTEM, SYS_FILE_LOGIC_INIT);
    if (stat(path, &st) != 0 || !S_ISREG(st.st_mode)) 
    {
        FILE* f = fopen(path, "w");
        if (!f) return 3;
        fprintf(f, "{}\n");
        fclose(f);
    }

    // === 4. Tạo file mqttcert.pem nếu thiếu ===
    snprintf(path, sizeof(path), "%s%s%s%s", SYS_FOLD_ROOT, SYS_FOLD_PROJECT, SYS_FOLD_SYSTEM, SYS_FILE_MQTT_CERT);
    if (stat(path, &st) != 0 || !S_ISREG(st.st_mode)) 
    {
        FILE* f = fopen(path, "w");
        if (!f) return 4;
        fprintf(f, "-----BEGIN CERTIFICATE-----\nDEFAULTCERTDATA==\n-----END CERTIFICATE-----\n");
        fclose(f);
    }

    // === 5. Tạo file mqtt_cf.dat nếu thiếu ===
    snprintf(path, sizeof(path), "%s%s%s%s", SYS_FOLD_ROOT, SYS_FOLD_PROJECT, SYS_FOLD_SYSTEM, SYS_FILE_MQTT_CONFIG);
    if (stat(path, &st) != 0 || !S_ISREG(st.st_mode)) 
    {
        FILE* f = fopen(path, "w");
        if (!f) return 5;
        fprintf(f,
            "{\n"
            "  \"broker\": \"%s\",\n"
            "  \"username\": \"%s\",\n"
            "  \"password\": \"%s\"\n"
            "}\n",
            MQTT_DEFAULT_BROKER,
            MQTT_DEFAULT_USERNAME,
            MQTT_DEFAULT_PASSWORD
        );
        fclose(f);
    }

    return 0;


    // root ("/sd_card0")
    //     |
    //     project file folder ("/prj_dt")
    //         |
    //         system folder ("/sys_fd")
    //         |            |
    //         |            wifi_config ("/wf_cf.dat") // type json format
    //         |            |                          // include: ap, sta config inform 
    //         |            Register_logic_block ("/lgb_init.dat") // type json format
    //         |            |
    //         |            Pem cert MQTT ("/mqttcert.pem") // pem certificate 
    //         |            MQTT_config ("/mqtt_cf.dat") // type json format
    //         |                                       //include: broker uri, username, password
    //         |
    //         resource folder ("/rsrc_fd")
    //                     |
    //                     wav_file("/filename.wav")    // wav format
    //                     |
    //                     binfile ("/filename.bin")    // binary format (bitmap, data...)




}


/**
 * @brief Đọc file wf_cf.dat và cập nhật thông tin WiFi (AP và STA)
 * @note  BSSID là bắt buộc, nếu sai định dạng sẽ trả lỗi
 * 
 * @retval 0 Thành công
 * @retval >0 Lỗi tương ứng từng bước xử lý
 */
uint8_t main_update_wifi_apsta_from_file()
{
    // open file with read mode
    FILE *f = fopen(WIFI_CONFIG_PATH, "r");
    if (!f) return 1;

    // get size
    fseek(f, 0, SEEK_END); //  index pointer 0 -> |ABC|  <- pointer here (index 3) 
    long size = ftell(f);  // <- get last pointer index == number byte
    rewind(f); // go to SEEK_SET


    // copy json file -> string in array
    char *json_str = malloc(size + 1);
    if (!json_str) {
        fclose(f);
        return 2;
    }

    fread(json_str, 1, size, f);
    json_str[size] = '\0'; // end string
    fclose(f); // done copy


    cJSON *root = cJSON_Parse(json_str);
    free(json_str);
    if (!root) return 3;

    const cJSON *ap = cJSON_GetObjectItem(root, "ap");
    const cJSON *sta = cJSON_GetObjectItem(root, "sta");

    if (!cJSON_IsObject(ap) || !cJSON_IsObject(sta)) {
        cJSON_Delete(root);
        return 4;
    }

    // === AP Config ===
    const cJSON *ap_ssid = cJSON_GetObjectItem(ap, "ssid");
    const cJSON *ap_pass = cJSON_GetObjectItem(ap, "pass");

    if (!cJSON_IsString(ap_ssid) || !cJSON_IsString(ap_pass)) {
        cJSON_Delete(root);
        return 5;
    }

    if (wifi_setup_set_wifi_ap_config((uint8_t*)ap_ssid->valuestring, (uint8_t*)ap_pass->valuestring)) {
        cJSON_Delete(root);
        return 6;
    }

    // === STA Config ===
    const cJSON *sta_ssid = cJSON_GetObjectItem(sta, "ssid");
    const cJSON *sta_pass = cJSON_GetObjectItem(sta, "pass");
    const cJSON *sta_bssid = cJSON_GetObjectItem(sta, "bssid");

    if (!cJSON_IsString(sta_ssid) || !cJSON_IsString(sta_pass) || !cJSON_IsString(sta_bssid)) {
        cJSON_Delete(root);
        return 7;
    }

    fprintf(stderr,"sta info: ssid[%s], pass[%s], MAC[%s]", sta_ssid->valuestring, 
        sta_pass->valuestring, sta_bssid->valuestring);

    uint8_t bssid[6];
    if (sscanf(sta_bssid->valuestring, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", // parse unsigned char hex value
               &bssid[0], &bssid[1], &bssid[2],
               &bssid[3], &bssid[4], &bssid[5]) != 6) {
        cJSON_Delete(root);
        return 8;
    }

    if (wifi_setup_set_wifi_sta_config((uint8_t*)sta_ssid->valuestring,
                                       (uint8_t*)sta_pass->valuestring,
                                       bssid)) {
        cJSON_Delete(root);
        return 9;
    }

    cJSON_Delete(root);
    return 0;
}



/**
 * @brief Create task wifi apsta manager driver
 */
void wifi_apsta_mode_driver_manager(void* args)
{
    if(wifi_setup_start_apsta_mode_manager_driver(&s_apsta_driver_manager))
        g_task_sync_tools->err_flag = 1; // set err flag if driver init error
                                            // program will restart soon

    // exit
    vTaskDelete(NULL);
}




/**
 * @brief Config mqtt client with user, pass, broker uri, pem certificate
 * 
 * @return !0 is fail
 * 
 */
uint8_t main_update_mqtt_config_from_file_and_init_mqtt_client()
{
    // lấy cấu hình broker uri, user, password
    FILE *f = fopen(MQTT_CONFIG_PATH, "r");
    if (!f) return 1;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *json_str = malloc(size + 1);
    if (!json_str) {
        fclose(f);
        return 2;
    }

    fread(json_str, 1, size, f);
    json_str[size] = '\0';
    fclose(f);

    cJSON *root = cJSON_Parse(json_str);
    free(json_str);
    if (!root) return 3;

    const cJSON *broker = cJSON_GetObjectItem(root, "broker");
    const cJSON *username = cJSON_GetObjectItem(root, "username");
    const cJSON *password = cJSON_GetObjectItem(root, "password");

    if (!cJSON_IsString(broker) || !cJSON_IsString(username) || !cJSON_IsString(password)) {
        cJSON_Delete(root);
        return 4;
    }

    // Đọc chứng chỉ từ file
    f = fopen(MQTT_CERT_PATH, "r");
    if (!f) {
        cJSON_Delete(root);
        return 5;
    }

    fseek(f, 0, SEEK_END);
    long cert_size = ftell(f);
    rewind(f);

    char *cert_data = malloc(cert_size + 1);
    if (!cert_data) {
        fclose(f);
        cJSON_Delete(root);
        return 6;
    }

    fread(cert_data, 1, cert_size, f);
    cert_data[cert_size] = '\0'; // kết thúc chuỗi
    fclose(f);

    // Gọi hàm MQTT khởi tạo
    if(mqtt_client_handle_client_init(
        broker->valuestring,
        cert_data,
        0,
        username->valuestring,
        password->valuestring
    )) return 7;

    free(cert_data);
    cJSON_Delete(root);
    return 0;
}
