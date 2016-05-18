GCC = g++

INC=-I/usr/include/irods/ -I/usr/include/irods/boost/

all: libmsiCalcNcDates

libmsiCalcNcDates.so: libmsiCalcNcDates.cpp
	${GCC} ${INC} -DRODS_SERVER -fPIC "-Wl,-E" -shared -Wno-deprecated -rdynamic -Ofast -march=native -o $@ $<

install: libmsiCalcNcDates.so
	cp libmsiCalcNcDates.so /var/lib/irods/plugins/microservices/
clean:
	@rm -f ./*.so
