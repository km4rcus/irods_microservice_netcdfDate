GCC = g++

INC=-I/usr/include/irods/ -I/usr/include/irods/boost/

all: libmsiCalcNcDates

libmsiCalcNcDates:
	${GCC} ${INC} -DRODS_SERVER -fPIC "-Wl,-E" -shared -Wno-deprecated -rdynamic -Ofast -march=native -o $@.so $@.cpp

install:
	cp libmsiCalcNcDates.so /var/lib/irods/plugins/microservices/
clean:
	@rm -f ./*.so
