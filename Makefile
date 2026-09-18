CC       = gcc
SRCDIR   = lib
BUILDDIR = build
LIBNAME  = libsem.a

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(SRCS:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

defs    = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
default = -std=c99 -pedantic -g -Wall $(defs)
verbose = -std=c99 -pedantic -g3 -Wall -Wextra -Wwrite-strings -Wconversion -Wformat=2 -Warray-bounds -Wstack-protector -Wshadow $(defs)

# compile-time hardening (applies to the .o files that go into the static lib)
harden_cc = -fstack-protector-strong -fstack-clash-protection -D_FORTIFY_SOURCE=2 \
            -fPIC -Wformat -Wformat-security -Werror=format-security

# link-time hardening (only meaningful when linking a final executable, not for `ar`.
# Pass these when you build your actual program against libsem.a)
harden_ld = -pie -Wl,-z,relro,-z,now -Wl,-z,noexecstack

asan = -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer -fno-common

library = -lpthread -pthread -lrt

# pick the base flag set with: make MODE=verbose ...
MODE   ?= default
CFLAGS  = $($(MODE))

.PHONY: all verbose asan harden clean

all: $(BUILDDIR)/$(LIBNAME)

verbose:
	$(MAKE) MODE=verbose all

asan: CFLAGS += $(asan)
asan: clean $(BUILDDIR)/$(LIBNAME)

harden: CFLAGS += $(harden_cc)
harden: clean $(BUILDDIR)/$(LIBNAME)
	@echo "Static lib built with compile-time hardening."
	@echo "When linking your executable against $(BUILDDIR)/$(LIBNAME), also add:"
	@echo "  $(harden_ld) $(library)"

$(BUILDDIR)/$(LIBNAME): $(OBJS)
	ar rcs $@ $^

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)
