
all: echoit

echoit:
	@echo "GenPed Web Interface Package V2.0 (c)1998 Christophe Person"
	@echo "Use only make clean-all, make build-all or make install-all"

clean-all:
	@echo "-----------------------------------------------------------"
	@echo "GenPed Web Interface Package V2.0 (c)1998 Christophe Person"
	@echo "-----------------------------------------------------------"
	@(cd libs/gpconfig/current; make -s clean)
	@(cd libs/libped/current; make -s clean)
	@(cd libs/gphtml/current; make -s clean)
	@(cd www/gpadmin/current; make -s clean)

build-libs:
	@(cd libs/gpconfig/current; make -s)
	@(cd libs/libped/current; make -s)
	@(cd libs/gphtml/current; make -s)

build-all: build-libs
	@echo "-----------------------------------------------------------"
	@echo "GenPed Web Interface Package V2.0 (c)1998 Christophe Person"
	@echo "-----------------------------------------------------------"
	@(cd www/gpquery/current; make -s)

install-all: build-libs
	@echo "-----------------------------------------------------------"
	@echo "GenPed Web Interface Package V2.0 (c)1998 Christophe Person"
	@echo "-----------------------------------------------------------"
	@(cd www/gpquery/current; make -s install)
