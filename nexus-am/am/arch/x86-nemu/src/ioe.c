#include <am.h>
#include <x86.h>

#define RTC_PORT 0x48   // Note that this is not standard
#define I8042_DATA_PORT 0x60
#define I8042_STATUS_PORT 0x64

static unsigned long boot_time;

void _ioe_init() {
  boot_time = inl(RTC_PORT);
}

unsigned long _uptime() {
	return inl(RTC_PORT) - boot_time;
}

uint32_t* const fb = (uint32_t *)0x40000;

_Screen _screen = {
  .width  = 400,
  .height = 300,
};

extern void* memcpy(void *, const void *, int);

void _draw_rect(const uint32_t *pixels, int x, int y, int w, int h) {
  int i;
	//for (i = 0; i < _screen.width * _screen.height; i++) {
  //  fb[i] = i;
  //}
	for (i = 0; i < h; i++) {
		memcpy(fb + (y + i) * _screen.width + x, pixels + i * w, w * 4);
	}
}

void _draw_sync() {
}

int _read_key() {
	uint8_t impresskey = inb(I8042_STATUS_PORT);
  if (impresskey) {
	  return inl(I8042_DATA_PORT);
  } else {
	  return _KEY_NONE;
  }
}
