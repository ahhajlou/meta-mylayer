#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <stdint.h>


int main() {
    int fb_fd = open("/dev/fb0", O_RDWR);
    if (fb_fd == -1) {
        perror("Error opening /dev/fb0");
        return 1;
    }

    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;

    if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo)) {
        perror("Error reading fixed info");
        close(fb_fd);
        return 1;
    }

    if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo)) {
        perror("Error reading variable info");
        close(fb_fd);
        return 1;
    }

    long screensize = vinfo.yres_virtual * finfo.line_length;

    uint8_t *fb_ptr = (uint8_t *)mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
    if (fb_ptr == MAP_FAILED) {
        perror("Error mmapping framebuffer");
        close(fb_fd);
        return 1;
    }

    // Fill screen with a color (e.g., solid red for 16-bit RGB565)
    uint16_t red_pixel = (31 << 11); // RGB565: Red full, Green=0, Blue=0

    for (int y = 0; y < vinfo.yres; y++) {
        for (int x = 0; x < vinfo.xres; x++) {
            long location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) +
                            (y + vinfo.yoffset) * finfo.line_length;
            if (vinfo.bits_per_pixel == 16) {
                *((uint16_t *)(fb_ptr + location)) = red_pixel;
            }
            // Add support for other depths if needed (e.g., 24, 32)
        }
    }

    sleep(5); // Show for 5 seconds

    munmap(fb_ptr, screensize);
    close(fb_fd);

    return 0;
}

