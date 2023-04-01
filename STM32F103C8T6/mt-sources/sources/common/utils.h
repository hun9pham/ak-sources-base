#ifndef __UTILS__
#define __UTILS__

#include "app.h"
#include "json.hpp"

using json = nlohmann::json;

string get_mac_address();
string get_device_id();

bool write_raw_file(const string data, const string& filename);
bool write_json_file(const json data , const string& filename);

bool read_raw_file(string& data, const string& filename);
bool read_json_file(json& data, const string& filename);

bool download_json(const string& link, json& response, unsigned long timeout);
bool download_file(const string& link, const string& filename, unsigned long timeout);

void erase_all_string(string& main_string, string& erase_string);
void replace_all_string(string& main_string, string& find_string, string& repl_string);

void bytetoHexChar(uint8_t ubyte, uint8_t *uHexChar);
void bytestoHexChars(uint8_t *ubyte, int32_t len, uint8_t *uHexChar);
void hexChartoByte(uint8_t *uHexChar, uint8_t *ubyte);
void hexCharsToBytes(uint8_t *uHexChar, int32_t len, uint8_t *ubyte);

string get_ip_address();
int get_list_music(const char* path, json& list);
uint32_t get_usb_usage(const char* path);
uint32_t get_usb_remain(const char* path);
bool check_usb_encrypted(const char *script);
int touch_full_file(const char* file_name);
bool check_file_exist(const char* file_name);

extern int exec_prog(int, const char**);

vector<string> string_split(string& s, string delimiter);

#endif //__UTILS__
