exe:
	cd patchelf && ./bootstrap.sh && ./configure && make

dll:
	gcc -fPIC -shared src/hook.c -ludev -ldl -o ./src/hook.so

exe_code: exe
	mkdir -p code && xxd -i ./patchelf/src/patchelf ./code/patchelf.h

dll_code: dll
	mkdir -p code && xxd -i ./src/hook.so ./code/hook.h

unraider:
	make exe_code
	make dll_code
	g++ ./src/unraider.cc -o ./unraider

all:
	make unraider

clean:
	git clean -dfX
