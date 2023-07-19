VERSION:=6.12.2
exe:
	cd patchelf && ./bootstrap.sh && ./configure && make

dll:
	cd $(VERSION) && gcc -fPIC -shared ../subhook/subhook.c  src/hook.c -ludev -ldl -o ./src/hook.so

exe_code: exe
	cd $(VERSION) && mkdir -p code && xxd -i ../patchelf/src/patchelf ./code/patchelf.h

dll_code: dll
	cd $(VERSION) && mkdir -p code && xxd -i ./src/hook.so ./code/hook.h

unraider:
	make exe_code
	make dll_code
	cd $(VERSION) && g++ ./src/unraider.cc -o ./unraider

all:
	make unraider

clean:
	git clean -dfX
