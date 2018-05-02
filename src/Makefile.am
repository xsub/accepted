bin_PROGRAMS = accepted
accepted_SOURCES = main.c

install-exec-hook:
	ln $(DESTDIR)$(bindir)/accepted$(EXEEXT) $(DESTDIR)$(bindir)/rejected$(EXEEXT)

uninstall-hook:
	rm -f $(DESTDIR)$(bindir)/rejected$(EXEEXT)
