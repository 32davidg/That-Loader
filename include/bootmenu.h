#pragma once
#include <uefi.h>

// while there is no actual defualt, I just eyeballed it
#define DEFAULT_CONSOLE_COLUMNS (80)
#define DEFAULT_CONSOLE_ROWS    (30)

#define DEFAULT_TIMEOUT_SECONDS (10);

typedef struct boot_menu_cfg_s
{
    int32_t maxEntriesOnScreen;
    int32_t selectedEntryIndex; // the desired entry
    int32_t entryOffset; // the current entry the program is pointing at

    int32_t timeoutSeconds;
    boolean_t timeoutCancelled;
    boolean_t bootImmediately;
} boot_menu_cfg_s;


extern boot_menu_cfg_s bmcfg;

void StartBootManager(void);

void ShowLogFile(void);



// GRAPHICS
typedef uintn_t              EFI_PHYSICAL_ADDRESS;

// UEFI 2.9 Specs PDF Page 491
typedef enum EFI_GRAPHICS_OUTPUT_BLT_OPERATION
{
    EfiBltVideoFill,
    EfiBltVideoToBltBuffer,
    EfiBltBufferToVideo,
    EfiBltVideoToVideo,
    EfiGraphicsOutputBltOperationMax
} EFI_GRAPHICS_OUTPUT_BLT_OPERATION;

// UEFI 2.9 Specs PDF Page 491
typedef struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL
{
    uint8_t   Blue;
    uint8_t   Green;
    uint8_t   Red;
    uint8_t   Reserved;
} EFI_GRAPHICS_OUTPUT_BLT_PIXEL;

// UEFI 2.9 Specs PDF Page 486
typedef enum EFI_GRAPHICS_PIXEL_FORMAT
{
    PixelRedGreenBlueReserved8BitPerColor,
    PixelBlueGreenRedReserved8BitPerColor,
    PixelBitMask,
    PixelBltOnly,
    PixelFormatMax
} EFI_GRAPHICS_PIXEL_FORMAT;

// UEFI 2.9 Specs PDF Page 485
typedef struct EFI_PIXEL_BITMASK
{
    uint32_t    RedMask;
    uint32_t    GreenMask;
    uint32_t    BlueMask;
    uint32_t    ReservedMask;
} EFI_PIXEL_BITMASK;

// UEFI 2.9 Specs PDF Page 486
typedef struct EFI_GRAPHICS_OUTPUT_MODE_INFORMATION
{
    uint32_t                      Version;
    uint32_t                      HorizontalResolution;
    uint32_t                      VerticalResolution;
    EFI_GRAPHICS_PIXEL_FORMAT   PixelFormat;
    EFI_PIXEL_BITMASK           PixelInformation;
    uint32_t                      PixelsPerScanLine;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

// UEFI 2.9 Specs PDF Page 488
typedef struct EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE
{
    uint32_t                                MaxMode;
    uint32_t                                Mode;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  *Info;
    uintn_t                                 SizeOfInfo;
    EFI_PHYSICAL_ADDRESS                  FrameBufferBase;
    uintn_t                                 FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

typedef EFI_STATUS (*EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE)(struct EFI_GRAPHICS_OUTPUT_PROTOCOL *This, uint32_t ModeNumber, uintn_t *SizeOfInfo, EFI_GRAPHICS_OUTPUT_MODE_INFORMATION **Info);
typedef EFI_STATUS (*EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE)(struct EFI_GRAPHICS_OUTPUT_PROTOCOL *This, uint32_t ModeNumber);
typedef EFI_STATUS (*EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT)(struct EFI_GRAPHICS_OUTPUT_PROTOCOL *This, EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer, EFI_GRAPHICS_OUTPUT_BLT_OPERATION BltOperation, uintn_t SourceX, uintn_t SourceY, uintn_t DestinationX, uintn_t DestinationY, uintn_t Width, uintn_t Height, uintn_t Delta);

// UEFI 2.9 Specs PDF Page 485
typedef struct EFI_GRAPHICS_OUTPUT_PROTOCOL
{
    EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE  QueryMode;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE    SetMode;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT         Blt;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE        *Mode;
} EFI_GRAPHICS_OUTPUT_PROTOCOL;


