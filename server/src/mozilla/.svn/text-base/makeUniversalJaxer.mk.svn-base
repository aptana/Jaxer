
include $(MOZ_OBJDIR)/i386/config/autoconf.mk
postflight_all:

	mkdir -p $(MOZ_OBJDIR)/universal/stage/i386
	mkdir -p $(MOZ_OBJDIR)/universal/stage/ppc
	mkdir -p $(MOZ_OBJDIR)/universal/final

	rsync -vrL $(MOZ_OBJDIR)/i386/dist/bin/* $(MOZ_OBJDIR)/universal/stage/i386/
	rsync -vrL $(MOZ_OBJDIR)/ppc/dist/bin/* $(MOZ_OBJDIR)/universal/stage/ppc/

#	$(TOPSRCDIR)/build/macosx/universal/fix-buildconfig $(MOZ_OBJDIR)/universal/stage/ppc/chrome/toolkit.jar $(MOZ_OBJDIR)/universal/stage/i386/chrome/toolkit.jar

	rm -f $(MOZ_OBJDIR)/universal/stage/ppc/nsinstall
	rm -f $(MOZ_OBJDIR)/universal/stage/i386/nsinstall

	rm -f $(MOZ_OBJDIR)/universal/stage/i386/mangle
	rm -f $(MOZ_OBJDIR)/universal/stage/i386/shlibsign
	rm -f $(MOZ_OBJDIR)/universal/stage/i386/xpidl

	rm -f $(MOZ_OBJDIR)/universal/stage/i386/*.chk
	rm -f $(MOZ_OBJDIR)/universal/stage/ppc/*.chk

	rm -rf $(MOZ_OBJDIR)/universal/final

	$(TOPSRCDIR)/build/macosx/universal/unify $(MOZ_OBJDIR)/universal/stage/ppc $(MOZ_OBJDIR)/universal/stage/i386 $(MOZ_OBJDIR)/universal/final

	mkdir -p $(MOZ_OBJDIR)/dist

	ln -f -s $(MOZ_OBJDIR)/universal/final $(MOZ_OBJDIR)/dist/bin


	echo "cd $(TOPSRCDIR)/aptana; $(TOPSRCDIR)/aptana/postBuild.sh $(MOZ_OBJDIR) $(OS_ARCH)"
	cd $(TOPSRCDIR)/aptana; $(TOPSRCDIR)/aptana/postBuild.sh $(MOZ_OBJDIR) $(OS_ARCH)

# This is a liitle hacky.  We are using the rules defined under i386
# to generate the symbols.
# Need to examine the stripping below.
ifdef MOZ_CRASHREPORTER
	$(MAKE) -C $(MOZ_OBJDIR)/i386 buildsymbols BUILDID=$(JAXER_BUILDID)
ifndef ENABLE_STRIP
	@echo "Stripping resulting files..."
	@cd $(MOZ_OBJDIR)/dist/bin; find . ! -type d \
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

