#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <curl/curl.h>
#include <vector>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <netinet/in.h>
#include <dirent.h>
#include <sys/wait.h>
#include <vector>

#include "app_dbg.h"
#include "cmd_line.h"
#include "utils.h"

size_t tmp;

#define APP_GET_MAC_ADDRESS_PATH					"/sys/class/net/eth0/address"
string get_mac_address() {
    struct stat file_info;
    int file_obj = -1;
    char *buffer;
    string mac_addr;

    file_obj = open(APP_GET_MAC_ADDRESS_PATH, O_RDONLY);
    if (file_obj < 0) {
        return string("000000000000");
    }

    fstat(file_obj, &file_info);
    buffer = (char*)malloc(file_info.st_size);
    if (!file_info.st_size || !buffer) {
        close(file_obj);
        return string("000000000000");
    }

    tmp = pread(file_obj, buffer, file_info.st_size, 0);
    buffer[17] = 0; /* ex: 38:d5:47:aa:76:26 */
    close(file_obj);
    
    char *ptr = strtok(buffer, ":");
    while(ptr != NULL) {
        mac_addr.append(ptr);
        ptr = strtok(NULL, ":");
    }
    free(buffer);

    return mac_addr;
}

string get_device_id() {
    return get_mac_address().append("ffde");
}

bool write_raw_file(const string data, const string& filename) {
    FILE* file = fopen(filename.data(), "w");
    if (file == NULL) {
        return false;
    }
    fwrite(data.data(), 1, data.length(), file);
	fsync(fileno(file));
    fclose(file);
    return true;
}

bool write_json_file(const json data , const string& filename) {
    return write_raw_file(data.dump(), filename);
}

bool read_raw_file(string& data, const string& filename) {
    struct stat file_info;
	int file_obj = -1;
	char *buffer;

	file_obj = open(filename.data(), O_RDONLY);
	if (file_obj < 0) {
		return false;
	}
	fstat(file_obj, &file_info);
	buffer = (char*)malloc(file_info.st_size);
	if (buffer == NULL) {
        close(file_obj);
		return false;
	}
	ssize_t n = pread(file_obj, buffer, file_info.st_size, 0);
    (void)n;
	close(file_obj);

    data.assign((const char*)buffer, file_info.st_size);    
	free(buffer);

    return true;
}

bool read_json_file(json& data, const string& filename) {
    string content;
    if (read_raw_file(content, filename)) {        
        try {
            data = json::parse(content);
            return true;
        }
        catch ( exception e) {
            (void)e;
            SYS_DBG("json::parse()\n");
        }
    }
    return false;
}

static size_t write_string_function(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

static size_t write_file_function(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

bool download_json(const string& link, json& response, unsigned long timeout) {
    CURL *curl_handle;
    string http_respone_string;
    long http_response_code = 0;
    bool return_state = false;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl_handle, CURLOPT_URL, link.data());
    if (timeout > 0) {
    	curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, timeout);
	}
    curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_string_function);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &http_respone_string);

    CURLcode curl_code = curl_easy_perform(curl_handle);
    curl_easy_getinfo (curl_handle, CURLINFO_RESPONSE_CODE, &http_response_code);

    if (http_response_code == 200 && curl_code != CURLE_ABORTED_BY_CALLBACK) {
        try {
            response = json::parse(http_respone_string.data());
            return_state = true;
        }
        catch ( exception e) {
            (void)e;
            SYS_DBG("json::parse(): %s\n", http_respone_string.data());
        }
    }
    else {
        SYS_DBG("http_response_code: %ld\n", http_response_code);
        SYS_DBG("link: %s\n", link.data());
        SYS_DBG("error: %s\n", curl_easy_strerror(curl_code));
    }

    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();

    return return_state;
}

bool download_file(const string& link, const string& filename, unsigned long timeout) {
    CURL *curl_handle;
    FILE *file;
    long http_response_code = 0;
    bool return_state = false;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl_handle, CURLOPT_URL, link.data());
	if (timeout > 0) {
    	curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, timeout);
	}
    curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_file_function);

    file = fopen(filename.data(), "wb");
    if(file) {
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, file);

        CURLcode curl_code = curl_easy_perform(curl_handle);
        curl_easy_getinfo (curl_handle, CURLINFO_RESPONSE_CODE, &http_response_code);

        if (http_response_code == 200 && curl_code != CURLE_ABORTED_BY_CALLBACK) {
            return_state = true;
        }
        else {
            SYS_DBG("http_response_code: %ld\n", http_response_code);
            SYS_DBG("link: %s\n", link.data());
            SYS_DBG("error: %s\n", curl_easy_strerror(curl_code));
        }

        fclose(file);
    }

    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();

    return return_state;
}

void erase_all_string(string& main_string, string& erase_string) {
	size_t pos = string::npos;
	while ((pos  = main_string.find(erase_string) )!= string::npos) {
		main_string.erase(pos, erase_string.length());
	}
}

void replace_all_string(string& main_string, string& find_string, string& repl_string) {
	size_t pos = string::npos;
	while ((pos  = main_string.find(find_string) )!= string::npos) {
		main_string.replace(pos, find_string.length(), repl_string);
	}
}

void bytetoHexChar(uint8_t ubyte, uint8_t *uHexChar) {
	uHexChar[1] = ((ubyte & 0x0F) < 10) ? ((ubyte & 0x0F) + '0') : (((ubyte & 0x0F) - 10) + 'a');
	uHexChar[0] = ((ubyte >> 4 & 0x0F) < 10) ? ((ubyte >> 4 & 0x0F) + '0') : (((ubyte >> 4 & 0x0F) - 10) + 'a');
}

void bytestoHexChars(uint8_t *ubyte, int32_t len, uint8_t *uHexChar) {
	for (int8_t i = 0; i < len; i++) {
		bytetoHexChar(ubyte[i], (uint8_t*) &uHexChar[i * 2]);
	}
}

void hexChartoByte(uint8_t *uHexChar, uint8_t *ubyte) {
	*ubyte = 0;
	*ubyte = ((uHexChar[0] <= '9' && uHexChar[0] >= '0') ? ((uHexChar[0] - '0') << 4) : *ubyte);
	*ubyte = ((uHexChar[0] <= 'f' && uHexChar[0] >= 'a') ? ((uHexChar[0] - 'a' + 10) << 4) : *ubyte);

	*ubyte = ((uHexChar[1] <= '9' && uHexChar[1] >= '0') ? *ubyte | (uHexChar[1] - '0') : *ubyte);
	*ubyte = ((uHexChar[1] <= 'f' && uHexChar[1] >= 'a') ? *ubyte | ((uHexChar[1] - 'a') + 10) : *ubyte);

}

void hexCharsToBytes(uint8_t *uHexChar, int32_t len, uint8_t *ubyte) {
	for (int8_t i = 0; i < len; i += 2) {
		hexChartoByte((uint8_t*) &uHexChar[i], (uint8_t *) &ubyte[i / 2]);
	}
}

#define APP_GET_IP_ADDRESS_CMD						"ifconfig eth0 | grep \"inet addr\" | cut -d ':' -f 2 | cut -d ' ' -f 1"
string get_ip_address() {
	FILE *fp = NULL;
	uint8_t ret = 0;
	char buff[20];

	memset(buff, 0, sizeof(char) * 20);

	fp = popen(APP_GET_IP_ADDRESS_CMD, "r");
	if (fp == NULL) {
		SYS_DBG("Failed to run command\n");
	}
	else {
		while (fgets(buff, sizeof(buff)-1, fp) != NULL) {
			ret = 1;
			SYS_DBG("%s", buff);
		}
	}

	/* close */
	pclose(fp);

	if (ret) return string(buff, strlen(buff) - 1);
	else return string("UNKNOWN");
}

int get_list_music(const char* path, json& list) {
	DIR *dir;
	struct dirent *ent;

	int song_index = 0;

	/* clean list */
	list = json::array();

	if ((dir = opendir(path)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			int len = strlen(ent->d_name);
			if (\
					(len > 4) \
					&& \
					(\
						(\
							ent->d_name[len - 1] == '3'\
							&& (ent->d_name[len - 2] == 'p' || ent->d_name[len - 2] == 'P') \
							&& (ent->d_name[len - 3] == 'm' || ent->d_name[len - 3] == 'M') \
							&& ent->d_name[len - 4] == '.' \
							)\
						||
						(
							(ent->d_name[len - 1] == 'v' || ent->d_name[len - 2] == 'V')\
							&& (ent->d_name[len - 2] == 'a' || ent->d_name[len - 2] == 'A') \
							&& (ent->d_name[len - 3] == 'w' || ent->d_name[len - 3] == 'W') \
							&& ent->d_name[len - 4] == '.') \
						)\
					) {

				SYS_DBG("song[%d] : [%d]%s\n",song_index, len, ent->d_name);
				song_index++;

				list.push_back(string(ent->d_name));
			}
		}

		closedir (dir);
	}
    return song_index;
}

#define APP_GET_DISK_USAGE							"df /mnt/sda1 | sed -n '2 p' | awk '{ print $3}'"
#define APP_GET_DISK_USAGE_ENCRYPT					"df /mnt/sda1 | sed -n '3 p' | awk '{ print $2}'"
uint32_t get_usb_usage(const char* path) {
	FILE *fp = NULL;
	uint8_t ret = 0;
	char resp[20];

	struct stat st = {0};
	if (stat(path, &st) == 0) {
		memset(resp, 0, 20);

		if (check_usb_encrypted("/etc/hotplug.d/block/80-decrypt-external-drive")) {
			fp = popen(APP_GET_DISK_USAGE_ENCRYPT, "r");
		} else {
			fp = popen(APP_GET_DISK_USAGE, "r");
		}

		if (fp == NULL) {
			SYS_DBG("Failed to run command\n");
		}
		else {
			while (fgets(resp, sizeof(resp) - 1, fp) != NULL) {
				ret = 1;
				SYS_DBG("%s", resp);
			}
		}

		/* close */
		pclose(fp);
	}

	if (ret == 0 || strlen(resp) == 1) {
		return 0;
	}	

	return stoi(string(resp, strlen(resp) - 1));
}

#define APP_GET_DISK_AVAI							"df /mnt/sda1 | sed -n '2 p' | awk '{ print $4}'"
#define APP_GET_DISK_AVAI_ENCRYPT					"df /mnt/sda1 | sed -n '3 p' | awk '{ print $3}'"
uint32_t get_usb_remain(const char* path) {
	FILE *fp = NULL;
	uint8_t ret = 0;
	char resp[20];

	struct stat st = {0};
	if (stat(path, &st) == 0) {
		memset(resp, 0, 20);

		if (check_usb_encrypted("/etc/hotplug.d/block/80-decrypt-external-drive")) {
			fp = popen(APP_GET_DISK_AVAI_ENCRYPT, "r");
		} else {
			fp = popen(APP_GET_DISK_AVAI, "r");
		}

		if (fp == NULL) {
			SYS_DBG("Failed to run command\n");
		}
		else {
			while (fgets(resp, sizeof(resp) - 1, fp) != NULL) {
				ret = 1;
				SYS_DBG("%s", resp);
			}
		}

		/* close */
		pclose(fp);
	}

	if (ret == 0 || strlen(resp) == 1) {
		return 0;
	}

	return stoi(string(resp, strlen(resp) - 1));
}

bool check_usb_encrypted(const char *script) {
	bool ret = false;
	if (access(script, F_OK) != -1) {
		ret = true;
	} else {
		ret = false;
	}
	return ret;
}

int touch_full_file(const char *file_name) {
	int readed_size, read_size;
	uint8_t temp_data[256];
	struct stat file_info;
	int file_name_f = -1;

	file_name_f = open(file_name, O_RDONLY);
	if (file_name_f < 0) {
		return -1;
	}

	fstat(file_name_f, &file_info);

	for (readed_size = 0; readed_size < static_cast<int>(file_info.st_size); readed_size += 256) {
		if (readed_size <= file_info.st_size) {
			read_size = 256;
		}
		else {
			read_size = file_info.st_size - (readed_size - 256);
		}

		tmp = pread(file_name_f, &temp_data, read_size, readed_size);
	}

	(void)temp_data;

	close(file_name_f);
	return 0;
}

bool check_file_exist(const char* file_name) {
	bool ret = false;
	if (access(file_name, F_OK) != -1) {
		ret = true;
	} else {
		ret = false;
	}
	return ret;
}

int exec_prog(int timeout_sec, const char **argv) {
	pid_t my_pid;

	if (0 == (my_pid = fork())) {
		if (-1 == execve(argv[0], (char **)argv , NULL)) {
			printf("[exec_prog] child process execve failed [%m]\n");
			return -1;
		}
	}

#define WAIT_FOR_COMPLETION
#ifdef WAIT_FOR_COMPLETION
	int status;

	while (0 == waitpid(my_pid , &status , WNOHANG)) {
		if ( --timeout_sec < 0 ) {
			printf("[exec_prog] timeout");
			return -1;
		}
		sleep(1);
	}

	printf("[exec_prog] %s WEXITSTATUS %d WIFEXITED %d [status %d]\n",
			  (const char *)argv[0], (int)WEXITSTATUS(status), (int)WIFEXITED(status), status);

	if (1 != WIFEXITED(status) || 0 != WEXITSTATUS(status)) {
		printf("[exec_prog] %s failed, halt system\n", (const char *)argv[0]);
		return -1;
	}

#endif
	return 0;
}

vector<string> string_split(string& s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
		if (token.length()) {
        	res.push_back (token);
		}
    }

    res.push_back (s.substr (pos_start));
    return res;
}