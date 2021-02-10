HWCONFIG := ringtone-player
GDB_UART_SWAP=1
ARDUINO_LIBRARIES := \
	ToneGenerator \
	RingTone \
	ArduinoJson6
	
COMPONENT_DEPENDS := \
	malloc_count

ifeq ($(SMING_ARCH),Host)
COMPONENT_SRCDIRS += host
endif

clean: ringtoneplayer-clean
.PHONY: ringtoneplayer-clean
ringtoneplayer-clean:
	rm out/fwfs.bin
