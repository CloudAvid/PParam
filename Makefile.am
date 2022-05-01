ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src examples
EXTRA_DIST = autogen.sh

pkgconfigdir= $(libdir)/pkgconfig
pkgconfig_DATA= $(PACKAGE).pc

RPM_TOPDIR=`rpm --showrc | perl -n -e 'print if(s/.*_topdir\s+(.*)/$$1/)'`

rpm:
	$(MAKE) distdir ; 
	mv $(PACKAGE)-$(VERSION) $(PACKAGE_TARNAME) ; 
	tar czvf $(PACKAGE_TARNAME).tgz $(PACKAGE_TARNAME) ; 
	$(RM) -rf $(PACKAGE_TARNAME) ; 
	cp $(PACKAGE).spec $(RPM_TOPDIR)/SPECS/ ; 
	cp $(PACKAGE_TARNAME).tgz $(RPM_TOPDIR)/SOURCES/ ; 
	$(RM) -f $(PACKAGE_TARNAME).tgz ; 
	rpmbuild -ba --clean $(RPM_TOPDIR)/SPECS/$(PACKAGE).spec ; 
	mkdir -p $(RPM_TOPDIR)/RPMS/x86_64/$(PACKAGE_TARNAME) ; 
	mv $(RPM_TOPDIR)/RPMS/x86_64/$(PACKAGE_RPMNAME)-*$(PACKAGE_RPMVERSION)-*.rpm \
		$(RPM_TOPDIR)/RPMS/x86_64/$(PACKAGE_TARNAME) ;
	expect -c " spawn  bash -c \"rpm --resign $(RPM_TOPDIR)/RPMS/x86_64/$(PACKAGE_TARNAME)/*\" ;  expect \"phrase: \"; send \"$(RPM_PASSPHRASE)\r\";  expect eof";

install_rpm:
	sudo rpm -iv --force --nodeps $(RPM_TOPDIR)/RPMS/x86_64/$(PACKAGE_TARNAME)/*.rpm

uninstall_rpm:
	sudo rpm -e `rpm -qa | grep $(PACKAGE_RPMNAME)`
