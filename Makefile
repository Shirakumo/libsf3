CMAKE ?= cmake
PREFIX ?= build
BUILD_TYPE ?= ReleaseWithDebug
CMAKEFLAGS ?= -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

build:
	mkdir -p $(PREFIX)
	$(CMAKE) . -B $(PREFIX) $(CMAKEFLAGS)
	$(MAKE) -C $(PREFIX) $(TARGET)

docs:
	mkdir -p $(PREFIX)
	$(CMAKE) . -B $(PREFIX) $(CMAKEFLAGS) -DBUILD_DOCS=ON
	$(MAKE) -C $(PREFIX) docs

install:
	$(MAKE) native
	$(MAKE) -C $(PREFIX) install
