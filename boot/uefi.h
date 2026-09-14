/*
 * Minimal, hand-written UEFI type definitions -- only as much of the real
 * UEFI spec as this project actually calls (console output, GOP graphics,
 * memory map / ExitBootServices). Unused function-pointer slots are
 * declared as `void *` so struct layout (size/order) still matches what
 * real firmware expects, without fully typing every service we don't use.
 */

#ifndef TOUHOU_UEFI_H
#define TOUHOU_UEFI_H

typedef unsigned char      UINT8;
typedef unsigned short     UINT16;
typedef unsigned int       UINT32;
typedef int                INT32;   /* signed -- needed for coordinate math that can go negative */
typedef unsigned long long UINT64;
typedef UINT64             UINTN;   /* native register width: 64-bit on x86-64 */
typedef unsigned short     CHAR16;  /* UEFI strings are UCS-2 / UTF-16, not ASCII */

typedef UINTN  EFI_STATUS;
typedef void  *EFI_HANDLE;

typedef struct {
    UINT32 Data1;
    UINT16 Data2;
    UINT16 Data3;
    UINT8  Data4[8];
} EFI_GUID;

#define EFIAPI __attribute__((ms_abi)) /* UEFI mandates the Microsoft x64 calling convention */
#define EFI_SUCCESS 0

typedef struct {
    UINT64 Signature;
    UINT32 Revision;
    UINT32 HeaderSize;
    UINT32 CRC32;
    UINT32 Reserved;
} EFI_TABLE_HEADER;

typedef struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

typedef EFI_STATUS (EFIAPI *EFI_TEXT_STRING)(
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    CHAR16 *String
);

struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
    void            *Reset;         /* unused -- present only to keep field order/size correct */
    EFI_TEXT_STRING  OutputString;  /* the only service we actually call */
    void            *TestString;
    void            *QueryMode;
    void            *SetMode;
    void            *SetAttribute;
    void            *ClearScreen;
    void            *SetCursorPosition;
    void            *EnableCursor;
    void            *Mode;
};

typedef EFI_STATUS (EFIAPI *EFI_LOCATE_PROTOCOL)(
    EFI_GUID *Protocol,
    void     *Registration,
    void    **Interface
);

typedef UINT64 EFI_PHYSICAL_ADDRESS;
typedef UINT64 EFI_VIRTUAL_ADDRESS;

/*
 * One entry in a UEFI memory map. IMPORTANT: the real stride between
 * consecutive entries in a memory map buffer is NOT sizeof(this struct) --
 * the spec explicitly allows descriptors to grow in future revisions, so
 * code must always use the DescriptorSize value GetMemoryMap itself
 * returns to step between entries.
 */
typedef struct {
    UINT32               Type;
    EFI_PHYSICAL_ADDRESS PhysicalStart;
    EFI_VIRTUAL_ADDRESS  VirtualStart;
    UINT64               NumberOfPages;
    UINT64               Attribute;
} EFI_MEMORY_DESCRIPTOR;

typedef EFI_STATUS (EFIAPI *EFI_GET_MEMORY_MAP)(
    UINTN                  *MemoryMapSize,
    EFI_MEMORY_DESCRIPTOR  *MemoryMap,
    UINTN                  *MapKey,
    UINTN                  *DescriptorSize,
    UINT32                 *DescriptorVersion
);

typedef EFI_STATUS (EFIAPI *EFI_EXIT_BOOT_SERVICES)(
    EFI_HANDLE ImageHandle,
    UINTN      MapKey
);

typedef struct {
    EFI_TABLE_HEADER Hdr;

    /*
     * Everything below is declared only to preserve size/order -- grouped
     * into arrays matching the UEFI spec's own named subsections, so the
     * count in each group can be checked against the spec directly instead
     * of hoping 37 anonymous fields were counted correctly.
     */
    void *_taskPriorityServices[2];       /* RaiseTPL, RestoreTPL */
    void *_memoryServices1[2];            /* AllocatePages, FreePages */
    EFI_GET_MEMORY_MAP GetMemoryMap;
    void *_memoryServices2[2];            /* AllocatePool, FreePool */
    void *_eventTimerServices[6];         /* CreateEvent, SetTimer, WaitForEvent, SignalEvent, CloseEvent, CheckEvent */
    void *_protocolHandlerServices[9];    /* Install/Reinstall/UninstallProtocolInterface, HandleProtocol, Reserved,
                                            * RegisterProtocolNotify, LocateHandle, LocateDevicePath, InstallConfigurationTable */
    void *_imageServices[4];              /* LoadImage, StartImage, Exit, UnloadImage */
    EFI_EXIT_BOOT_SERVICES ExitBootServices;
    void *_miscServices[3];               /* GetNextMonotonicCount, Stall, SetWatchdogTimer */
    void *_driverSupportServices[2];      /* ConnectController, DisconnectController */
    void *_openCloseProtocolServices[3];  /* OpenProtocol, CloseProtocol, OpenProtocolInformation */
    void *_libraryServices[2];            /* ProtocolsPerHandle, LocateHandleBuffer */

    EFI_LOCATE_PROTOCOL LocateProtocol;   /* still the only "discovery" service we call */

    /* InstallMultipleProtocolInterfaces onward: not declared, we never reach past LocateProtocol */
} EFI_BOOT_SERVICES;

#define EfiResetCold             0
#define EfiResetWarm             1
#define EfiResetShutdown         2
#define EfiResetPlatformSpecific 3

typedef void (EFIAPI *EFI_RESET_SYSTEM)(
    UINT32     ResetType,
    EFI_STATUS ResetStatus,
    UINTN      DataSize,
    void      *ResetData
);

typedef struct {
    EFI_TABLE_HEADER Hdr;

    /* Same "preserve size/order only" technique as EFI_BOOT_SERVICES,
     * grouped to match the spec's own named subsections -- just a much
     * shorter table this time (10 fields to skip instead of 37). */
    void *_timeServices[4];           /* GetTime, SetTime, GetWakeupTime, SetWakeupTime */
    void *_virtualMemoryServices[2];  /* SetVirtualAddressMap, ConvertPointer */
    void *_variableServices[3];       /* GetVariable, GetNextVariableName, SetVariable */
    void *_miscServices[1];           /* GetNextHighMonotonicCount */

    EFI_RESET_SYSTEM ResetSystem;     /* the one service we actually call */

    /* UpdateCapsule onward: not declared, we never reach past ResetSystem */
} EFI_RUNTIME_SERVICES;

typedef struct {
    EFI_TABLE_HEADER                 Hdr;
    CHAR16                           *FirmwareVendor;
    UINT32                            FirmwareRevision;
    EFI_HANDLE                        ConsoleInHandle;
    void                             *ConIn;
    EFI_HANDLE                        ConsoleOutHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *ConOut;
    EFI_HANDLE                        StandardErrorHandle;
    void                             *StdErr;
    EFI_RUNTIME_SERVICES             *RuntimeServices;
    EFI_BOOT_SERVICES                *BootServices;
    UINTN                             NumberOfTableEntries;
    void                             *ConfigurationTable;
} EFI_SYSTEM_TABLE;

#define PixelRedGreenBlueReserved8BitPerColor  0
#define PixelBlueGreenRedReserved8BitPerColor  1
#define PixelBitMask                           2
#define PixelBltOnly                           3

typedef struct {
    UINT32 RedMask;
    UINT32 GreenMask;
    UINT32 BlueMask;
    UINT32 ReservedMask;
} EFI_PIXEL_BITMASK;

typedef struct {
    UINT32             Version;
    UINT32             HorizontalResolution;
    UINT32             VerticalResolution;
    UINT32             PixelFormat;       /* one of the Pixel... values above -- UINT32, not a C enum, since enum size isn't guaranteed by the C standard */
    EFI_PIXEL_BITMASK  PixelInformation;  /* only meaningful when PixelFormat == PixelBitMask */
    UINT32             PixelsPerScanLine; /* real row stride in pixels -- can be larger than HorizontalResolution */
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

typedef struct {
    UINT32                                 MaxMode;
    UINT32                                 Mode;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  *Info;
    UINTN                                  SizeOfInfo;
    EFI_PHYSICAL_ADDRESS                   FrameBufferBase;  /* the address we actually want */
    UINTN                                  FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

typedef struct {
    void                                *QueryMode;  /* unused -- we accept firmware's current mode as-is */
    void                                *SetMode;    /* unused, for the same reason */
    void                                *Blt;        /* unused -- we write the framebuffer directly instead */
    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE   *Mode;
} EFI_GRAPHICS_OUTPUT_PROTOCOL;

#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID \
    { 0x9042a9de, 0x23dc, 0x4a38, { 0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a } }

#endif /* TOUHOU_UEFI_H */
