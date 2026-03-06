global header_start
section .multiboot_header
align 8
header_start:
    ; multiboot2 magic
    dd 0xe85250d6
    ; architecture (0 = i386)
    dd 0
    ; header length (bytes from header_start to header_end)
    dd header_end - header_start
    ; checksum: (magic + arch + header_length + checksum) == 0 (mod 2^32)
    dd -(0xe85250d6 + 0 + (header_end - header_start))

    ; end tag: type 0, size 8
    dd 0
    dd 8
header_end: