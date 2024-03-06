VERSION:=6.12.8
LD_LIBRARY_PATH=/usr/lib/
ifeq ($(VERSION),6.12.8)
override LD_LIBRARY_PATH=/tmp/openssl/lib
endif

.PHONY: all openssl clean

.DEFAULT_GOAL := all

exe:
	cd patchelf && ./bootstrap.sh && ./configure && make

openssl:
	cd openssl &&  ./config --prefix=/tmp/openssl && ./config -t && make install

dll:
	cd $(VERSION) && gcc -fPIC -shared ../subhook/subhook.c  src/hook.c -ludev -ldl -o ./src/hook.so

exe_code: exe
	cd $(VERSION) && mkdir -p code && xxd -i ../patchelf/src/patchelf ./code/patchelf.h

dll_code: dll
	cd $(VERSION) && mkdir -p code && xxd -i ./src/hook.so ./code/hook.h

unraider:
	@echo $(LD_LIBRARY_PATH)
	cd $(VERSION) && g++ -L$(LD_LIBRARY_PATH) ./src/unraider.cc -o ./unraider -lcrypto -ludev

debug:
	cd $(VERSION) && g++ -g -L$(LD_LIBRARY_PATH) ./src/unraider.cc -o ./unraider -lcrypto -ludev


all:
ifeq ($(VERSION),6.12.8)
	@echo "Building openssl"
	make openssl

	@echo "Building unraider"
	make unraider
else
	@echo "Building patchelf"
	make exe_code
	make dll_code

	@echo "Building unraider"
	make unraider
endif


clean:
	git clean -dfX
