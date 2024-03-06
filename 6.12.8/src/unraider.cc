#define _GNU_SOURCE
#include <dlfcn.h>
#include <fcntl.h>
#include <openssl/rsa.h>
#include <libudev.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/uio.h>
#include <unistd.h>
#include <iostream>

#include <sys/stat.h>
#include <string>
#include <cstring>
#include <vector>

#define SERIAL_BASE 64
#define UNUSED_WARN (void)
#define DISK_LABEL "UNRAID"
#define BTRS_FORMAT \
    "regGUID=%s&regTy=%s&regTo=\"%s\"&regTm=%s&regGen=0&regDays=0"

using namespace std;

static char *unraid_uuid = NULL;
static char *unraid_name = NULL;
static char *unraid_date = NULL;
static char *unraid_version = NULL;
static char *unraid_reg_info = NULL;
static RSA *rsa = NULL;

/**** udev stuff ****/
int get_dev_path(char *buffer, size_t size)
{
    char link_device[1024];
    char real_device[1024];

    sprintf(link_device, "/dev/disk/by-label/%s", DISK_LABEL);
    char *rv = realpath(link_device, real_device);
    if (!rv)
        return 2;

    struct udev *udev;
    struct udev_device *dev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *dev_list_entry;

    int find = -1;

    udev = udev_new();
    if (!udev)
    {
        return 1;
    }

    enumerate = udev_enumerate_new(udev);
    if (!enumerate)
    {
        return 1;
    }

    udev_enumerate_add_match_subsystem(enumerate, "block");
    udev_enumerate_scan_devices(enumerate);

    devices = udev_enumerate_get_list_entry(enumerate);
    if (!devices)
    {
        return 1;
    }

    udev_list_entry_foreach(dev_list_entry, devices)
    {
        const char *path, *tmp;
        unsigned long long disk_size = 0;

        path = udev_list_entry_get_name(dev_list_entry);
        dev = udev_device_new_from_syspath(udev, path);

        if (strncmp(udev_device_get_devtype(dev), "partition", 9) != 0 &&
            strncmp(udev_device_get_sysname(dev), "loop", 4) != 0)
        {
            const char *devnode = udev_device_get_devnode(dev);
            char *ptr = strstr(real_device, devnode);
            if (ptr && ptr == real_device)
            { // prefix
                find = 0;
                strcpy(buffer, udev_device_get_devpath(dev));
            }
        }

        udev_device_unref(dev);
    }

    udev_enumerate_unref(enumerate);
    udev_unref(udev);

    return find;
}

int get_usb_device(char *buffer, size_t size)
{
    char dev_path[1024];
    if (get_dev_path(dev_path, 1024))
    {
        return 2;
    }

    int slash_index[1024];
    int slash_count = 0;
    for (int i = 0; i < strlen(dev_path); ++i)
    {
        if (dev_path[i] == '/')
        {
            slash_index[slash_count++] = i;
        }
    }

    int find = -1;
    for (int i = slash_count - 1; i >= 0; --i)
    {
        char usb_device[1024] = "/sys";
        strcpy(&dev_path[slash_index[i]], "/serial");
        strcat(usb_device, dev_path);
        if (!access(usb_device, F_OK | R_OK))
        {
            strcpy(buffer, usb_device);
            buffer[strlen(buffer) - strlen("/serial")] = '\0';
            find = 0;
            break;
        }
    }

    return find;
}

void read_file(char *buff_ptr, char *base_ptr, char *file_ptr, char *file)
{
    strcpy(file_ptr, file);
    FILE *fp = fopen(base_ptr, "r");
    fscanf(fp, "%s", buff_ptr);
    fclose(fp);
}

int get_serial_string(char *buffer, size_t size)
{
    char usb_device[1024];
    if (get_usb_device(usb_device, 1024))
    {
        return 2;
    }

    char *file_ptr = &usb_device[strlen(usb_device)];
    char *buff_ptr = buffer;

    read_file(buff_ptr, usb_device, file_ptr, "/idVendor");
    strcat(buff_ptr, "-");
    buff_ptr += strlen(buff_ptr);

    read_file(buff_ptr, usb_device, file_ptr, "/idProduct");
    strcat(buff_ptr, "-");
    buff_ptr += strlen(buff_ptr);

    char raw_serial_buffer[1024];
    memset(raw_serial_buffer, '0', SERIAL_BASE);
    read_file(&raw_serial_buffer[SERIAL_BASE], usb_device, file_ptr, "/serial");
    int serial_length = strlen(&raw_serial_buffer[SERIAL_BASE]);

    char *serial_buffer = &raw_serial_buffer[SERIAL_BASE + serial_length - 16];
    strcpy(buff_ptr, serial_buffer);
    *(buff_ptr + 4) = '-';
    strcpy(buff_ptr + 5, &serial_buffer[4]);

    int offset = 'A' - 'a';
    for (int i = 0; i < strlen(buffer); ++i)
    {
        if (buffer[i] >= 'a' && buffer[i] <= 'z')
        {
            buffer[i] += offset;
        }
    }

    return 0;
}

void unraid_init()
{

    if (!unraid_uuid)
    {
        unraid_uuid = (char *)malloc(1024);
        strcpy(unraid_uuid, "1234-1234-1234-1234567890AB");
        if (getenv("UNRAID_UUID"))
        {
            strcpy(unraid_uuid, getenv("UNRAID_UUID"));
        }
        else
        {
            char buffer[1024];
            int err = get_serial_string(buffer, 1024);
            if (!err)
            {
                strcpy(unraid_uuid, buffer);
            }
        }
        (unraid_name = getenv("UNRAID_NAME")) || (unraid_name = "SpringHack");
        (unraid_date = getenv("UNRAID_DATE")) || (unraid_date = "1615449189");
        (unraid_version = getenv("UNRAID_VERSION")) || (unraid_version = "Pro");
        unraid_reg_info = new char[0x400];
        sprintf(unraid_reg_info, BTRS_FORMAT, unraid_uuid, unraid_version, unraid_name,
                unraid_date);
    }
}

void unraid_rsa_init(void)
{
    rsa = RSA_new();
    BIGNUM *a = BN_new();
    BN_dec2bn(&a, "65537");
    RSA_generate_key_ex(rsa, 2048, a, 0);
}

int unraid_rsa_get_modules(unsigned char *ptr_n)
{
    const BIGNUM *n = RSA_get0_n(rsa);
    return BN_bn2bin(n, ptr_n);
}

int unraid_rsa_encrypt(std::string from, char *to)
{
    int flen = from.size();
    const char *from_ptr = from.c_str();
    return RSA_private_encrypt(flen, (const unsigned char *)from_ptr, (unsigned char *)to, rsa, 1);
}

int main(int argc, char **argv)
{
    unsigned int modulus_len;
    unsigned int key;
    FILE *stream;
    int size;
    char *ptr;
    unraid_init();
    unraid_rsa_init();
    char *n = new char[0x800];
    char *unraidRegInfo_enc = new char[0x800];
    modulus_len = unraid_rsa_get_modules((unsigned char *)n);
    key = unraid_rsa_encrypt(unraid_reg_info, unraidRegInfo_enc);
    std::cout << "[UNRAIDER] modulus_len=" << modulus_len << std::endl;
    std::cout << "[UNRAIDER] key=" << key << std::endl;
    stream = fopen("/usr/local/bin/emhttpd", "rb+");
    if (!stream)
    {
        std::cout << "[UNRAIDER] open file failed" << std::endl;
        exit(2);
    }
    fseek(stream, 0, 2);
    size = ftell(stream);
    rewind(stream);
    ptr = new char[size];
    fread(ptr, size, 1, stream);

    char s[12] = "\0\x93\x13\x15\xcb\xe5\rr";
    // TODO:
    int flag = 1;
    for (int i = 0; i <= size - 8; i++)
    {
        for (int j = 0; j <= 0; j++)
        {
            flag = 1;
            for (int k = 0; k <= 7; ++k)
            {
                flag = (flag && ptr[i + k] == s[k]);
            }
            if (flag)
            {
                std::cout << "[UNRAIDER] pkey=" << std::hex << i << std::endl;
                for (int m = 0; m < (257 - modulus_len); ++m)
                    ptr[i + m] = 0;
                for (int l = 0; l < modulus_len; l++)
                    ptr[257 - modulus_len + i + l] = n[l];
            }
        }
    }
    vector<vector<char>> vector_vectorchar;
    strcpy(s, "EBLACKLISTED");
    for (int ii = 0; ii <= size - 12; ++ii)
    {
        flag = 1;
        // 根据s定位位置
        for (int jj = 0; jj <= 11; ++jj)
        {
            flag = (flag && ptr[ii + jj] == s[jj]);
        }
        if (flag)
        {
            long long int addr = ii + 0x400000;
            vector<char> vector_char;

            int num = 8;
            while (num--)
            {
                char c = (char)addr;
                vector_char.push_back(c);
                addr = addr >> 8;
            }

            std::cout << "[UNRAIDER] str=" << std::hex << ii << std::endl;
            vector_vectorchar.push_back(vector_char);
        }
    }

    for (int kk = 0; kk <= size - 8; ++kk)
    {
        for (int mm = 0;; mm++)
        {
            // 根据vector_vectorchar里面的内容定位数据，并修改为0.
            if (mm >= vector_vectorchar.size())
                break;
            int flag = 1;
            for (int nn = 0; nn <= 7; ++nn)
            {
                // int flag2 = 0;
                // if (flag)
                // {
                //     if (ptr[kk + nn] == *(char *)vector_vectorchar[mm][nn])
                //         flag2 = 1;
                // }
                // flag = flag2;

                flag = (flag && (ptr[kk + nn] == vector_vectorchar[mm][nn]));
            }
            if (flag)
            {
                std::cout << "[UNRAIDER] ptr=" << std::hex << kk << std::endl;
                for (int i1 = 0; i1 <= 7; i1++)
                    ptr[kk + i1] = 0;
            }
        }
    }

    // END TODO:
    rewind(stream);
    fwrite(ptr, size, 1, stream);
    fclose(stream);
    FILE *f = fopen("/boot/config/BTRS.key", "wb");
    fwrite(unraidRegInfo_enc, key, 1, f);
    fclose(f);
    return 0;
}