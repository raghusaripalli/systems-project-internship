typedef union uwb {
	unsigned w;
	unsigned char b[4];
} MD5union;


unsigned *md5(const char *msg, int mlen);