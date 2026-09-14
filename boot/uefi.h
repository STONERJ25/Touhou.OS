/*
 * Minimal, hand-written UEFI type definitions -- just enough to write to
 * the console via EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL. This is NOT the full
 * UEFI spec: unused function-pointer slots are declared as `void *` so the
 * struct layout (size/order) still matches what real firmware expects,
 * without us having to fully type every service we don't call yet.
 */

typedef unsigned char      UINT8;
typedef unsigned short     UINT16;
typedef unsigned int       UINT32;
typedef unsigned long long UINT64;
typedef UINT64             UINTN;   /* native register width: 64-bit on x86-64 */
typedef unsigned short     CHAR16;  /* UEFI strings are UCS-2 / UTF-16, not ASCII */

typedef UINTN  EFI_STATUS;
typedef void  *EFI_HANDLE;

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
    void                             *RuntimeServices;
    void                             *BootServices;
    UINTN                             NumberOfTableEntries;
    void                             *ConfigurationTable;
} EFI_SYSTEM_TABLE;
