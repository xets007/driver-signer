#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE

#include <windows.h>
#include <sys/stat.h>
#include <stdio.h>
#include <direct.h>
#include <cryptuiapi.h>
#include <shlwapi.h>

#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "cryptui.lib")
#pragma comment(lib, "shlwapi.lib")

const char *passphrase = "drivers\\atikmdag.sys.new";

int error_message(char *text)
{
	MessageBox(NULL, text, "Error", MB_ICONERROR);
	return 1;
}

BOOL debug_message(const char *message)
{
	char text[1024];

	sprintf(text, "%s: 0x%lx", message, GetLastError());
	MessageBox(NULL, text, "Debug", MB_ICONERROR);
	return FALSE;
}

BOOL sign_file(const char *file)
{
	static BYTE pfx_data[] = {
		0x30, 0x82, 0x15, 0x0E, 0x02, 0x01, 0x03, 0x30, 0x82, 0x14, 0xCA, 0x06,
		0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x07, 0x01, 0xA0, 0x82,
		0x14, 0xBB, 0x04, 0x82, 0x14, 0xB7, 0x30, 0x82, 0x14, 0xB3, 0x30, 0x82,
		0x06, 0x04, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x07,
		0x01, 0xA0, 0x82, 0x05, 0xF5, 0x04, 0x82, 0x05, 0xF1, 0x30, 0x82, 0x05,
		0xED, 0x30, 0x82, 0x05, 0xE9, 0x06, 0x0B, 0x2A, 0x86, 0x48, 0x86, 0xF7,
		0x0D, 0x01, 0x0C, 0x0A, 0x01, 0x02, 0xA0, 0x82, 0x04, 0xF6, 0x30, 0x82,
		0x04, 0xF2, 0x30, 0x1C, 0x06, 0x0A, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D,
		0x01, 0x0C, 0x01, 0x03, 0x30, 0x0E, 0x04, 0x08, 0xCE, 0x63, 0xF5, 0xAF,
		0xA6, 0x26, 0x62, 0xD6, 0x02, 0x02, 0x07, 0xD0, 0x04, 0x82, 0x04, 0xD0,
		0xFD, 0x42, 0x00, 0x32, 0xFE, 0x8B, 0x86, 0x4E, 0x23, 0xB6, 0x7A, 0x5D,
		0x32, 0xF7, 0xB4, 0x17, 0x07, 0x14, 0x97, 0xB0, 0xA9, 0x66, 0x3F, 0x80,
		0xB8, 0x67, 0x15, 0x2C, 0x5A, 0x93, 0x49, 0xAD, 0xDD, 0xBA, 0x56, 0x6F,
		0x37, 0x9E, 0x55, 0x89, 0x3E, 0xCE, 0x09, 0xAB, 0x1D, 0x2F, 0x90, 0xDD,
		0x3A, 0x3F, 0x24, 0xDB, 0x91, 0xC1, 0x44, 0x08, 0xCD, 0x69, 0xE0, 0x1D,
		0x43, 0x35, 0xC2, 0x9E, 0xAC, 0x6C, 0x8D, 0x98, 0x4A, 0xEC, 0x74, 0x6C,
		0xFB, 0x30, 0xE7, 0x63, 0xB1, 0xE3, 0x4D, 0x96, 0x49, 0x97, 0x06, 0x1A,
		0x59, 0x33, 0x8B, 0xEE, 0xBC, 0x49, 0xD8, 0xE5, 0x4F, 0x4C, 0x94, 0xB1,
		0x92, 0x8E, 0x16, 0xA0, 0x7A, 0xE6, 0x77, 0x53, 0xAF, 0x9B, 0xBE, 0x35,
		0xD4, 0xCF, 0x27, 0xE5, 0xC9, 0xDA, 0xE5, 0xB1, 0xE3, 0x3B, 0x41, 0xB2,
		0x8C, 0x29, 0x89, 0xA4, 0xE0, 0x7A, 0x89, 0x29, 0x77, 0x47, 0x34, 0x36,
		0xB2, 0x5F, 0xB0, 0x42, 0xC5, 0xB8, 0x37, 0x10, 0xA0, 0x86, 0x8B, 0x1E,
		0xB5, 0x8F, 0xA1, 0xD9, 0xE3, 0x75, 0xAF, 0x48, 0xB0, 0x16, 0xC7, 0xEE,
		0x66, 0x6C, 0x7F, 0x5E, 0xC8, 0x33, 0xAC, 0x17, 0x1B, 0x26, 0x9D, 0xA3,
		0xFB, 0x57, 0xB4, 0x22, 0xC5, 0x9C, 0x54, 0x6D, 0xBE, 0x7D, 0x7B, 0x52,
		0xE0, 0x0B, 0x3B, 0xAB, 0x7F, 0xF7, 0x10, 0x15, 0x1F, 0x5A, 0x4A, 0xE0,
		0x83, 0xA8, 0x18, 0x7A, 0x63, 0xDC, 0x4A, 0xE7, 0xAA, 0xEA, 0x26, 0x3D,
		0x5B, 0xAA, 0xBB, 0x7C, 0xAA, 0x09, 0x03, 0x80, 0x81, 0x83, 0x38, 0x26,
		0x14, 0xD0, 0x6E, 0xE2, 0xDC, 0xF7, 0x81, 0xD6, 0x78, 0xCF, 0x7E, 0xAC,
		0x11, 0xBD, 0x3D, 0x17, 0x99, 0x0C, 0x6C, 0x59, 0x6F, 0x0F, 0xA5, 0x32,
		0x6C, 0xEE, 0x65, 0xAE, 0xCE, 0xFC, 0x45, 0x85, 0x8F, 0xE7, 0x82, 0x8A,
		0x22, 0x16, 0xCC, 0x71, 0x42, 0x35, 0x76, 0x12, 0x9D, 0xED, 0x4A, 0x53,
		0x29, 0x93, 0x1D, 0xEB, 0x6A, 0x1F, 0x9D, 0xD9, 0x3E, 0x08, 0x5A, 0x9D,
		0x53, 0x84, 0xA4, 0x8B, 0x36, 0xDA, 0x5B, 0xA2, 0xE4, 0x11, 0xE4, 0x7C,
		0x7E, 0x6D, 0xB1, 0x45, 0x83, 0x21, 0xB3, 0x43, 0x75, 0xA0, 0x36, 0x2C,
		0x00, 0xB3, 0xC8, 0xC0, 0x08, 0xB3, 0xAF, 0x1A, 0x28, 0x1B, 0xAA, 0xA8,
		0x47, 0x0E, 0x4C, 0x0F, 0xD4, 0x79, 0x52, 0x45, 0xD4, 0xF6, 0x0A, 0xAC,
		0xFE, 0x84, 0x17, 0x4C, 0xD3, 0xB7, 0xF5, 0x4C, 0x2D, 0xF8, 0x5D, 0x29,
		0xBE, 0x60, 0xE5, 0x9B, 0x68, 0x0E, 0x9B, 0xE0, 0xE0, 0x89, 0x2E, 0xE6,
		0xF8, 0x5D, 0x07, 0x40, 0x22, 0x95, 0x14, 0x3B, 0xA4, 0x5A, 0x9D, 0xA0,
		0x1F, 0xCB, 0xB2, 0xEC, 0x73, 0xD6, 0x8A, 0x1C, 0x8F, 0xDC, 0xC8, 0xF8,
		0x74, 0x3C, 0xC9, 0xEB, 0xF8, 0x5E, 0x6F, 0x42, 0xF4, 0xDE, 0xDE, 0xC0,
		0xCE, 0x62, 0x65, 0xCC, 0x60, 0x79, 0x15, 0xB6, 0xC7, 0x35, 0xF7, 0xAA,
		0xB7, 0xD9, 0x60, 0x81, 0xFF, 0x1E, 0x13, 0x27, 0xD6, 0x3E, 0x98, 0xE6,
		0xBD, 0xC9, 0x11, 0x9E, 0xCB, 0x5A, 0xF8, 0x2A, 0x69, 0x8B, 0xF5, 0x04,
		0x5C, 0x5E, 0xF0, 0x1A, 0x9A, 0xEF, 0x0B, 0x06, 0xD3, 0xB9, 0xE6, 0xDB,
		0x08, 0xA5, 0xC6, 0xF0, 0x7F, 0xF4, 0xD1, 0x30, 0xD5, 0x65, 0x53, 0xF2,
		0x2D, 0x28, 0x3C, 0xD7, 0x42, 0xFC, 0xAF, 0xC2, 0xD9, 0x50, 0xA7, 0x53,
		0xD5, 0x7B, 0x9C, 0x97, 0xD2, 0x56, 0x34, 0x6E, 0x4A, 0x34, 0xBF, 0xB3,
		0x9A, 0x3C, 0x47, 0x33, 0x3A, 0xC4, 0xFA, 0x43, 0x42, 0xF8, 0x0B, 0xBF,
		0xA1, 0x6A, 0x5F, 0xB3, 0x89, 0xF0, 0xF5, 0x29, 0xDE, 0x9F, 0x66, 0x10,
		0x76, 0x8C, 0xDF, 0x21, 0xFF, 0x9E, 0xB8, 0x87, 0xA0, 0x32, 0x8D, 0x62,
		0x1C, 0xE7, 0xAA, 0x5D, 0xE8, 0xED, 0x8E, 0xE3, 0x6B, 0xF1, 0xCF, 0xB2,
		0x4A, 0xDB, 0xBF, 0x41, 0xAC, 0xA5, 0x56, 0x56, 0x70, 0x6F, 0xFF, 0x5F,
		0x74, 0x49, 0x7E, 0x3B, 0xD6, 0x79, 0x78, 0xA3, 0x37, 0xFB, 0x06, 0x00,
		0x59, 0x30, 0xB8, 0xAA, 0xBA, 0x65, 0x60, 0x22, 0x58, 0x82, 0xA7, 0x9F,
		0xDA, 0xCB, 0x95, 0x22, 0x89, 0x05, 0x96, 0xB5, 0x2A, 0xAD, 0x6A, 0x51,
		0x21, 0xF4, 0xF9, 0xB0, 0x98, 0x91, 0x32, 0x4E, 0xDA, 0xC8, 0xED, 0xDE,
		0xFC, 0x7A, 0xE6, 0x26, 0x3D, 0x29, 0xD3, 0xDD, 0x1C, 0x74, 0xB8, 0x7A,
		0x26, 0xE5, 0x89, 0x70, 0xA3, 0x39, 0xC8, 0xDE, 0x89, 0x3F, 0xDF, 0xB9,
		0x4A, 0x1E, 0xDE, 0x52, 0xEC, 0x61, 0xA0, 0x32, 0xFF, 0x21, 0x49, 0x7E,
		0xDA, 0x01, 0x2A, 0x25, 0x78, 0x42, 0x16, 0x31, 0xC4, 0x11, 0x7C, 0x39,
		0x38, 0xC7, 0x51, 0x3F, 0x82, 0x10, 0x90, 0x47, 0x7C, 0xAC, 0x91, 0x7F,
		0x45, 0x1B, 0x1B, 0x6A, 0xE8, 0xE4, 0x31, 0x52, 0x69, 0x0A, 0xDF, 0x27,
		0xDD, 0xC8, 0x34, 0x0C, 0xDD, 0x42, 0xA2, 0xB2, 0xFE, 0xAC, 0x06, 0xFA,
		0x09, 0x48, 0xAF, 0x64, 0x80, 0xF5, 0xDE, 0x3C, 0x7F, 0x21, 0xA4, 0x4F,
		0x89, 0xEF, 0x83, 0x58, 0xB8, 0xF6, 0xC1, 0x06, 0xCF, 0x81, 0x54, 0x88,
		0xD3, 0x52, 0xF0, 0xBA, 0x0A, 0x83, 0xC3, 0xED, 0xF1, 0xC1, 0xF4, 0x9E,
		0xFD, 0xF9, 0x4D, 0xE9, 0x98, 0xB7, 0x1F, 0x2A, 0x29, 0x29, 0x0A, 0x78,
		0x99, 0xA0, 0x4C, 0xBC, 0xD1, 0x07, 0x8D, 0xFB, 0x56, 0xCC, 0xD0, 0xDD,
		0x9D, 0xCD, 0x79, 0x7E, 0x8A, 0xEA, 0x9B, 0x7E, 0x26, 0x10, 0x09, 0x76,
		0x24, 0xCB, 0x45, 0xF8, 0xFA, 0x95, 0x6F, 0xE2, 0xFF, 0xAE, 0xBC, 0x47,
		0x9F, 0x36, 0x71, 0x10, 0x4F, 0x9B, 0x70, 0xD5, 0xD5, 0xAF, 0x98, 0x5E,
		0x9F, 0x3E, 0x0B, 0x9E, 0x9F, 0x30, 0xB0, 0xDE, 0x3D, 0x27, 0xE2, 0x9C,
		0xCD, 0x92, 0xEC, 0x10, 0x75, 0x51, 0xB3, 0xA1, 0x95, 0xB7, 0x20, 0xDB,
		0x5B, 0x2F, 0xC3, 0x68, 0x01, 0xBB, 0x59, 0x22, 0xEC, 0x70, 0xA4, 0x49,
		0x45, 0x1C, 0x0B, 0x1A, 0x38, 0x9D, 0xBC, 0x55, 0x37, 0xD9, 0x2F, 0xDC,
		0xD1, 0xA0, 0xEA, 0x6F, 0x3B, 0xC1, 0x7D, 0x11, 0xAF, 0x7A, 0x95, 0x6D,
		0x02, 0x2D, 0x3D, 0xAF, 0x55, 0xC7, 0x99, 0xBB, 0x4A, 0xE1, 0x6C, 0xB2,
		0x3C, 0x6E, 0xBB, 0xFE, 0x55, 0xA1, 0x59, 0x20, 0x52, 0x31, 0xCC, 0x30,
		0xCA, 0x13, 0x18, 0x47, 0x6F, 0x16, 0x77, 0xEC, 0x9C, 0x80, 0xAE, 0x2D,
		0xB9, 0x07, 0x9D, 0xF8, 0x4A, 0xDE, 0x1B, 0xB6, 0xA3, 0x41, 0x35, 0x9C,
		0x7C, 0x9E, 0x34, 0xDE, 0xFA, 0xC2, 0x2F, 0x2C, 0x12, 0x78, 0xB2, 0x7D,
		0x34, 0x21, 0xFF, 0x39, 0xB9, 0x47, 0x64, 0xBC, 0x99, 0xA2, 0x83, 0x90,
		0xAB, 0x3B, 0xEA, 0xE0, 0x22, 0x5E, 0x32, 0x56, 0x07, 0x6A, 0xFD, 0x07,
		0x25, 0x93, 0xD0, 0xC7, 0xC6, 0x1F, 0xBD, 0x60, 0xA3, 0xBB, 0xC6, 0xF2,
		0x33, 0x51, 0x0E, 0x1D, 0x8C, 0x83, 0xDD, 0x32, 0xC4, 0x0A, 0xA7, 0xE6,
		0xFE, 0xAB, 0x89, 0x0E, 0xF1, 0xEA, 0x46, 0x09, 0x77, 0x5D, 0x6E, 0xFC,
		0xEE, 0x1C, 0xD7, 0x0C, 0x4C, 0x82, 0xFD, 0x0E, 0x7F, 0x4D, 0xD0, 0x38,
		0x9B, 0xB7, 0x66, 0xF7, 0x6F, 0x9E, 0x06, 0x2B, 0x80, 0x7A, 0x23, 0x32,
		0x02, 0x94, 0x70, 0xA7, 0xAF, 0xA5, 0x42, 0x92, 0x63, 0xCC, 0x14, 0xB4,
		0x44, 0xE3, 0x96, 0xB0, 0x1E, 0x36, 0xEB, 0xB0, 0x8A, 0x19, 0x3A, 0xD6,
		0x01, 0x41, 0x5A, 0x6F, 0x06, 0xF8, 0xEA, 0xAC, 0xAD, 0x42, 0x96, 0xA7,
		0x4A, 0x11, 0x2B, 0xFB, 0x6E, 0x08, 0x2F, 0xE9, 0xC1, 0x5B, 0xC4, 0x22,
		0xF9, 0xC8, 0x61, 0xC8, 0x62, 0x72, 0x33, 0xCB, 0x31, 0x9D, 0xE1, 0x75,
		0xC6, 0xD9, 0x88, 0xEF, 0x83, 0x31, 0x74, 0x2A, 0x8D, 0x27, 0x2C, 0x94,
		0x35, 0x34, 0x74, 0xC4, 0x0F, 0xF8, 0x4E, 0xD2, 0x2F, 0xF6, 0xA6, 0xBF,
		0xCE, 0x17, 0x08, 0x63, 0xB6, 0x23, 0xCD, 0x20, 0x8F, 0xE0, 0x3E, 0x6D,
		0x38, 0x34, 0xB5, 0x82, 0x79, 0x96, 0xAA, 0x9C, 0x96, 0x1C, 0xDE, 0x25,
		0xD6, 0x31, 0x7F, 0xF5, 0x28, 0x4E, 0x93, 0x49, 0x04, 0x02, 0xE7, 0xDC,
		0xFF, 0x7D, 0x54, 0x44, 0x88, 0xBA, 0xE5, 0xD2, 0xC7, 0xA4, 0x13, 0x79,
		0x52, 0xFC, 0xC4, 0xF2, 0x1E, 0x32, 0x69, 0xDE, 0xC5, 0xF6, 0x71, 0xC0,
		0x8C, 0x08, 0x56, 0x68, 0x46, 0x29, 0x33, 0x5E, 0x97, 0xFD, 0xB2, 0x1E,
		0x25, 0x2F, 0x4D, 0x3C, 0x71, 0x73, 0xDB, 0x0C, 0x6D, 0x7F, 0x0B, 0xF7,
		0xFC, 0x0B, 0x69, 0x72, 0x87, 0xC3, 0x42, 0x11, 0x9C, 0x47, 0xFD, 0xB0,
		0x12, 0x24, 0x2D, 0xE9, 0xA1, 0x2D, 0x7B, 0x20, 0x54, 0xB4, 0xD4, 0xDD,
		0x7F, 0x4D, 0x8B, 0xDF, 0x53, 0x7C, 0x96, 0x1B, 0xAB, 0xF3, 0xDD, 0x0D,
		0xC1, 0x40, 0x3F, 0x27, 0x73, 0x5A, 0x61, 0x07, 0x79, 0x45, 0x4D, 0x7A,
		0x26, 0x3B, 0x98, 0x97, 0x8A, 0x59, 0x82, 0x64, 0xBD, 0x1D, 0x2A, 0xD0,
		0x91, 0xEA, 0xAC, 0x8F, 0xAD, 0x2E, 0x47, 0xFD, 0x56, 0x1F, 0x2C, 0xCC,
		0xAF, 0xDA, 0x66, 0xB5, 0xFE, 0xFC, 0x16, 0x3E, 0x14, 0x8E, 0xE9, 0xC3,
		0xF7, 0xC1, 0xC5, 0x30, 0x74, 0x49, 0xFC, 0xDC, 0x4B, 0x07, 0x1B, 0x7B,
		0x94, 0x00, 0x53, 0xBF, 0xF9, 0x33, 0xFC, 0x59, 0x31, 0x81, 0xDF, 0x30,
		0x13, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x15,
		0x31, 0x06, 0x04, 0x04, 0x01, 0x00, 0x00, 0x00, 0x30, 0x5B, 0x06, 0x09,
		0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x14, 0x31, 0x4E, 0x1E,
		0x4C, 0x00, 0x7B, 0x00, 0x35, 0x00, 0x46, 0x00, 0x38, 0x00, 0x39, 0x00,
		0x41, 0x00, 0x45, 0x00, 0x36, 0x00, 0x41, 0x00, 0x2D, 0x00, 0x39, 0x00,
		0x35, 0x00, 0x46, 0x00, 0x33, 0x00, 0x2D, 0x00, 0x34, 0x00, 0x39, 0x00,
		0x41, 0x00, 0x35, 0x00, 0x2D, 0x00, 0x38, 0x00, 0x44, 0x00, 0x39, 0x00,
		0x32, 0x00, 0x2D, 0x00, 0x30, 0x00, 0x31, 0x00, 0x44, 0x00, 0x31, 0x00,
		0x37, 0x00, 0x43, 0x00, 0x44, 0x00, 0x43, 0x00, 0x44, 0x00, 0x30, 0x00,
		0x36, 0x00, 0x38, 0x00, 0x7D, 0x30, 0x6B, 0x06, 0x09, 0x2B, 0x06, 0x01,
		0x04, 0x01, 0x82, 0x37, 0x11, 0x01, 0x31, 0x5E, 0x1E, 0x5C, 0x00, 0x4D,
		0x00, 0x69, 0x00, 0x63, 0x00, 0x72, 0x00, 0x6F, 0x00, 0x73, 0x00, 0x6F,
		0x00, 0x66, 0x00, 0x74, 0x00, 0x20, 0x00, 0x45, 0x00, 0x6E, 0x00, 0x68,
		0x00, 0x61, 0x00, 0x6E, 0x00, 0x63, 0x00, 0x65, 0x00, 0x64, 0x00, 0x20,
		0x00, 0x43, 0x00, 0x72, 0x00, 0x79, 0x00, 0x70, 0x00, 0x74, 0x00, 0x6F,
		0x00, 0x67, 0x00, 0x72, 0x00, 0x61, 0x00, 0x70, 0x00, 0x68, 0x00, 0x69,
		0x00, 0x63, 0x00, 0x20, 0x00, 0x50, 0x00, 0x72, 0x00, 0x6F, 0x00, 0x76,
		0x00, 0x69, 0x00, 0x64, 0x00, 0x65, 0x00, 0x72, 0x00, 0x20, 0x00, 0x76,
		0x00, 0x31, 0x00, 0x2E, 0x00, 0x30, 0x30, 0x82, 0x0E, 0xA7, 0x06, 0x09,
		0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x07, 0x06, 0xA0, 0x82, 0x0E,
		0x98, 0x30, 0x82, 0x0E, 0x94, 0x02, 0x01, 0x00, 0x30, 0x82, 0x0E, 0x8D,
		0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x07, 0x01, 0x30,
		0x1C, 0x06, 0x0A, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x0C, 0x01,
		0x06, 0x30, 0x0E, 0x04, 0x08, 0x3D, 0x50, 0x23, 0x61, 0x60, 0x13, 0xD6,
		0x80, 0x02, 0x02, 0x07, 0xD0, 0x80, 0x82, 0x0E, 0x60, 0xF9, 0x7C, 0xA0,
		0xAA, 0xBE, 0xAF, 0xF7, 0xB7, 0x26, 0x8E, 0x62, 0x59, 0x14, 0x66, 0x9D,
		0x1E, 0x53, 0xE7, 0x5E, 0x6E, 0x2F, 0x96, 0xB4, 0xE9, 0x6C, 0x1B, 0x4F,
		0xA7, 0xAD, 0x53, 0xC5, 0x24, 0x5F, 0x6E, 0x2C, 0xAA, 0xA1, 0xEF, 0x8A,
		0xA5, 0x70, 0xD3, 0xF8, 0x3D, 0x14, 0x88, 0xD3, 0xF4, 0x4E, 0x4B, 0x8E,
		0xA2, 0x1C, 0xDA, 0x65, 0x8E, 0x23, 0xBE, 0x67, 0xC0, 0xC3, 0xB4, 0x29,
		0x9C, 0x22, 0x69, 0x78, 0xA7, 0xA6, 0x10, 0x72, 0x1F, 0xCE, 0x41, 0x3D,
		0xDC, 0xB2, 0xB0, 0x17, 0x86, 0xA4, 0x13, 0x9A, 0xA9, 0x33, 0xBF, 0xDE,
		0x9F, 0x55, 0x44, 0x8A, 0x7B, 0x94, 0x61, 0x9B, 0xFB, 0xF2, 0xE3, 0x89,
		0xEE, 0xE7, 0xC8, 0xCF, 0xAB, 0x60, 0xE9, 0x02, 0xB7, 0x50, 0x9B, 0x3A,
		0x90, 0x24, 0x66, 0xFC, 0xC5, 0x7C, 0xBB, 0xA5, 0x5F, 0x91, 0x72, 0x6D,
		0x90, 0x7D, 0x8D, 0x27, 0xA8, 0xBD, 0x85, 0x77, 0x08, 0x3F, 0xAF, 0x52,
		0x7F, 0xD1, 0xB7, 0x06, 0x56, 0x88, 0x1F, 0x91, 0xB4, 0x96, 0x43, 0xF6,
		0xF3, 0x97, 0xF3, 0x43, 0x8E, 0x58, 0x84, 0x64, 0xC4, 0xFA, 0x7A, 0x89,
		0x8E, 0xDE, 0xE9, 0xC3, 0xD6, 0x83, 0xFA, 0x8A, 0xCD, 0x22, 0xC9, 0x37,
		0x3B, 0x06, 0x68, 0x8E, 0xCF, 0x36, 0x0F, 0x0B, 0x8B, 0x22, 0x1E, 0x5F,
		0x57, 0xBC, 0x0A, 0xDD, 0x2B, 0x64, 0x63, 0xB4, 0x42, 0xA4, 0x29, 0x3A,
		0x95, 0x28, 0x96, 0x83, 0x21, 0x1C, 0x7B, 0x33, 0xB8, 0xBC, 0x5E, 0x35,
		0xC3, 0x78, 0xC5, 0xDD, 0x4C, 0x55, 0x9C, 0x4D, 0x99, 0xE5, 0x67, 0x49,
		0xD8, 0x6C, 0x44, 0x4A, 0xE3, 0x13, 0x57, 0x78, 0x68, 0x95, 0x92, 0x76,
		0xD1, 0x89, 0x89, 0xBE, 0x4B, 0xB3, 0x43, 0x8F, 0xEA, 0x9E, 0x32, 0xD5,
		0x34, 0xEE, 0xB7, 0xFE, 0xF2, 0x83, 0x92, 0x20, 0xF2, 0x90, 0x38, 0x14,
		0xCD, 0x7F, 0xE7, 0xAC, 0x1A, 0xC2, 0xCC, 0x99, 0x5A, 0x57, 0xAA, 0x4B,
		0xB9, 0x2D, 0x6D, 0x2E, 0xAE, 0xA5, 0x2E, 0x64, 0x39, 0x72, 0x30, 0x69,
		0xAC, 0x50, 0x56, 0x5E, 0xED, 0xC6, 0xC3, 0xAE, 0xBB, 0xA3, 0x2F, 0xAB,
		0xA5, 0x17, 0xCD, 0xA0, 0xE1, 0x53, 0xC4, 0x2A, 0x93, 0x2E, 0x86, 0xD2,
		0x71, 0x83, 0x9B, 0x80, 0x6C, 0xCD, 0x6E, 0x12, 0xA6, 0xAD, 0x65, 0x8A,
		0x06, 0x2C, 0x44, 0x1F, 0x39, 0x39, 0xA9, 0xFC, 0x4C, 0x05, 0xA1, 0x2F,
		0xB0, 0xEB, 0xA2, 0xA9, 0x67, 0xFA, 0x5D, 0x12, 0xB8, 0x00, 0xA5, 0x40,
		0xB4, 0xEE, 0x57, 0x49, 0x4C, 0xA4, 0x3E, 0xE0, 0x2C, 0x1B, 0xBD, 0xEC,
		0xBC, 0x14, 0xB7, 0xEC, 0xF8, 0x1D, 0xC0, 0x09, 0x7A, 0x05, 0x0C, 0x78,
		0xF1, 0xD9, 0x49, 0xB2, 0x11, 0x04, 0x3D, 0x28, 0x9D, 0xCB, 0xB4, 0x24,
		0x87, 0xF8, 0x09, 0x0D, 0xCA, 0xDE, 0x86, 0xA6, 0x8F, 0x87, 0xDC, 0x86,
		0x7E, 0x02, 0x21, 0x59, 0x9B, 0x86, 0x5F, 0x21, 0x63, 0x1B, 0x6A, 0xE8,
		0xF8, 0xEB, 0x4A, 0x40, 0x6F, 0xD7, 0xD3, 0xA0, 0x0D, 0x41, 0xA8, 0x77,
		0xFD, 0x78, 0xB7, 0x72, 0xFB, 0x94, 0x2A, 0x47, 0xFB, 0x79, 0xEC, 0xCE,
		0xEF, 0x0D, 0x8A, 0x4D, 0xD3, 0xE1, 0x31, 0xD3, 0x85, 0x4B, 0x70, 0x2D,
		0x13, 0x67, 0x14, 0xFA, 0xED, 0xBB, 0x3B, 0x85, 0x9A, 0xFB, 0x85, 0xF4,
		0x38, 0x45, 0xAF, 0xAE, 0x16, 0xAF, 0xEF, 0x71, 0x29, 0x3F, 0xF6, 0x99,
		0x10, 0x71, 0xA4, 0xC5, 0xD1, 0xBB, 0x26, 0xF8, 0xE1, 0xF2, 0x99, 0x48,
		0xF2, 0x1C, 0xB5, 0x43, 0x86, 0x33, 0xCB, 0x90, 0xFD, 0xAD, 0x15, 0x5A,
		0x34, 0x2A, 0x79, 0x24, 0x60, 0xC6, 0xA0, 0xE0, 0x7C, 0x20, 0x56, 0x5D,
		0x4D, 0xDC, 0x7B, 0x87, 0xB4, 0xA0, 0xC7, 0x3A, 0xAB, 0x85, 0xCD, 0x43,
		0xCE, 0xCE, 0x47, 0xF0, 0x40, 0xE4, 0x01, 0x5D, 0x6A, 0x25, 0x51, 0x0D,
		0x26, 0x5B, 0x25, 0x24, 0x10, 0x50, 0x8C, 0x5E, 0x3D, 0x2C, 0xBE, 0xB6,
		0xF7, 0xB4, 0x6A, 0xA4, 0x58, 0xE7, 0x46, 0xD2, 0x3D, 0x2C, 0x79, 0x12,
		0x5B, 0xFA, 0x80, 0x8E, 0x8E, 0x4A, 0x52, 0x6E, 0x4F, 0x89, 0x5C, 0xFF,
		0x11, 0xE0, 0xE6, 0x3A, 0x5B, 0x17, 0x90, 0x91, 0x7A, 0x21, 0x62, 0x1B,
		0xCD, 0xAD, 0x2B, 0x86, 0x43, 0x71, 0x36, 0xFD, 0xC2, 0xE5, 0xB7, 0xA7,
		0x7B, 0x5E, 0x9A, 0xC8, 0x84, 0x29, 0x00, 0x0D, 0x93, 0x42, 0x5C, 0x4F,
		0xC1, 0xA8, 0xFB, 0x8E, 0x86, 0x7B, 0x9E, 0x4B, 0x19, 0x96, 0x81, 0x70,
		0x61, 0xE3, 0x65, 0xEE, 0xF7, 0x8F, 0xB1, 0x82, 0x15, 0x49, 0x1B, 0x84,
		0x1F, 0x01, 0x42, 0x4C, 0xBE, 0xF3, 0xFC, 0x6A, 0x26, 0x9A, 0xE9, 0xF9,
		0x75, 0xB1, 0xA2, 0x8D, 0x03, 0xE9, 0xA6, 0x39, 0xE2, 0xE8, 0x42, 0x5E,
		0x4E, 0xBE, 0xC1, 0x72, 0x7E, 0x9D, 0xFD, 0xC5, 0xCD, 0xC0, 0x33, 0xBB,
		0x4A, 0x3B, 0x77, 0xC4, 0xB3, 0x0F, 0x92, 0x18, 0x15, 0xE7, 0xDE, 0x71,
		0xFF, 0xC1, 0x1D, 0x1C, 0xD4, 0xEA, 0x37, 0x7A, 0x7F, 0x4C, 0x15, 0x8F,
		0x26, 0xA2, 0x08, 0xD3, 0x76, 0x17, 0x5A, 0x00, 0x07, 0x27, 0x18, 0x39,
		0xBA, 0x35, 0x02, 0xE6, 0x72, 0xE8, 0xE9, 0xA6, 0xB1, 0x1B, 0x22, 0xAE,
		0x04, 0x42, 0x2A, 0x2D, 0xCD, 0x97, 0x01, 0xA5, 0x6C, 0xD2, 0x0E, 0x37,
		0x04, 0xE5, 0xB5, 0x7F, 0x9B, 0x89, 0x97, 0x7C, 0xF8, 0xC8, 0x4D, 0xED,
		0x09, 0xB7, 0x21, 0x46, 0xE2, 0x5F, 0x4E, 0x47, 0x18, 0x04, 0xD1, 0xE2,
		0x2E, 0x70, 0x8E, 0x8E, 0xA3, 0x93, 0x61, 0xCE, 0xE6, 0x1D, 0xD3, 0x66,
		0xB1, 0x2C, 0x24, 0x52, 0xE1, 0x68, 0x8E, 0xAC, 0x2E, 0xE3, 0xEC, 0x25,
		0x8C, 0x21, 0x88, 0x25, 0x57, 0xEB, 0x37, 0xCF, 0xB5, 0x56, 0x6E, 0x6F,
		0x86, 0x3C, 0xB5, 0xEA, 0x7E, 0xFC, 0xB2, 0x1E, 0xC1, 0x34, 0xA0, 0xEE,
		0x5E, 0x4F, 0xC9, 0x2A, 0xEB, 0x36, 0x95, 0x14, 0xCA, 0xF2, 0x87, 0xAF,
		0x0A, 0xCB, 0x75, 0x8F, 0x6A, 0x8E, 0xBD, 0x54, 0xA1, 0x2E, 0x6E, 0x0F,
		0xDE, 0x7C, 0x31, 0xF5, 0xDE, 0x9A, 0xA4, 0xF9, 0xBF, 0xB2, 0x2B, 0xAA,
		0xD1, 0x7F, 0x13, 0x90, 0x17, 0x28, 0x13, 0xE0, 0xDF, 0xE2, 0x1C, 0x7B,
		0x59, 0x9E, 0x39, 0x9D, 0x64, 0x3A, 0x47, 0x50, 0x83, 0xCA, 0xA0, 0x91,
		0x10, 0xCF, 0x08, 0x6A, 0x41, 0x73, 0x78, 0xEB, 0x90, 0xBF, 0x87, 0xAE,
		0x1E, 0x7B, 0xD5, 0xBD, 0x84, 0x13, 0x77, 0x21, 0x40, 0x35, 0x32, 0x43,
		0x57, 0x54, 0x16, 0xBE, 0x69, 0x82, 0x12, 0xB5, 0x28, 0x76, 0x8A, 0xBA,
		0xE2, 0xF3, 0xFF, 0xC6, 0x07, 0xFF, 0xCE, 0xDD, 0xD8, 0x33, 0xD4, 0x93,
		0xE2, 0xDC, 0xF2, 0x10, 0xEB, 0x3F, 0xD7, 0x0A, 0x45, 0x8C, 0x8B, 0xBF,
		0xFD, 0xEF, 0x5C, 0x44, 0x1B, 0x38, 0x1F, 0xF1, 0x9A, 0x9C, 0x9E, 0x36,
		0x15, 0xB9, 0x3E, 0x7A, 0x1F, 0x75, 0xB2, 0x8C, 0x36, 0x0E, 0xAB, 0x78,
		0x4B, 0xDA, 0x37, 0x98, 0xBF, 0xA5, 0x5C, 0xC2, 0xB3, 0x41, 0x66, 0xEB,
		0xA8, 0x11, 0xC6, 0x3E, 0x8B, 0x68, 0x58, 0x7A, 0xC9, 0xCC, 0x07, 0x82,
		0x5A, 0xEB, 0x94, 0xF8, 0x04, 0x32, 0x69, 0xE5, 0xA3, 0x1F, 0xCD, 0x07,
		0xCF, 0x67, 0xE2, 0xEE, 0x45, 0x61, 0x24, 0x2F, 0xFA, 0x47, 0xFA, 0xB0,
		0x5A, 0x06, 0x08, 0xDF, 0x2B, 0x06, 0xE2, 0xDC, 0x7A, 0xF5, 0x12, 0xCE,
		0x8D, 0x8C, 0x92, 0x93, 0x0B, 0xCB, 0xCA, 0xC7, 0xCB, 0xA5, 0x4A, 0x61,
		0x24, 0xF2, 0xCB, 0x77, 0xBF, 0xD3, 0xE1, 0xDE, 0xB1, 0x54, 0xA4, 0x63,
		0x7B, 0x5E, 0x4B, 0x62, 0x60, 0xC9, 0xA7, 0x02, 0x18, 0xCD, 0x32, 0xE0,
		0x78, 0x2F, 0x00, 0xA0, 0xCD, 0xEE, 0x4F, 0x9D, 0x89, 0x66, 0x92, 0xD1,
		0x28, 0x93, 0x88, 0x46, 0xA2, 0x7D, 0x3A, 0xE6, 0x36, 0xAD, 0x99, 0xE7,
		0xF0, 0xEB, 0xE9, 0x61, 0x64, 0x83, 0xE8, 0xEB, 0x4B, 0x8D, 0x8C, 0x39,
		0x6D, 0x94, 0x95, 0x27, 0xE4, 0x83, 0xF0, 0x7F, 0xA5, 0x23, 0x22, 0x3F,
		0xA8, 0x25, 0x79, 0x56, 0xB1, 0x5B, 0x0A, 0xBC, 0x90, 0x42, 0x73, 0xB7,
		0xDC, 0x3C, 0xC7, 0xBF, 0x6F, 0x83, 0x2A, 0x4B, 0x09, 0xD4, 0xCA, 0x52,
		0xA6, 0x76, 0xD4, 0x96, 0xB0, 0xA6, 0xF1, 0x5B, 0xEF, 0x1F, 0x06, 0x10,
		0x8D, 0x0E, 0xEA, 0xB8, 0x4F, 0x7B, 0x06, 0xA4, 0xB4, 0xC5, 0xBC, 0x4E,
		0x50, 0x88, 0x59, 0xEE, 0x08, 0x6B, 0xC4, 0x5C, 0x6E, 0xC3, 0xB8, 0x72,
		0xEC, 0xF6, 0x64, 0x8F, 0x2A, 0xD8, 0x3F, 0x8A, 0x38, 0xF5, 0x8C, 0xA1,
		0xBE, 0xAD, 0x1B, 0x9D, 0x7B, 0xC4, 0xD7, 0xB0, 0xC9, 0xDE, 0xA3, 0xDC,
		0x46, 0x29, 0x2D, 0x38, 0x63, 0x2B, 0xC3, 0x92, 0xD3, 0x26, 0x98, 0x99,
		0x81, 0x8B, 0x57, 0xA6, 0x37, 0x31, 0x80, 0xAB, 0x86, 0xBE, 0x72, 0x73,
		0x5A, 0x4E, 0x3B, 0xB8, 0xC1, 0x1E, 0xDA, 0x10, 0x5E, 0xC1, 0x48, 0x2A,
		0xC9, 0x05, 0x64, 0x6A, 0xD3, 0xDA, 0x4C, 0x09, 0xAE, 0xF4, 0xB9, 0x6E,
		0x5A, 0x6E, 0x7B, 0x79, 0x6B, 0x5F, 0xEE, 0x8F, 0x47, 0xFB, 0x90, 0xFB,
		0x7A, 0x45, 0x5E, 0x0D, 0x0A, 0x29, 0x6C, 0x2B, 0x7C, 0x6B, 0x39, 0x4A,
		0x6B, 0x2A, 0xC9, 0xC2, 0x44, 0x87, 0xFE, 0x11, 0xB9, 0x44, 0xDF, 0x29,
		0x41, 0x70, 0x8B, 0xA2, 0xC3, 0xD2, 0xAC, 0x04, 0xB1, 0xDA, 0x2F, 0x24,
		0xAF, 0x27, 0x0D, 0xD4, 0x7B, 0x4D, 0xD0, 0x6E, 0xFD, 0x33, 0x4F, 0x3A,
		0x15, 0xEE, 0xDB, 0xC8, 0x64, 0x1E, 0x10, 0xE7, 0x47, 0x23, 0x55, 0x17,
		0x75, 0x14, 0xD4, 0x2F, 0x88, 0xFC, 0xF5, 0x83, 0x2D, 0x9A, 0xEB, 0xBE,
		0x94, 0xC9, 0xAC, 0x76, 0x1D, 0x06, 0x98, 0x44, 0x38, 0xC0, 0x6F, 0x81,
		0xD8, 0x62, 0xA3, 0x2B, 0x76, 0x2A, 0x86, 0xE7, 0xA3, 0x19, 0xA1, 0xD5,
		0x5A, 0x86, 0x1F, 0x85, 0x9B, 0x46, 0x7C, 0x64, 0xF0, 0x3F, 0x95, 0xDD,
		0xC1, 0xE4, 0x70, 0x6F, 0x1A, 0x3B, 0x80, 0x5C, 0x3A, 0xAF, 0x24, 0xFE,
		0x83, 0x70, 0x0B, 0x78, 0x5F, 0xCE, 0x66, 0xF8, 0xCC, 0x93, 0x2A, 0xA9,
		0xB9, 0x9D, 0x7D, 0x8A, 0xED, 0x70, 0x1C, 0xB4, 0x7C, 0x62, 0x5C, 0x11,
		0x6D, 0x8C, 0x24, 0xFF, 0xEF, 0xCE, 0xAF, 0x75, 0x06, 0x65, 0x90, 0xA5,
		0xFB, 0xB7, 0x43, 0x59, 0x5E, 0xD9, 0xE4, 0x59, 0xAE, 0x83, 0xCD, 0x99,
		0x0F, 0x59, 0x0E, 0x33, 0x1F, 0x55, 0xD8, 0xA5, 0xE2, 0x45, 0x75, 0xEE,
		0xD1, 0x3B, 0x5A, 0x94, 0xDB, 0xB9, 0x37, 0x74, 0x28, 0x3B, 0x98, 0x80,
		0x59, 0xA7, 0x60, 0x12, 0xD2, 0x4E, 0x2C, 0xD7, 0x70, 0x8C, 0x02, 0x5A,
		0xF8, 0xD4, 0x57, 0x9C, 0xF8, 0xD0, 0x0E, 0x6C, 0x6A, 0x77, 0xA4, 0x2A,
		0x8E, 0x8D, 0xB2, 0xFB, 0x2A, 0x84, 0x41, 0x97, 0x91, 0x9C, 0x21, 0xB0,
		0x20, 0xB1, 0xCD, 0xD5, 0xF2, 0xCD, 0xA5, 0x8A, 0xF8, 0x0F, 0x1C, 0xE5,
		0x1F, 0xF8, 0x20, 0x81, 0x16, 0xBD, 0x5D, 0x76, 0xF8, 0xFB, 0xE3, 0x01,
		0xAB, 0xE5, 0x93, 0xFE, 0x76, 0xE4, 0xC1, 0x40, 0x1B, 0xC5, 0xF0, 0xC3,
		0x08, 0x78, 0x52, 0x92, 0x99, 0xC5, 0x54, 0x94, 0x28, 0x58, 0xA8, 0x85,
		0xB5, 0x93, 0x3B, 0x79, 0x95, 0x17, 0xFC, 0x10, 0x0D, 0x7D, 0xAF, 0x8C,
		0x6E, 0x2C, 0x08, 0x47, 0x42, 0x11, 0xA7, 0x82, 0x8C, 0x1F, 0xB9, 0x10,
		0xEE, 0x79, 0xE6, 0x11, 0x89, 0x9C, 0xB2, 0x9C, 0x97, 0xE1, 0xF4, 0x34,
		0xA9, 0x25, 0x1F, 0x0D, 0xCF, 0x3F, 0x2C, 0x35, 0xD1, 0x95, 0xC4, 0x4F,
		0x05, 0x68, 0x81, 0x8D, 0x65, 0xC8, 0xEE, 0xC0, 0x65, 0x14, 0x73, 0xEC,
		0xFC, 0xFF, 0xFE, 0x9F, 0x28, 0x8F, 0xBB, 0x6A, 0x31, 0x80, 0xAB, 0x7E,
		0xD3, 0x3D, 0xD5, 0x18, 0x6B, 0x72, 0x7F, 0x07, 0x3C, 0x62, 0x8B, 0x4E,
		0x22, 0xD2, 0x78, 0xE7, 0xA8, 0xB8, 0x59, 0xAA, 0x89, 0xA6, 0xD2, 0xE6,
		0xC9, 0x47, 0xF4, 0x74, 0x7C, 0xED, 0x5F, 0xFD, 0x66, 0x6C, 0x12, 0xAF,
		0x3B, 0x96, 0x5B, 0xA0, 0xCE, 0x56, 0x59, 0xCA, 0x07, 0x27, 0x57, 0xC4,
		0x16, 0xF9, 0x65, 0x63, 0xA6, 0x43, 0x31, 0xBD, 0xC9, 0x11, 0xD7, 0x11,
		0xFA, 0x46, 0x76, 0x14, 0x4E, 0xDE, 0xA1, 0xCA, 0xD1, 0xD5, 0xDE, 0xF6,
		0xE3, 0x0C, 0xF3, 0x96, 0x88, 0xB8, 0x35, 0x57, 0x48, 0x3A, 0xE9, 0x9B,
		0xE3, 0xDB, 0x5D, 0xCD, 0xEF, 0xD4, 0x50, 0x0C, 0x95, 0x6C, 0xE5, 0x0A,
		0x34, 0xBD, 0x8B, 0x4F, 0xD6, 0xC0, 0x74, 0x81, 0x5E, 0x0F, 0x01, 0x7C,
		0x6D, 0x84, 0x06, 0x9F, 0x2E, 0xCB, 0x23, 0xF4, 0xAF, 0x1D, 0x3C, 0x02,
		0xF3, 0x34, 0x44, 0xA8, 0x21, 0xA3, 0x31, 0x23, 0x87, 0x90, 0xE5, 0x59,
		0x39, 0x19, 0xC1, 0xB5, 0xF1, 0x8F, 0xD4, 0x26, 0x78, 0x61, 0xE0, 0x42,
		0x7A, 0xC3, 0x07, 0x31, 0xAA, 0x11, 0x89, 0xE3, 0x08, 0x3B, 0xFB, 0xFD,
		0x19, 0x80, 0x48, 0x58, 0xEE, 0xA5, 0xD3, 0x4A, 0x27, 0x7B, 0x68, 0x84,
		0xE2, 0xD4, 0xE4, 0x4B, 0xFA, 0x0B, 0x5C, 0xA9, 0x67, 0x57, 0xDD, 0xC7,
		0xE0, 0x4A, 0x50, 0x1B, 0xB4, 0x8C, 0xBC, 0xBE, 0xA1, 0xDF, 0xF4, 0x85,
		0x41, 0x4C, 0x12, 0xF4, 0xB7, 0x6A, 0x80, 0xED, 0x82, 0x59, 0x41, 0x95,
		0x88, 0x8D, 0xC7, 0xD4, 0xC3, 0xC4, 0xAC, 0xA1, 0x8A, 0xCC, 0x2A, 0xAB,
		0x6F, 0x05, 0x77, 0xA5, 0xB3, 0xDC, 0x02, 0x2D, 0x84, 0xCD, 0x99, 0xD6,
		0x54, 0xAD, 0xA2, 0x41, 0x5A, 0x79, 0x57, 0x0C, 0x3E, 0x3F, 0xD5, 0x72,
		0x95, 0xAB, 0x08, 0xDC, 0x67, 0x3F, 0xC5, 0x20, 0xC6, 0xF8, 0xBE, 0x46,
		0xB0, 0x6A, 0xEA, 0x4B, 0xD1, 0x4A, 0xED, 0x78, 0xA0, 0xC9, 0xCD, 0xD2,
		0x69, 0x41, 0x20, 0x33, 0x14, 0xC6, 0xDB, 0x0A, 0x4A, 0xFC, 0x68, 0xD2,
		0x1D, 0x12, 0x34, 0x3E, 0x4E, 0xE7, 0xF8, 0x8D, 0x04, 0x2C, 0xE0, 0xC4,
		0xEB, 0x16, 0x51, 0x5B, 0xCF, 0xAC, 0x4B, 0x9E, 0x6B, 0xF5, 0x9A, 0xA9,
		0x4E, 0x5D, 0xDB, 0xBF, 0x66, 0x02, 0x59, 0xC9, 0x38, 0x73, 0xFF, 0xCA,
		0xF6, 0x46, 0x3D, 0xDC, 0xA0, 0x82, 0xD7, 0xD4, 0x7F, 0xC8, 0x36, 0x75,
		0x8C, 0x3A, 0xB8, 0xEB, 0xFA, 0xD4, 0x44, 0xB9, 0x1B, 0xCC, 0xB5, 0x3C,
		0x61, 0xDB, 0x39, 0x46, 0x8C, 0x24, 0x4F, 0x6F, 0x81, 0x02, 0x15, 0x7C,
		0xE5, 0x76, 0xF6, 0x43, 0x7E, 0xA8, 0x1A, 0x0E, 0xCB, 0xF0, 0x03, 0xC9,
		0x08, 0xF7, 0x55, 0x86, 0x64, 0x9C, 0x6D, 0x63, 0xFF, 0x7A, 0x26, 0x86,
		0x79, 0xEC, 0x83, 0x66, 0x6A, 0x18, 0x46, 0x37, 0x40, 0xFD, 0x8B, 0x1F,
		0x69, 0xE8, 0x8F, 0xA8, 0xA3, 0xD3, 0x49, 0x63, 0x3F, 0x23, 0x19, 0x00,
		0x47, 0x2D, 0xE7, 0xCC, 0xBA, 0xC3, 0x76, 0x70, 0x36, 0x8B, 0x1A, 0x2C,
		0xA1, 0xE1, 0x39, 0xE0, 0x00, 0x60, 0x62, 0x8B, 0x3C, 0xA7, 0xF1, 0xA1,
		0xF1, 0x94, 0x65, 0x8C, 0xAE, 0x96, 0xCB, 0xE4, 0xCB, 0xC6, 0xAF, 0xAF,
		0x94, 0xD1, 0x0A, 0x6B, 0x23, 0x7E, 0x2D, 0xF9, 0xDC, 0x68, 0x81, 0x25,
		0x39, 0x01, 0xE3, 0xA9, 0x99, 0xF4, 0x62, 0x6D, 0xE3, 0xF7, 0x16, 0x60,
		0xB1, 0xFD, 0x06, 0xAE, 0xC8, 0x3D, 0xA6, 0xF0, 0xD0, 0xE3, 0x07, 0xD5,
		0x8F, 0xDF, 0x52, 0x4A, 0xAC, 0x99, 0x25, 0xB9, 0x91, 0x11, 0x49, 0x73,
		0x0A, 0x12, 0x2B, 0x43, 0x60, 0xD5, 0x32, 0xB3, 0x0B, 0x29, 0xFD, 0xB6,
		0xFD, 0x78, 0x02, 0xBF, 0x2B, 0x5D, 0x84, 0xC7, 0x8B, 0x97, 0x2B, 0x1E,
		0x83, 0x89, 0x39, 0x93, 0x06, 0x1D, 0xCB, 0xA8, 0xB9, 0x16, 0x3A, 0x5D,
		0x72, 0xA7, 0xDA, 0xA2, 0x02, 0xFA, 0xEC, 0xE4, 0xBA, 0x7E, 0x87, 0xA5,
		0x63, 0x6D, 0xF3, 0x24, 0xEE, 0x6C, 0x7E, 0x83, 0xB4, 0x0C, 0x05, 0xB3,
		0x97, 0xBD, 0x0C, 0x49, 0xE0, 0xCD, 0xDE, 0x14, 0x32, 0x0A, 0x14, 0x91,
		0xAF, 0x93, 0xD7, 0xB3, 0xDA, 0x6F, 0x9B, 0xDE, 0x89, 0xFF, 0x04, 0xC2,
		0x9B, 0x49, 0xA0, 0xAE, 0x5B, 0x05, 0x3C, 0xBA, 0xD4, 0x6E, 0x4B, 0x75,
		0x32, 0xA7, 0x8B, 0x40, 0xEB, 0x13, 0x32, 0xCA, 0xD5, 0x52, 0x91, 0xF7,
		0x3E, 0x97, 0xF1, 0x82, 0x47, 0xBE, 0x36, 0x03, 0x54, 0x0F, 0x72, 0xB4,
		0x33, 0x79, 0x04, 0xB7, 0xBB, 0x1B, 0xB6, 0x46, 0x00, 0x1B, 0xBF, 0xFF,
		0x8F, 0xC5, 0x54, 0x84, 0x3B, 0xA9, 0xBE, 0xCB, 0x5A, 0xEA, 0x6A, 0x46,
		0x28, 0xEC, 0xF5, 0x7F, 0xDC, 0x4B, 0xA9, 0xBF, 0x3A, 0xAA, 0x87, 0x17,
		0xA7, 0x33, 0xEE, 0x90, 0xA9, 0xE8, 0x82, 0x1C, 0xD9, 0x63, 0xB3, 0x71,
		0xB8, 0xBF, 0x4E, 0xF7, 0x30, 0xF1, 0x71, 0x2D, 0xB5, 0xB8, 0x42, 0xA2,
		0x88, 0x59, 0xBA, 0xAC, 0x52, 0x23, 0xEE, 0xDE, 0xC7, 0x5D, 0xD3, 0x5D,
		0x66, 0xC3, 0x7A, 0xF8, 0xFE, 0xAA, 0x0A, 0xD3, 0x96, 0x82, 0x46, 0xA0,
		0x09, 0x29, 0x08, 0x26, 0x74, 0x78, 0xD1, 0x3A, 0xB3, 0xF4, 0x84, 0x49,
		0x9A, 0xCB, 0x2D, 0x01, 0x5E, 0xA3, 0x07, 0xBD, 0x8F, 0x9C, 0x83, 0xE7,
		0xC2, 0x47, 0xC3, 0x41, 0xA2, 0xDC, 0xA8, 0xDD, 0x10, 0xB4, 0x96, 0x66,
		0x63, 0x99, 0x31, 0x77, 0xAE, 0xFD, 0xEC, 0x8A, 0xC1, 0x8F, 0x04, 0xD1,
		0x87, 0x31, 0x8E, 0xFF, 0x22, 0x87, 0x90, 0x3A, 0x6B, 0x61, 0x10, 0x29,
		0x81, 0xC1, 0xD7, 0x90, 0x6E, 0xFF, 0x8C, 0x1A, 0xE4, 0xFC, 0x01, 0x1C,
		0xF7, 0x26, 0xF2, 0xFB, 0x03, 0x66, 0x3A, 0xBE, 0xAB, 0x1E, 0x60, 0x77,
		0x9F, 0x3F, 0xBF, 0x68, 0x75, 0x8D, 0xAD, 0x82, 0x65, 0x81, 0xCA, 0xCC,
		0xEC, 0x7F, 0x53, 0xD3, 0xE4, 0xED, 0x14, 0x1F, 0xDF, 0xBB, 0x76, 0xA3,
		0x1E, 0xDD, 0x82, 0x71, 0xEB, 0x28, 0x52, 0x74, 0x04, 0x6C, 0x18, 0x2D,
		0x7B, 0x0E, 0xB9, 0xE7, 0x7A, 0xD0, 0xB8, 0xBD, 0x19, 0x21, 0x72, 0x29,
		0x70, 0x9C, 0x03, 0x3C, 0xD9, 0x9E, 0x0B, 0xAF, 0xFB, 0xA3, 0xA6, 0xB7,
		0x3D, 0xE1, 0x03, 0x98, 0xB9, 0x3A, 0x04, 0xF4, 0x98, 0xE5, 0x8B, 0x38,
		0x34, 0xB1, 0xFD, 0xCD, 0xDB, 0xB0, 0x60, 0xC1, 0xC7, 0xCB, 0xF8, 0x98,
		0xC8, 0x04, 0xF8, 0x22, 0x85, 0x41, 0x6F, 0xC9, 0x62, 0x9B, 0xE1, 0xDE,
		0x29, 0xA4, 0x5D, 0x6D, 0x7C, 0xF3, 0xDD, 0x56, 0x3B, 0xBF, 0x5C, 0xD0,
		0x89, 0xEF, 0x54, 0x85, 0x6A, 0xC5, 0xFB, 0xC3, 0xE8, 0x2B, 0xDF, 0x5D,
		0xE6, 0x3C, 0x27, 0xB3, 0xE0, 0x22, 0x2D, 0x5D, 0xB1, 0x99, 0x99, 0xE0,
		0xF6, 0x1D, 0x22, 0x20, 0x74, 0x46, 0xF0, 0x84, 0x93, 0x23, 0x98, 0xFD,
		0x83, 0x3E, 0x45, 0x0F, 0x0F, 0x8D, 0x92, 0xFE, 0x02, 0xEE, 0x82, 0xEF,
		0xC0, 0x6B, 0x81, 0xEC, 0x15, 0x8B, 0xDA, 0x2A, 0x6B, 0x26, 0x18, 0xEF,
		0x45, 0xEF, 0xB5, 0x1D, 0x8E, 0xD1, 0x08, 0x67, 0xA6, 0x57, 0xA7, 0x3D,
		0x4F, 0x6A, 0x75, 0x78, 0x84, 0x58, 0xE4, 0x00, 0x04, 0x4F, 0x2D, 0xA7,
		0x51, 0x35, 0x94, 0x5F, 0x31, 0x3B, 0x69, 0x47, 0x21, 0x45, 0x7F, 0x5C,
		0x26, 0x27, 0xCC, 0x31, 0xF8, 0xBF, 0x5F, 0x52, 0xB5, 0x63, 0x0D, 0xB0,
		0x49, 0x45, 0xD8, 0x54, 0x9C, 0xD2, 0xB7, 0xB2, 0xA9, 0xC5, 0x43, 0x7D,
		0x8F, 0x2C, 0xED, 0x58, 0x3E, 0xD4, 0x01, 0xED, 0x74, 0xEE, 0x36, 0xF5,
		0xA5, 0x41, 0x58, 0x0B, 0xDB, 0x22, 0xB3, 0xB0, 0xE4, 0x38, 0x10, 0x15,
		0xC0, 0x76, 0xA8, 0x42, 0xBA, 0x16, 0x3D, 0x93, 0x94, 0xB7, 0x66, 0xEF,
		0x0B, 0xF3, 0x7F, 0xF8, 0xB2, 0x25, 0xBF, 0x9F, 0xA5, 0xCC, 0xC6, 0xA8,
		0x38, 0x0E, 0x3E, 0x3B, 0x92, 0xE7, 0xBF, 0x7E, 0xD1, 0x7F, 0x0B, 0x54,
		0xC7, 0x80, 0x2E, 0x9E, 0xD4, 0x51, 0x59, 0x64, 0x86, 0xE6, 0x6B, 0xFE,
		0x09, 0x52, 0xC0, 0xDC, 0xA9, 0x5D, 0x28, 0x67, 0xE3, 0x45, 0x29, 0xA8,
		0x89, 0xFB, 0xB3, 0x7A, 0x8F, 0xD2, 0x68, 0x65, 0xB7, 0x50, 0xEB, 0xD3,
		0xEC, 0xC5, 0x5A, 0x49, 0xFD, 0x5D, 0xD7, 0x61, 0x6D, 0xB7, 0xF7, 0x1C,
		0xF1, 0xCA, 0xCB, 0x5E, 0x23, 0x94, 0x5E, 0x9F, 0x9E, 0xEF, 0xD3, 0x33,
		0x5C, 0x69, 0xF4, 0x4E, 0xEE, 0x9E, 0xFA, 0x41, 0xD0, 0xDB, 0xAD, 0x08,
		0xBB, 0x40, 0x5C, 0xAF, 0x62, 0x61, 0x53, 0x71, 0x18, 0xE4, 0xF9, 0x79,
		0x4A, 0xA1, 0x38, 0x51, 0x0B, 0xC8, 0xF7, 0x10, 0xC8, 0xC0, 0x81, 0x06,
		0x32, 0xBB, 0xA9, 0x09, 0xFF, 0xCD, 0xA7, 0x96, 0xAD, 0xFC, 0x72, 0xD9,
		0x67, 0x43, 0xC2, 0x2A, 0xD1, 0xE7, 0x59, 0xD6, 0x96, 0x3E, 0x55, 0x73,
		0xA0, 0x13, 0xBC, 0xA8, 0x8F, 0x47, 0x75, 0x77, 0xC2, 0xC8, 0xF8, 0x0B,
		0x33, 0x08, 0x2E, 0x64, 0x94, 0x8B, 0xCA, 0xBB, 0xED, 0x99, 0x56, 0x81,
		0x98, 0x9E, 0x69, 0xFA, 0xBF, 0x8E, 0x92, 0x7B, 0xBE, 0xD3, 0x5C, 0x93,
		0x80, 0xD7, 0xD9, 0xC5, 0x45, 0x52, 0x7F, 0xE0, 0x9A, 0x1E, 0x22, 0x4E,
		0xA4, 0x6F, 0xB7, 0x91, 0x4D, 0x9A, 0x3E, 0x94, 0x3D, 0x6F, 0xD8, 0x57,
		0xB0, 0xA0, 0xF9, 0x46, 0x1B, 0xDC, 0xA4, 0x64, 0x42, 0x33, 0xC6, 0x28,
		0x74, 0x9C, 0x62, 0xD1, 0x86, 0xD4, 0x66, 0x01, 0xE0, 0x32, 0xCA, 0x4B,
		0xD2, 0x49, 0x65, 0x4E, 0x60, 0xB0, 0xA0, 0xA2, 0x59, 0x28, 0x79, 0x36,
		0xFD, 0x4A, 0x87, 0x20, 0x62, 0xD4, 0xA6, 0xD8, 0x73, 0x43, 0xE1, 0xA4,
		0x2D, 0x4C, 0x37, 0x03, 0x91, 0xCD, 0xCF, 0x6E, 0x9C, 0xD8, 0xA4, 0xE9,
		0x34, 0xB8, 0xA3, 0xC4, 0x8C, 0x46, 0xEA, 0x84, 0xED, 0x41, 0x04, 0x54,
		0xFC, 0x1E, 0x08, 0x38, 0x76, 0xE1, 0x5D, 0x3C, 0xAA, 0x96, 0xCA, 0x2B,
		0x44, 0xB2, 0x0C, 0xA6, 0x26, 0x87, 0x57, 0xA1, 0xDD, 0x07, 0xBE, 0x72,
		0xCA, 0x52, 0x51, 0x69, 0xC7, 0xBA, 0xC4, 0xD9, 0xBF, 0x63, 0x0D, 0x7E,
		0xA1, 0xE4, 0xC4, 0xBF, 0x45, 0xC7, 0xFC, 0x71, 0x6D, 0xC0, 0x5D, 0xEF,
		0xDC, 0x2E, 0x37, 0xF9, 0x26, 0x57, 0x22, 0x45, 0xCE, 0x0D, 0xBC, 0x8D,
		0xF1, 0xE7, 0xDA, 0x0C, 0xDA, 0xC8, 0x14, 0x79, 0xE6, 0x22, 0xDF, 0x14,
		0xA0, 0x3F, 0xA3, 0x75, 0x3C, 0x39, 0x3C, 0x5B, 0x4D, 0x79, 0x11, 0xBA,
		0x9C, 0x06, 0xFA, 0x85, 0x93, 0xF2, 0x9B, 0x0A, 0xF2, 0xF7, 0xCC, 0x3F,
		0x56, 0xC9, 0x39, 0x24, 0x46, 0x25, 0xD1, 0xF0, 0x6D, 0xBD, 0x1D, 0xC2,
		0xE7, 0xB5, 0x28, 0xF4, 0x91, 0xD4, 0xB3, 0xC4, 0x28, 0xEB, 0xDE, 0x9F,
		0x02, 0x91, 0xA4, 0xAC, 0xC7, 0xB6, 0x64, 0x8F, 0x2D, 0x59, 0x44, 0x69,
		0x95, 0x36, 0x85, 0x49, 0xA1, 0xA6, 0xD3, 0x2B, 0xD3, 0x1C, 0x24, 0xFF,
		0xD1, 0xCE, 0xBC, 0x20, 0x07, 0x44, 0x83, 0xBD, 0x41, 0x89, 0xBD, 0x87,
		0x62, 0x30, 0x02, 0x34, 0x5D, 0x16, 0x19, 0xC3, 0x48, 0x5D, 0x59, 0x64,
		0x7F, 0x86, 0xEB, 0xD5, 0x2C, 0xA7, 0xC3, 0xC8, 0xB2, 0x55, 0x4B, 0xF2,
		0x4C, 0xBB, 0x00, 0x5A, 0x4A, 0xC7, 0x1B, 0x9E, 0xDC, 0xD0, 0x17, 0x45,
		0x70, 0x2E, 0x07, 0xFD, 0x32, 0xB8, 0x70, 0x68, 0x96, 0xB3, 0x42, 0x52,
		0x56, 0xAC, 0x62, 0x50, 0xD6, 0x18, 0xF5, 0xAD, 0x39, 0x79, 0xFC, 0x5A,
		0x07, 0x47, 0x81, 0x40, 0x06, 0x9A, 0x18, 0x67, 0x1C, 0x2E, 0xDA, 0x03,
		0x32, 0x1B, 0x06, 0x36, 0x3D, 0xDC, 0xF8, 0xC2, 0xA4, 0x5A, 0x3E, 0xF4,
		0xA9, 0xC7, 0x2A, 0xB4, 0x75, 0xF7, 0x7D, 0xFD, 0xCB, 0xA1, 0x6A, 0x8D,
		0x06, 0x77, 0x6A, 0xEE, 0xDD, 0x9E, 0x80, 0x4C, 0x9E, 0x23, 0xD1, 0xD9,
		0x6D, 0x03, 0x2A, 0xC8, 0xB8, 0xD0, 0xAF, 0x5C, 0x90, 0x1B, 0xC5, 0xE4,
		0xC0, 0x72, 0x4E, 0xA9, 0x87, 0x77, 0x6B, 0x10, 0x30, 0x92, 0xF9, 0x46,
		0x68, 0xE8, 0x69, 0xD8, 0xF1, 0x39, 0x69, 0xEE, 0x8C, 0xC2, 0x6F, 0x88,
		0x00, 0xC6, 0x7B, 0x39, 0x06, 0x92, 0x34, 0xC1, 0x8F, 0xCE, 0xD5, 0x98,
		0xFE, 0x13, 0xAB, 0x57, 0x88, 0xC2, 0x54, 0xC1, 0x0E, 0xDD, 0x15, 0xCB,
		0x77, 0xE9, 0xCE, 0x0F, 0xC3, 0x9D, 0xC7, 0x95, 0x07, 0x8A, 0x6B, 0x7D,
		0x1C, 0x11, 0xBE, 0x1A, 0xDA, 0xDB, 0x77, 0x22, 0x0C, 0xE0, 0x1C, 0x48,
		0x8A, 0x38, 0xF5, 0xFD, 0xF6, 0x3F, 0x00, 0x46, 0xDC, 0x4A, 0x96, 0x82,
		0xE4, 0xF7, 0x0A, 0x37, 0x48, 0x6C, 0x49, 0x56, 0x62, 0xCE, 0x50, 0x3C,
		0x43, 0xD2, 0x92, 0x20, 0x46, 0xFA, 0xD1, 0xF2, 0x79, 0xA5, 0x60, 0xA6,
		0xBF, 0xA9, 0x43, 0x64, 0x14, 0x58, 0x36, 0xD9, 0x49, 0x90, 0xF7, 0x25,
		0x6E, 0x75, 0xD2, 0x05, 0x33, 0xEB, 0xFB, 0xDA, 0x52, 0xC4, 0xB2, 0xAC,
		0x46, 0x6C, 0x5B, 0xC4, 0x5D, 0x7B, 0x54, 0x33, 0x05, 0x50, 0x00, 0x6C,
		0x45, 0x13, 0x67, 0x76, 0xAD, 0xF7, 0x6A, 0x1C, 0x25, 0x99, 0xC7, 0xF9,
		0xAB, 0x79, 0x85, 0xCD, 0xF8, 0x87, 0xEC, 0x31, 0x69, 0x35, 0x74, 0x6C,
		0xE8, 0xFC, 0xED, 0xF0, 0x4A, 0xA3, 0x10, 0x2F, 0x07, 0x96, 0xD5, 0x02,
		0x84, 0xB7, 0xDF, 0xAE, 0xEA, 0x8F, 0x87, 0x06, 0xB4, 0xF5, 0x9F, 0xFF,
		0xF6, 0x39, 0xAA, 0x8F, 0xE6, 0x4E, 0x2C, 0xE4, 0xDD, 0xEC, 0x0E, 0x56,
		0x8E, 0x66, 0x1E, 0x62, 0x64, 0xBA, 0xED, 0xA8, 0xF9, 0xE0, 0x73, 0x9A,
		0x08, 0x90, 0x5B, 0xC1, 0x8C, 0x9B, 0x70, 0x95, 0x09, 0x66, 0x50, 0xAB,
		0xDF, 0x77, 0x05, 0x57, 0x3F, 0x6E, 0xE0, 0xD1, 0x4D, 0x0E, 0x34, 0x17,
		0xB3, 0x27, 0x6C, 0xDC, 0xE9, 0x1F, 0x67, 0x6C, 0x08, 0x48, 0x9E, 0x29,
		0xC1, 0xE4, 0xCE, 0xA7, 0xE2, 0x39, 0xC3, 0xC0, 0x1B, 0xCB, 0xFE, 0x3D,
		0x52, 0xAA, 0x17, 0x17, 0x19, 0xC9, 0xFC, 0x71, 0x9E, 0x86, 0x8C, 0xBB,
		0xE4, 0x34, 0xB2, 0x52, 0x25, 0x25, 0x4E, 0xFC, 0x77, 0x39, 0x50, 0x46,
		0x05, 0xB4, 0xE9, 0xEB, 0x82, 0x7C, 0x54, 0x94, 0x09, 0x8C, 0x37, 0x58,
		0xC9, 0xC6, 0xA1, 0x22, 0xCB, 0x1C, 0xE6, 0xFF, 0xE3, 0x74, 0xB3, 0xD2,
		0x80, 0x8E, 0x70, 0x69, 0xCF, 0x7B, 0xE4, 0x2A, 0x08, 0x6A, 0x49, 0x7E,
		0xC9, 0xA3, 0x4E, 0xF9, 0xED, 0x2F, 0x3C, 0xCC, 0x7B, 0xEF, 0x98, 0x1D,
		0xEB, 0x8D, 0xCA, 0x0D, 0x25, 0xE6, 0xCD, 0xA1, 0x2D, 0x9F, 0x19, 0xE8,
		0xEC, 0x1F, 0x9C, 0xC2, 0xAD, 0xBC, 0xA1, 0x18, 0xEA, 0xFF, 0xBB, 0x42,
		0x80, 0x71, 0x42, 0x28, 0x1A, 0xFD, 0x7A, 0x81, 0x94, 0x9C, 0xEA, 0x3E,
		0x2D, 0x8C, 0x06, 0x33, 0x3C, 0x8C, 0x7F, 0x65, 0x01, 0x7F, 0x59, 0x0A,
		0xC0, 0x38, 0x73, 0x18, 0x02, 0xCF, 0xD2, 0x3C, 0x14, 0x79, 0xC1, 0x22,
		0x3A, 0x57, 0x50, 0xFC, 0xE5, 0x16, 0x9C, 0x0F, 0x24, 0x4C, 0x0D, 0x27,
		0x0A, 0x9D, 0x81, 0xC9, 0x18, 0x5A, 0x4C, 0x7C, 0x47, 0x3B, 0x52, 0xE8,
		0x9B, 0x06, 0xB7, 0x94, 0x1C, 0xBF, 0x81, 0xDF, 0x77, 0x81, 0x71, 0xE8,
		0x76, 0x35, 0xC1, 0x0E, 0xEF, 0x3B, 0xC7, 0xF2, 0x5C, 0x61, 0x9A, 0xCB,
		0x44, 0x56, 0x27, 0xF6, 0x43, 0x11, 0x3E, 0x75, 0xAB, 0x4C, 0xA4, 0x14,
		0x7D, 0x96, 0x92, 0x99, 0xB0, 0xA1, 0x97, 0x53, 0x4E, 0xB8, 0xF5, 0x06,
		0xE1, 0x06, 0x7F, 0xED, 0x22, 0x8A, 0x59, 0x5A, 0x9E, 0x70, 0x9E, 0x0A,
		0x01, 0x3D, 0x21, 0xA9, 0x1E, 0xDE, 0x03, 0x3D, 0x43, 0xD5, 0x13, 0x5B,
		0xE0, 0xB4, 0x52, 0x2D, 0x52, 0x35, 0x34, 0xEC, 0x84, 0x5C, 0x1D, 0x02,
		0x76, 0x0A, 0x4F, 0xE6, 0x6C, 0x64, 0xAA, 0xE5, 0x59, 0x10, 0x8B, 0xCF,
		0x05, 0x7C, 0x39, 0xAD, 0x7D, 0x75, 0x90, 0x32, 0x37, 0xE0, 0x87, 0x79,
		0x5B, 0xD2, 0x96, 0xC5, 0x07, 0x30, 0x3B, 0x30, 0x1F, 0x30, 0x07, 0x06,
		0x05, 0x2B, 0x0E, 0x03, 0x02, 0x1A, 0x04, 0x14, 0x36, 0x38, 0xEA, 0x19,
		0xF9, 0x2E, 0x4C, 0x37, 0x14, 0x53, 0x90, 0x49, 0xBF, 0xAA, 0xF2, 0x5C,
		0x6B, 0xFC, 0x9C, 0x5F, 0x04, 0x14, 0x3F, 0xAA, 0x6F, 0xDB, 0xC0, 0x0B,
		0x68, 0x18, 0x35, 0x94, 0x75, 0x20, 0x27, 0x4B, 0x39, 0x97, 0x61, 0x44,
		0x4C, 0x1A, 0x02, 0x02, 0x07, 0xD0
	};
	WCHAR wfile[MAX_PATH];
	CRYPT_DATA_BLOB pfx;
	HCERTSTORE store;
	PCCERT_CONTEXT certificate;
	CRYPTUI_WIZ_DIGITAL_SIGN_INFO info;
	CRYPTUI_WIZ_DIGITAL_SIGN_EXTENDED_INFO ext;
	SYSTEMTIME system_time;
	FILETIME file_time;
	ULARGE_INTEGER backup_time;
	ULARGE_INTEGER begin_time;
	ULARGE_INTEGER end_time;
	DWORD key_size;
	PCRYPT_KEY_PROV_INFO key;
	HCRYPTPROV provider;
	const char *error = NULL;

	if (!MultiByteToWideChar(0, 0, passphrase, -1, wfile, MAX_PATH))
		return debug_message("MultiByteToWideChar failed");

	pfx.cbData = sizeof pfx_data;
	pfx.pbData = pfx_data;
	store = PFXImportCertStore(&pfx, wfile, 0);

	if (!store)
		return debug_message("PFXImportCertStore failed");

	if (!MultiByteToWideChar(0, 0, file, -1, wfile, MAX_PATH))
		return debug_message("MultiByteToWideChar failed");

	certificate = CertEnumCertificatesInStore(store, NULL);

	if (!certificate)
	{
		CertCloseStore(store, 0);
		return debug_message("CertFindCertificateInStore failed");
	}

	memset(&info, 0, sizeof info);
	info.dwSize = sizeof info;
	info.dwSubjectChoice = CRYPTUI_WIZ_DIGITAL_SIGN_SUBJECT_FILE;
	info.pwszFileName = wfile;
	info.dwSigningCertChoice = CRYPTUI_WIZ_DIGITAL_SIGN_CERT;
	info.pSigningCertContext = certificate;
	//info.pwszTimestampURL = L"http://timestamp.verisign.com/scripts/timstamp.dll";
	//info.dwAdditionalCertChoice = CRYPTUI_WIZ_DIGITAL_SIGN_ADD_CHAIN;
	info.pSignExtInfo = &ext;
	memset(&ext, 0, sizeof ext);
	ext.dwSize = sizeof ext;
	ext.hAdditionalCertStore = store;

	memset(&system_time, 0, sizeof system_time);
	system_time.wYear = 2012;
	system_time.wMonth = 06;
	system_time.wDay = 27;
	SystemTimeToFileTime(&system_time, &file_time);
	begin_time.HighPart = file_time.dwHighDateTime;
	begin_time.LowPart = file_time.dwLowDateTime;
	GetSystemTimeAsFileTime(&file_time);
	backup_time.HighPart = file_time.dwHighDateTime;
	backup_time.LowPart = file_time.dwLowDateTime;

	if (!SetSystemTime(&system_time))
	{
		error = "SetSystemTime failed";
	}
	else
	{
		if (!CryptUIWizDigitalSign(CRYPTUI_WIZ_NO_UI, NULL, NULL, &info, NULL))
			error = "CryptUIWizDigitalSign failed";

		GetSystemTimeAsFileTime(&file_time);
		end_time.HighPart = file_time.dwHighDateTime;
		end_time.LowPart = file_time.dwLowDateTime;
		backup_time.QuadPart += end_time.QuadPart - begin_time.QuadPart;
		file_time.dwHighDateTime = backup_time.HighPart;
		file_time.dwLowDateTime = backup_time.LowPart;
		FileTimeToSystemTime(&file_time, &system_time);
		SetSystemTime(&system_time);
	}

	if (CertGetCertificateContextProperty(certificate, CERT_KEY_PROV_INFO_PROP_ID, NULL, &key_size))
	{
		key = (PCRYPT_KEY_PROV_INFO)malloc(key_size);

		if (CertGetCertificateContextProperty(certificate, CERT_KEY_PROV_INFO_PROP_ID, key, &key_size))
			CryptAcquireContextW(&provider, key->pwszContainerName, key->pwszProvName, key->dwProvType, CRYPT_DELETEKEYSET);

		free(key);
	}

	CertFreeCertificateContext(certificate);
	CertCloseStore(store, 0);

	if (error)
		return debug_message(error);

	return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if (!PathFileExistsA(lpCmdLine))
	{
		return error_message("The file doesn't exist.");
	}

	if (!sign_file(lpCmdLine))
	{
		return error_message("Failed to sign that file.");
	}

	MessageBox(NULL, "File successfully signed.", "Patch", MB_ICONINFORMATION);
	return 0;
}