#
# spec file for package mingw32-pw3270
#
# Copyright (c) 2014 SUSE LINUX Products GmbH, Nuernberg, Germany.
# Copyright (C) <2008> <Banco do Brasil S.A.>
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

%define MAJOR_VERSION 5
%define MINOR_VERSION 1

%define __strip %{_mingw32_strip}
%define __objdump %{_mingw32_objdump}
%define _use_internal_dependency_generator 0
%define __find_requires %{_mingw32_findrequires}
%define __find_provides %{_mingw32_findprovides}
%define __os_install_post %{_mingw32_debug_install_post} \
                          %{_mingw32_install_post}

#---[ Packaging ]-----------------------------------------------------------------------------------------------------

Name:           mingw32-pw3270
Version:        5.1
Release:        0
Summary:        IBM 3270 Terminal emulator for GTK
License:        GPL-2.0
Group:          System/X11/Terminals
Url:            http://www.softwarepublico.gov.br/dotlrn/clubs/pw3270
Source:         pw3270-%{version}.tar.bz2
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

Requires:		mingw32-gtk3
Requires:		mingw32-lib3270 = %{version}

Provides:		mingw32(lib:pw3270) = %{version}
Requires:		mingw32-lib3270-%{MAJOR_VERSION}_%{MINOR_VERSION} = %{version}

BuildRequires:	autoconf
BuildRequires:	automake
BuildRequires:	rsvg-view
BuildRequires:	gettext-tools
BuildRequires:	glib2-devel

#BuildRequires:	gtk3-devel
BuildRequires:	pkgconfig(gtk+-3.0)

BuildRequires:  desktop-file-utils
BuildRequires:	optipng

BuildRequires:	mingw32-cross-binutils
BuildRequires:	mingw32-cross-gcc
BuildRequires:	mingw32-cross-gcc-c++
BuildRequires:	mingw32-cross-pkg-config
BuildRequires:	mingw32-filesystem
BuildRequires:	mingw32-libopenssl-devel
BuildRequires:	mingw32-zlib-devel
#BuildRequires:	mingw32-gtk3-devel

BuildRequires:	mingw32(pkg:gtk+-win32-3.0)

#Patch0:			force_inet_ntop.patch

#---------------------------------------------------------------------------------------------------------------------

%description
Open-source GTK-based IBM 3270 terminal emulator with many advanced features. It can be used to communicate with any IBM host that supports 3270-style connections over TELNET.
Based on the original x3270 code, pw3270 was originally created for Banco do Brasil, and is now used worldwide. 

#--[ lib3270 ]--------------------------------------------------------------------------------------------------------

%package -n mingw32-lib3270-%{MAJOR_VERSION}_%{MINOR_VERSION}
Summary:	3270 Communication library for %{name}
Group:		Development/Libraries/C and C++

Provides:	mingw32-lib3270 = %{version}
Provides:	mingw32(lib:3270) = %{version}

%description -n mingw32-lib3270-%{MAJOR_VERSION}_%{MINOR_VERSION}
Open-source GTK-based IBM 3270 terminal emulator with many advanced features. It can be used to communicate with any IBM host that supports 3270-style connections over TELNET.

This package contains the tn3270 protocol library for %{name}

#--[ Devel ]----------------------------------------------------------------------------------------------------------

%package -n mingw32-lib3270-devel
Summary:        Devel for 3270 Communication library for %{name}
Group:          Development/Libraries/C and C++

Requires:       mingw32-lib3270-%{MAJOR_VERSION}_%{MINOR_VERSION} = %{version}
Provides:       mingw32-lib3270-devel-%{MAJOR_VERSION}_%{MINOR_VERSION} = %{version}
Requires:       mingw32-lib3270-%{MAJOR_VERSION}_%{MINOR_VERSION} = %{version}

%description -n mingw32-lib3270-devel
Open-source GTK-based IBM 3270 terminal emulator with many advanced features. It can be used to communicate with any IBM host that supports 3270-style connections over TELNET.
This package contains the development files for tn3270 protocol library for %{name}

%package -n %{name}-devel
Summary:	Files required for development of %{name} plugins
Group:		Development/Libraries/C and C++

Requires:   mingw32(lib:3270) = %{version}
Requires:   mingw32(lib:pw3270) = %{version}

%description -n %{name}-devel
Open-source GTK-based IBM 3270 terminal emulator with many advanced features. It can be used to communicate with any IBM host that supports 3270-style connections over TELNET.

This package contains the development files for %{name}

#---[ Build & Install ]-----------------------------------------------------------------------------------------------

%prep

%setup -q -n pw3270-%{version}
#%patch0 -p0

echo "m4_define([SVN_REVISION], %{release})" > revision.m4
echo "m4_define([SVN_URL], http://softwarepublico.gov.br/gitlab/pw3270/principal.git)" >> revision.m4
echo "m4_define([APP_LEVEL], 0)" >> revision.m4

find . -exec touch {} \;
aclocal
autoconf
%{_mingw32_configure}

%build
make clean
make all

%{_mingw32_strip} --strip-all .bin/Release/*.dll.%{MAJOR_VERSION}.%{MINOR_VERSION}
%{_mingw32_strip} --strip-all .bin/Release/*.exe
%{_mingw32_strip} --strip-all .bin/Release/plugins/*.dll

%install
%{_mingw32_makeinstall}

rm -f %{buildroot}%{_mingw32_datadir}/pw3270/ui/80javasamples.xml
rm -f %{buildroot}%{_mingw32_datadir}/pw3270/ui/80rexx.xml


%clean
rm -rf %{buildroot}

#---[ Files ]---------------------------------------------------------------------------------------------------------

%files
%defattr(-,root,root)
%doc AUTHORS LICENSE 
# %{_mingw32_mandir}/*/*

# Main application
%dir %{_mingw32_datadir}/pw3270
%dir %{_mingw32_datadir}/pw3270/ui
%{_mingw32_bindir}/pw3270.exe
%{_mingw32_libdir}/pw3270.dll
%{_mingw32_libdir}/pw3270.dll.%{MAJOR_VERSION}
%{_mingw32_libdir}/pw3270.dll.%{MAJOR_VERSION}.%{MINOR_VERSION}

%{_mingw32_datadir}/pw3270/ui/00default.xml
%{_mingw32_datadir}/pw3270/ui/10functions.xml
%{_mingw32_datadir}/pw3270/ui/10keypad.xml
%{_mingw32_datadir}/pw3270/colors.conf
%{_mingw32_datadir}/pw3270/pw3270.png
%{_mingw32_datadir}/pw3270/pw3270-logo.png

%dir %{_mingw32_datadir}/locale
%dir %{_mingw32_datadir}/locale/pt_BR
%dir %{_mingw32_datadir}/locale/pt_BR/LC_MESSAGES
%{_mingw32_datadir}/locale/pt_BR/LC_MESSAGES/pw3270.mo

%dir %{_mingw32_libdir}/pw3270-plugins
%{_mingw32_libdir}/libhllapi.dll
%{_mingw32_libdir}/pw3270-plugins/hllapi.dll

%files -n mingw32-lib3270-%{MAJOR_VERSION}_%{MINOR_VERSION}
%defattr(-,root,root)
%{_mingw32_libdir}/lib3270.dll.%{MAJOR_VERSION}.%{MINOR_VERSION}
%{_mingw32_libdir}/lib3270.dll.%{MAJOR_VERSION}
%{_mingw32_libdir}/lib3270.dll

%files -n mingw32-lib3270-devel
%defattr(-,root,root)
%{_mingw32_includedir}/lib3270
%{_mingw32_includedir}/lib3270.h
%{_mingw32_libdir}/pkgconfig/lib3270.pc

%files -n %{name}-devel
%defattr(-,root,root)
%{_mingw32_includedir}/pw3270
%{_mingw32_includedir}/pw3270.h
%{_mingw32_libdir}/pkgconfig/pw3270.pc
%{_mingw32_datadir}/pw3270/ui/98trace.xml
%{_mingw32_datadir}/pw3270/ui/99debug.xml

%dir %{_mingw32_datadir}/pw3270/locale
%{_mingw32_datadir}/pw3270/locale/Makefile
%{_mingw32_datadir}/pw3270/locale/pt_BR.po
%{_mingw32_datadir}/pw3270/locale/pw3270.pot

%{_mingw32_includedir}/pw3270cpp.h
%{_mingw32_libdir}/libpw3270cpp.a

%changelog

