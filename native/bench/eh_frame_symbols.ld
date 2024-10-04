__eh_frame_hdr_start = SIZEOF(.eh_frame_hdr) > 0 ? ADDR(.eh_frame_hdr) : 0;
__eh_frame_hdr_end = SIZEOF(.eh_frame_hdr) > 0 ? (ADDR(.eh_frame_hdr) + SIZEOF(.eh_frame_hdr)) : 0;
__eh_frame_start = SIZEOF(.eh_frame) > 0 ? ADDR(.eh_frame) : 0;
__eh_frame_end = SIZEOF(.eh_frame) > 0 ? (ADDR(.eh_frame) + SIZEOF(.eh_frame)) : 0;

__bss_start = SIZEOF(.bss) > 0 ? ADDR(.bss) : 0;
__bss_end = SIZEOF(.bss) > 0 ? (ADDR(.bss) + SIZEOF(.bss)) : 0;
__tbss_start = SIZEOF(.tbss) > 0 ? ADDR(.tbss) : 0;
__tbss_end = SIZEOF(.tbss) > 0 ? (ADDR(.tbss) + SIZEOF(.tbss)) : 0;