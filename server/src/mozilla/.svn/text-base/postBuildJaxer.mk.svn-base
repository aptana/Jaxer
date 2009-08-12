
ifndef OBJDIR
OBJDIR = $(MOZ_OBJDIR)
endif

include $(OBJDIR)/config/autoconf.mk

postflight:

	@echo "cd $(TOPSRCDIR)/aptana; $(TOPSRCDIR)/aptana/postBuild.sh $(OBJDIR) $(OS_ARCH)"
	@cd $(TOPSRCDIR)/aptana; $(TOPSRCDIR)/aptana/postBuild.sh $(OBJDIR) $(OS_ARCH)

ifdef MOZ_CRASHREPORTER
	$(MAKE) -C $(OBJDIR) buildsymbols BUILDID=$(JAXER_BUILDID)
ifndef ENABLE_STRIP
	@echo "Stripping resulting files..."
	@cd $(OBJDIR)/dist/bin; find . ! -type d \
			! -name "*.js" \
			! -name "*.xpt" \
			! -name "*.gif" \
			! -name "*.jpg" \
			! -name "*.png" \
			! -name "*.xpm" \
			! -name "*.txt" \
			! -name "*.rdf" \
			! -name "*.sh" \
			! -name "*.properties" \
			! -name "*.dtd" \
			! -name "*.html" \
			! -name "*.xul" \
			! -name "*.css" \
			! -name "*.xml" \
			! -name "*.jar" \
			! -name "*.dat" \
			! -name "*.tbl" \
			! -name "*.src" \
			! -name "*.reg" \
			$(PLATFORM_EXCLUDE_LIST) \
			-exec $(STRIP) $(STRIP_FLAGS) {} >/dev/null 2>&1 \;
endif
endif
