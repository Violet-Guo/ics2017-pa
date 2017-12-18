#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t len) {
  return 0;
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) {
	strncpy(buf, dispinfo + offset, len);
}

void fb_write(const void *buf, off_t offset, size_t len) {
	int x, y;
	int len1, len2, len3;
	offset /= 4;
	y = offset / _screen.width;
	x = offset % _screen.width;
	
  //Log("fb_write x:%d y:%d len:%d\n", x, y, len);
	len /= 4;
	len1 = len2 = len3 = 0;
	
	// the first line
	len1 = len <= _screen.width - x ? len : _screen.width - x;
	_draw_rect((uint32_t *)buf, x, y, len1, 1);

	// the middle line
	if (len > len1 && ((len - len1) > _screen.width))	{
		len2 = (len - len1) / _screen.width * _screen.width;
		_draw_rect((uint32_t *)buf + len1, 0, y + 1, _screen.width, len2 / _screen.width);
	}	

	// the lase line
	if (len - len1 - len2 > 0) {
		len3 = len - len1 - len2;
		_draw_rect((uint32_t *)buf + len1 + len2, 0, y + len2 / _screen.width + 1, len3, 1);
	}
}

void init_device() {
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
	strcpy(dispinfo ,"WIDTH:400\nHEIGHT:300");
}
